/* 
 *----------------------------------------------------------------------------
 * SimpleOculusRift
 * ----------------------------------------------------------------------------
 * Copyright (C) 2014 Max Rheiner / Interaction Design Zhdk
 *
 * This file is part of SimpleOculusRift.
 *
 * SimpleOpenNI is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version (subject to the "Classpath" exception
 * as provided in the LICENSE.txt file that accompanied this code).
 *
 * SimpleOpenNI is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with SimpleOpenNI.  If not, see <http://www.gnu.org/licenses/>.
 * ----------------------------------------------------------------------------
 */

#ifdef WIN32
#define _CRT_SECURE_NO_WARNINGS
#define NOMINMAX	// eigen + windows.h will have conflict without this

#include <direct.h>
#endif

#include <cmath>

#ifdef __APPLE__
    #include <unistd.h>
#elif _WIN32

#else
    #include <unistd.h>
#endif

#include <stdio.h>
#include <stdarg.h>
#include <fstream>

#include "ContextWrapper.h"

using namespace sOR;

#define		SIMPLEOCULUSRIFT_VERSION	20		// 1234 = 12.24

// if you plan to debug on linux, don't forget to call the following commands, otherwise you want be able to
// attacht to the shared library
// echo 0 | sudo tee /proc/sys/kernel/yama/ptrace_scope
// http://www.deder.at/wordpress/?p=307

///////////////////////////////////////////////////////////////////////////////
// ContextWrapper

bool ContextWrapper::_globalContextFlag = false;

GLint uniforms[6];

ContextWrapper::ContextWrapper():
  _initFlag(false),
  _firstTimeUpdate(false),
  _sensorFusion(NULL),
  _frameBuffer(0),
  _frameBufferTexture(0),
  _frameBufferDepth(0),
  _shaderProgram(0),
/*
  _frameBufferTexW(1024),
  _frameBufferTexH(1024)
*/
  _frameBufferTexW(1024*4),
  _frameBufferTexH(1024*4),
  _headTracking(true)
{
    _clearColor[0] = 0;
    _clearColor[1] = 0;
    _clearColor[2] = 0;
}

ContextWrapper::~ContextWrapper()
{
    close();
}

void ContextWrapper::chdir(const char* dir)
{
#ifdef WIN32
    int ret = _chdir(dir);
#else
    int ret = ::chdir(dir);
#endif
}

std::string ContextWrapper::getcwd()
{
    char buf[1024];
    if(::getcwd(buf, 1024) != NULL)
        return std::string(buf);
    else
        return std::string("");
}

void ContextWrapper::setDataFolder(const char* dir)
{
    _dataFolder = dir;
    std::cout << dir << std::endl;
}

void ContextWrapper::close()
{
    if(!_initFlag)
        return;

    _initFlag = false;

    // close the rest
    OVR::MessageHandler::RemoveHandlerFromDevices();

    if(_sensorFusion)
    {
        // Detach sensor
        _sensorFusion->AttachToSensor(NULL);
        delete _sensorFusion;
        _sensorFusion = NULL;
    }

    _sensor.Clear();
    _hmd.Clear();
    _deviceManager.Clear();

    OVR::System::Destroy();
    OVR_DEBUG_STATEMENT(_CrtDumpMemoryLeaks());
}

int ContextWrapper::version() 
{ 
    return SIMPLEOCULUSRIFT_VERSION;
}

void ContextWrapper::logOut(int msgType,const char* msg,...)
{
    char strBuffer[STRING_BUFFER];

    switch(msgType)
    {
    case Msg_Error:
        std::cout << "SimpleOculusRift Error: ";
        break;
    case Msg_Deprecated:
        std::cout << "SimpleOculusRift Deprecated: ";
        break;
    case Msg_Info:
    default:
        std::cout << "SimpleOculusRift Info: ";
        break;
    };

    va_list args;
    va_start(args, msg);
    vsnprintf(strBuffer,STRING_BUFFER,msg, args);
    va_end(args);

    std::cout << strBuffer << std::endl;
}

void ContextWrapper::logDeprecated(int msgType,const char* oldFunc,const char* newFunc)
{
     logOut(Msg_Deprecated,"%s is deprecated, please use %s.",oldFunc,newFunc);
}

bool ContextWrapper::initContext()
{
    if(_globalContextFlag == false)
    {   // create context
        std::cout << "SimpleOculusRift Version " << (SIMPLEOCULUSRIFT_VERSION / 100) << "." <<  (SIMPLEOCULUSRIFT_VERSION % 100) << std::endl;

        _globalContextFlag = true;

        return _globalContextFlag;
    }
    else
        return _globalContextFlag;
}

bool ContextWrapper::init(int w,int h,int qualtiy)
{
    if(initContext() == false)
    {
        logOut(Msg_Error,"ContextWrapper::init\n");
        return false;
    }

    _w = w;
    _h = h;

    // set quality
    switch(qualtiy)
    {
    case ContextWrapper::High_Quality:
        _frameBufferTexW = 1024*4;
        _frameBufferTexH = 1024*4;
        break;
    case Middle_Quality:
        _frameBufferTexW = 1024*2;
        _frameBufferTexH = 1024*2;
        break;
    case VeryLow_Quality:
        _frameBufferTexW = 512;
        _frameBufferTexH = 512;
        break;
    case Low_Quality:
    default:
        _frameBufferTexW = 1024*1;
        _frameBufferTexH = 1024*1;
        break;
    }

    // start the oculus
    OVR::System::Init(OVR::Log::ConfigureDefaultLog(OVR::LogMask_All));

    _deviceManager = *OVR::DeviceManager::Create();

    // We'll handle it's messages in this case.
    _deviceManager->SetMessageHandler(this);

    // Release Sensor/HMD in case this is a retry.
    _sensor.Clear();
    _hmd.Clear();

    _hmd = *_deviceManager->EnumerateDevices<OVR::HMDDevice>().CreateDevice();
    if(_hmd)
    {
        _sensor = *_hmd->GetSensor();

        // This will initialize HMDInfo with information about configured IPD,
        // screen size and other variables needed for correct projection.
        // We pass HMD DisplayDeviceName into the renderer to select the
        // correct monitor in full-screen mode.
        _hmd->GetDeviceInfo(&_hmdInfo);

        _stereoConfig.SetHMDInfo(_hmdInfo);
        //_stereoConfig.SetFullViewport(OVR::Util::Render::Viewport(0,0, _w, _h));
        _stereoConfig.SetFullViewport(OVR::Util::Render::Viewport(0,0, 2.0*_hmdInfo.HResolution, _hmdInfo.VResolution));
        _stereoConfig.SetStereoMode(OVR::Util::Render::Stereo_LeftRight_Multipass);

        if (_hmdInfo.HScreenSize > 0.140f) // 7"
            _stereoConfig.SetDistortionFitPointVP(-1.0f, 0.0f);
        else
            _stereoConfig.SetDistortionFitPointVP(0.0f, 1.0f);
    }
    else
    {
        // If we didn't detect an HMD, try to create the sensor directly.
        // This is useful for debugging sensor interaction; it is not needed in
        // a shipping app.
        _sensor = _deviceManager->EnumerateDevices<OVR::SensorDevice>().CreateDevice();
    }

    const char* detectionMessage=0;
    if(!_hmd && !_sensor)
        detectionMessage = "Oculus Rift not detected.";
    else if (!_hmd)
        detectionMessage = "Oculus Sensor detected; HMD Display not detected.";
    else if (!_sensor)
        detectionMessage = "Oculus HMD Display detected; Sensor not detected.";
    else if (_hmdInfo.DisplayDeviceName[0] == '\0')
        detectionMessage = "Oculus Sensor detected; HMD display EDID not detected.";
    else
        detectionMessage = 0;

    if(detectionMessage)
    {
        logOut(Msg_Error,"Oculus Rift ERROR:  %s\n",detectionMessage);
    }

    if(_sensor)
    {
        // We need to attach sensor to SensorFusion object for it to receive
        // body frame messages and update orientation. SFusion.GetOrientation()
        // is used in OnIdle() to orient the view.
        _sensorFusion = new OVR::SensorFusion;
        _sensorFusion->AttachToSensor(_sensor);
        _sensorFusion->SetPredictionEnabled(true);
        _sensorFusion->SetDelegateMessageHandler(this);
    }

    if(_hmd == NULL)
    {
        logOut(Msg_Error,"Oculus Rift : no HMD around, set default values");
        /// If the HMD is turned off or not present, fill in some halfway sensible
        /// default values here so we can at least see some rendering.
        ///@todo Grab these values from the HMD setup, see how we did on guesing
        OVR::HMDInfo& hmd = _hmdInfo;
        hmd.DesktopX = 0;
        hmd.DesktopY = 0;
        hmd.HResolution = 1280;
        hmd.VResolution = 800;

        hmd.HScreenSize = 0.09f;
        hmd.VScreenSize = 0.06f;
        hmd.VScreenCenter = 0.02f;

        hmd.DistortionK[0] = 1.0f;
        hmd.DistortionK[1] = 0.5f;
        hmd.DistortionK[2] = 0.25f;
        hmd.DistortionK[3] = 0.0f;

        hmd.EyeToScreenDistance = 0.01f;
        hmd.InterpupillaryDistance = 0.064f;
        hmd.LensSeparationDistance = 0.07f;
    }
    ////////////////////////////////////////////////////////////////////////////
    // setup opengl
    setupShaders();
    setupFrameBuffer();

    _initFlag = true;



    return true;
}


void applyGlMatrix(const OVR::Matrix4f& matrix)
{
        glMultMatrixf(&(matrix.Transposed().M[0][0]));
}

void ContextWrapper::setupCamera(OVR::Util::Render::StereoEye eye)
{
    const OVR::Util::Render::StereoEyeParams& params = _stereoConfig.GetEyeRenderParams(eye);
    glViewport(0,0,_frameBufferTexW,_frameBufferTexH);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    applyGlMatrix(params.ViewAdjust);
    applyGlMatrix(params.Projection);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    OVR::Matrix4f eye_view = eyeView(eye);
    applyGlMatrix(eye_view);
}

void ContextWrapper::getMatrix(int eye,
                               float* proj,
                               float* modelView)
{
    const OVR::Util::Render::StereoEyeParams& params = _stereoConfig.GetEyeRenderParams((OVR::Util::Render::StereoEye)eye);

    glViewport(0,0,_frameBufferTexW,_frameBufferTexH);

    //OVR::Matrix4f p = params.ViewAdjust * params.Projection;
    OVR::Matrix4f p = params.Projection * params.ViewAdjust;
    memcpy(proj,p.M,sizeof(float) * 16);

    OVR::Matrix4f eye_view = eyeView((OVR::Util::Render::StereoEye)eye);
    memcpy(modelView,eye_view.M,sizeof(float) * 16);
}


OVR::Matrix4f ContextWrapper::eyeView(OVR::Util::Render::StereoEye eye)
{
    if(!_headTracking)
        return OVR::Matrix4f();

    static const OVR::Vector3f UpVector(0.0f, 1.0f, 0.0f);
    static const OVR::Vector3f ForwardVector(0.0f, 0.0f, -1.0f);
    static const OVR::Vector3f RightVector(1.0f, 0.0f, 0.0f);

    float yaw, pitch, roll;
    sensorOrientation(yaw, pitch, roll);

    OVR::Matrix4f eye_rpy = OVR::Matrix4f::RotationY(yaw) *
                            OVR::Matrix4f::RotationX(pitch) *
                            OVR::Matrix4f::RotationZ(roll);
    /*

    OVR::Matrix4f eye_rpy = OVR::Matrix4f::RotationZ(roll) *
                            OVR::Matrix4f::RotationX(pitch) *
                            OVR::Matrix4f::RotationY(yaw);
*/


    OVR::Vector3f eye_pos       = _position;
    OVR::Vector3f eye_forward   = eye_rpy.Transform(ForwardVector);
    OVR::Vector3f eye_up        = eye_rpy.Transform(UpVector);
    OVR::Vector3f eye_right     = eye_rpy.Transform(RightVector);
    OVR::Matrix4f eye_view      = OVR::Matrix4f::LookAtRH(eye_pos, eye_pos + eye_forward, eye_up);

    return eye_view;
}

void ContextWrapper::setupShaders()
{
    // initialise glew API.
    glewExperimental = GL_TRUE;
    glewInit();

    const char* vertexShader    = oculusRiftVertexShader;
    const char* fragShader      = oculusRiftFragmentShader;

    // Now create the shaders
    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    GLint Result = GL_FALSE;
    int InfoLogLength;

    // Compile Vertex Shader
    logOut(Msg_Info,"Compiling vertex shader");
    glShaderSource(VertexShaderID, 1, &vertexShader , NULL);
    glCompileShader(VertexShaderID);

    // Check Vertex Shader
    glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if( InfoLogLength > 0 )
    {
        std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
        glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
        logOut(Msg_Info,"%s", &VertexShaderErrorMessage[0]);
    }


    // Compile Fragment Shader
    logOut(Msg_Info,"Compiling fragment shader");
    glShaderSource(FragmentShaderID, 1, &fragShader , NULL);

    glCompileShader(FragmentShaderID);

    // Check Fragment Shader
    glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if( InfoLogLength > 0 )
    {
        std::vector<char> FragmentShaderErrorMessage(InfoLogLength+1);
        glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
        logOut(Msg_Info,"%s", &FragmentShaderErrorMessage[0]);
    }


    // Link the program
    logOut(Msg_Info,"Linking program");
    _shaderProgram = glCreateProgram();
    glAttachShader(_shaderProgram, VertexShaderID);
    glAttachShader(_shaderProgram, FragmentShaderID);
    glLinkProgram(_shaderProgram);

    // Check the program
    glGetProgramiv(_shaderProgram, GL_LINK_STATUS, &Result);
    glGetProgramiv(_shaderProgram, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if( InfoLogLength > 0 )
    {
        std::vector<char> ProgramErrorMessage(InfoLogLength+1);
        glGetProgramInfoLog(_shaderProgram, InfoLogLength, NULL, &ProgramErrorMessage[0]);
        logOut(Msg_Info,"%s", &ProgramErrorMessage[0]);
    }

    // setup uniforms
    uniforms[0] = glGetUniformLocation(_shaderProgram, "Texture");
    uniforms[1] = glGetUniformLocation(_shaderProgram, "LensCenter");
    uniforms[2] = glGetUniformLocation(_shaderProgram, "ScreenCenter");
    uniforms[3] = glGetUniformLocation(_shaderProgram, "Scale");
    uniforms[4] = glGetUniformLocation(_shaderProgram, "ScaleIn");
    uniforms[5] = glGetUniformLocation(_shaderProgram, "HmdWarpParam");


    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);

}

void ContextWrapper::setupFrameBuffer()
{

    // The texture we're going to render to
    glGenTextures(1, &_frameBufferTexture);

    // "Bind" the newly created texture : all future texture functions will modify this texture
    glBindTexture(GL_TEXTURE_2D, _frameBufferTexture);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, _frameBufferTexW, _frameBufferTexH, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

    glGenFramebuffers(1, &_frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, _frameBuffer);

    // The depth buffer
    glGenRenderbuffers(1, &_frameBufferDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, _frameBufferDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, _frameBufferTexW, _frameBufferTexH);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _frameBufferTexture, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _frameBufferDepth);


    // Always check that our framebuffer is ok
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        logOut(Msg_Error,"[FRAMEBUFFER] error.");
    }
}

void ContextWrapper::draw()
{    
    glPushAttrib( GL_TEXTURE_BIT | GL_DEPTH_TEST | GL_LIGHTING );

    glClearColor(_clearColor[0],_clearColor[1],_clearColor[2], 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    /*
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
**/

    renderEye(OVR::Util::Render::StereoEye_Left);
    renderEye(OVR::Util::Render::StereoEye_Right);

    glPopAttrib();
}

void perspective(double fovY , double aspect, double zNear ,double zFar)
{
    const double pi = 3.14159265359;
    double fW, fH;
    fH = tan(fovY/360.*pi) * zNear;
    fW = fH * aspect;
    glFrustum(-fW, fW, -fH, fH, zNear, zFar);
}


void ContextWrapper::renderEye(OVR::Util::Render::StereoEye eye)
{
    const OVR::Util::Render::StereoEyeParams& params = _stereoConfig.GetEyeRenderParams(eye);
    if(params.pDistortion == NULL)
        return;


    GLint oldFBO;
    GLint oldTEX;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &oldFBO);
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &oldTEX);

    ///////////////////////////////////////////////////////////////////////////
    // setup scene to render to texture
    glViewport(0, 0, _frameBufferTexW, _frameBufferTexH);
    glMatrixMode (GL_PROJECTION);

    glLoadIdentity ();

    glMatrixMode (GL_MODELVIEW);
    // setup texture
    glBindFramebuffer(GL_FRAMEBUFFER, _frameBuffer);


    glClearColor(_clearColor[0],_clearColor[1],_clearColor[2], 1);
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //draw scene to square texture
    glLoadIdentity ();


    // offset by interpupillary distance
    if(eye == OVR::Util::Render::StereoEye_Left)
        glTranslatef(_hmdInfo.InterpupillaryDistance, 0.0f, 0.0f);
    else if (eye == OVR::Util::Render::StereoEye_Right)
        glTranslatef(-_hmdInfo.InterpupillaryDistance, 0.0f, 0.0f);


    glPushMatrix();


    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    applyGlMatrix(params.ViewAdjust);
    applyGlMatrix(params.Projection);

    //perspective(190.0f,(1280/2.0f)/(float)(800),.1f,10000.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    OVR::Matrix4f eye_view = eyeView((OVR::Util::Render::StereoEye)eye);
    applyGlMatrix(eye_view);


    // draw scene
    drawScene(eye);

    glPopMatrix();

    // unbind framebuffer, now render to screen
    glBindFramebuffer(GL_FRAMEBUFFER, oldFBO);

    ///////////////////////////////////////////////////////////////////////////
    //

    glBindTexture(GL_TEXTURE_2D, _frameBufferTexture);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);

    if(eye == OVR::Util::Render::StereoEye_Left)                        // left screen
        glViewport (0, 0, _w/2., _h);
    else if (eye == OVR::Util::Render::StereoEye_Right)                  // right screen
        glViewport ( _w/2., 0,  _w/2., _h);

    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    glMatrixMode (GL_MODELVIEW);

    glLoadIdentity();

    glUseProgram(_shaderProgram);

    // preset suggestions from http://www.mtbs3d.com/phpbb/viewtopic.php?f=140&t=17081
    const float Scale[2] = {0.1469278, 0.2350845};
    const float ScaleIn[2] = {2, 2.5};
    const float HmdWarpParam[4] = {1, 0.22, 0.24, 0};
    const float LeftLensCenter[2] = {0.2863248*2.0, 0.5};
    const float LeftScreenCenter[2] = {0.55, 0.5};
    const float RightLensCenter[2] = {(0.7136753-.5) * 2.0, 0.5};
    const float RightScreenCenter[2] = {0.45, 0.5};

    // apply shader uniforms
    glUniform2fv(uniforms[3], 1, Scale);
    glUniform2fv(uniforms[4], 1, ScaleIn);
    glUniform4fv(uniforms[5], 1, HmdWarpParam);
    if(eye == OVR::Util::Render::StereoEye_Left)
    {
        glUniform2fv(uniforms[1], 1, LeftLensCenter);
        glUniform2fv(uniforms[2], 1, LeftScreenCenter);
    }
    else
    {
        glUniform2fv(uniforms[1], 1, RightLensCenter);
        glUniform2fv(uniforms[2], 1, RightScreenCenter);
    }

 //   setEyeUniform(eye,params);


    // draw scene on a quad for each side
    glBindTexture(GL_TEXTURE_2D, _frameBufferTexture);

    glColor4f(1, 1, 1, 1);

    glBegin(GL_TRIANGLES);
    glNormal3f(0,0,1);
    glTexCoord2f(1,1);  glVertex3f(1,1,0);
    glTexCoord2f(0,1);  glVertex3f(-1,1,0);
    glTexCoord2f(0,0);  glVertex3f(-1,-1,0);
    glTexCoord2f(0,0);  glVertex3f(-1,-1,0);
    glTexCoord2f(1,0);  glVertex3f(1,-1,0);
    glTexCoord2f(1,1);  glVertex3f(1,1,0);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, 0);
}

void ContextWrapper::setEyeUniform(OVR::Util::Render::StereoEye eye,const OVR::Util::Render::StereoEyeParams& params)
{
    // apply distortion for each eye.
    if(params.pDistortion)
    {
        GLsizei width =_hmdInfo.HResolution;
        GLsizei height =_hmdInfo.VResolution;
        const OVR::Util::Render::DistortionConfig& distortion = *params.pDistortion;

        float w  = float(params.VP.w) / float(width),
              h  = float(params.VP.h) / float(height),
              x  = float(params.VP.x) / float(width),
              y  = float(params.VP.y) / float(height);
        float as = float(params.VP.w) / float(params.VP.h);

        // We are using 1/4 of DistortionCenter offset value here, since it is
        // relative to [-1,1] range that gets mapped to [0, 0.5].
        float HmdWarpParam[4]   = { distortion.K[0], distortion.K[1], distortion.K[2], distortion.K[3] };
        float ChromAbParam[4]   = { distortion.ChromaticAberration[0], distortion.ChromaticAberration[1], distortion.ChromaticAberration[2], distortion.ChromaticAberration[3] };
        float scaleFactor       = 1.0f / distortion.Scale;
        OVR::Vector2f LensCenter        (x + (w + (eye == OVR::Util::Render::StereoEye_Right ? - distortion.XCenterOffset: distortion.XCenterOffset) * 0.5f)*0.5f, y + h*0.5f);
        OVR::Vector2f ScreenCenter      (x + w*0.5f, y + h*0.5f);
        OVR::Vector2f Scale             ((w/2.0f) * scaleFactor, (h/2.0f) * scaleFactor * as);
        OVR::Vector2f ScaleIn           ((2.0f/w), (2.0f/h) / as);

        // fragment shader.
 //       gl_uniform_1i("WarpTexture", 0);
 //       gl_uniform_1i("tex", 0);

       //ScreenCenter = OVR::Vector2f(x+w, y+h);
        //ScreenCenter    = (x + w*0.5f, y + h*0.5f);
  //      LensCenter      = OVR::Vector2f(x + (w + (eye == OVR::Util::Render::StereoEye_Right ? - distortion.XCenterOffset: distortion.XCenterOffset))*1.0f, y + h*1.0f);
  //      ScaleIn         = OVR::Vector2f((1.0f / w), (2.0f/h) / as);
     //   LensCenter      = OVR::Vector2f(0.2863248*2.0, 0.5);
        ScreenCenter    = OVR::Vector2f(0.55, 0.5);
        ScaleIn         = OVR::Vector2f(1.0f / w ,2.0f / h / as);
     //   Scale           = OVR::Vector2f(0.1469278, 0.2350845 * 3.0f);

        gl_uniform_2f("LensCenter",     LensCenter.x,    LensCenter.y);
        gl_uniform_2f("ScreenCenter",   ScreenCenter.x,  ScreenCenter.y);
        gl_uniform_2f("Scale",          Scale.x,         Scale.y);
        gl_uniform_2f("ScaleIn",        ScaleIn.x,       ScaleIn.y);
        gl_uniform_4f("HmdWarpParam",   HmdWarpParam[0], HmdWarpParam[1], HmdWarpParam[2], HmdWarpParam[3]);
        gl_uniform_4f("ChromAbParam",   ChromAbParam[0], ChromAbParam[1], ChromAbParam[2], ChromAbParam[3]);

        static bool firstTime = false;
        if(!firstTime)
        {
          //  firstTime = true;
            std::cout << "----------------------" << std::endl;

            std::cout << " distortion.Scale: " <<   distortion.Scale << std::endl;
            std::cout << "val: " <<  x << ","<<  y << ","<<  w << ","<<  h << std::endl;
            std::cout << "_hmdInfo.resolution: " <<  _hmdInfo.HResolution << "," << _hmdInfo.VResolution << std::endl;
            std::cout << "params: " <<  params.VP.x << "," << params.VP.y << "," << params.VP.w << "," << params.VP.h << std::endl;
            std::cout << "LensCenter: " <<  LensCenter.x << "," << LensCenter.y << std::endl;
            std::cout << "ScreenCenter: " <<  ScreenCenter.x << "," << ScreenCenter.y << std::endl;
            std::cout << "Scale: " <<  Scale.x << "," << Scale.y << std::endl;
            std::cout << "ScaleIn: " <<  ScaleIn.x << "," << ScaleIn.y << std::endl;
            std::cout << "HmdWarpParam: " <<  HmdWarpParam[0] << "," << HmdWarpParam[1]<< "," << HmdWarpParam[2]<< "," << HmdWarpParam[3] << std::endl;
            std::cout << "ChromAbParam: " <<  ChromAbParam[0] << "," << ChromAbParam[1]<< "," << ChromAbParam[2]<< "," << ChromAbParam[3] << std::endl;
        }
    }
}


void ContextWrapper::drawScene(int eye)
{


}
