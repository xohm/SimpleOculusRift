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

// This code is based on the example from Olivier Renault.
// http://pastebin.com/2VEasjbu


#ifndef _CONTEXWRAPPER_H_
#define _CONTEXWRAPPER_H_

#ifdef WIN32
	#pragma warning( disable : 4251 )	// disable warnings
	#pragma warning( disable : 4275 )
#endif

// opengl, glew
#ifdef __APPLE__
    #include <GL/glew.h>
#else
    // opengl, glew
    #include <GL/glew.h>
    #include <GL/gl.h>
    #include <GL/glu.h>
#endif


// Oculus Rift
#include <OVR.h>

// stl
#include <vector>
#include <iostream>

// eigen
#include <Eigen/Geometry>

#include "oculusRiftShaders.h"

///////////////////////////////////////////////////////////////////////////////
// defines

#define		MAX_DEPTH		10000	// 10m
#define		STRING_BUFFER	255

#define SOPENNI_CB_VIRTUAL(FuncName,...)\
    protected:\
    virtual void on##FuncName##Cb(__VA_ARGS__){;}

#define SOPENNI_CB_CALL(FuncName,...)\
    on##FuncName##Cb(__VA_ARGS__);

// added this typedef because of problems with virtal methods and the %apply
typedef const float* constPFloat;

namespace sOR{

class Vec3f
{
public:
    Vec3f(float x,float y,float z)
    {
        vec[0] = x;
        vec[1] = y;
        vec[2] = z;

    }

    Vec3f(const Vec3f& copy)
    {
        vec[0] = copy.vec[0];
        vec[1] = copy.vec[1];
        vec[2] = copy.vec[2];
    }

    float x() const { return vec[0]; }
    float y() const { return vec[1]; }
    float z() const { return vec[2]; }

    const float* get() const { return vec; }

protected:
    float vec[3];
};

//////////////////////////////////////////////////////////////////////////////
// ContextWrapper

struct EyePatch
{
    // various patches for eyes.
    void setup(OVR::Util::Render::StereoEye eye)
    {
        static float g_vertex_buffer_data[3][4][3]=
        {
            // centre eye
            {       { -1.0f, -1.0f, 0.0f, },
                    {  1.0f, -1.0f, 0.0f, },
                    {  1.0f,  1.0f, 0.0f, },
                    { -1.0f,  1.0f, 0.0f, }, },

            // left eye
            {       { -1.0f, -1.0f, 0.0f, },
                    {  0.0f, -1.0f, 0.0f, },
                    {  0.0f,  1.0f, 0.0f, },
                    { -1.0f,  1.0f, 0.0f, }, },

            // right eye
            {       {  0.0f, -1.0f, 0.0f, },
                    {  1.0f, -1.0f, 0.0f, },
                    {  1.0f,  1.0f, 0.0f, },
                    {  0.0f,  1.0f, 0.0f, }, },
        };

        static float g_uv_buffer_data[3][4][2] =
        {
            // center eye
            {       { 0.0f, 0.0f, },
                    { 1.0f, 0.0f, },
                    { 1.0f, 1.0f, },
                    { 0.0f, 1.0f, }, },

            // left eye
            {       { 0.0f, 0.0f, },
                    { 0.5f, 0.0f, },
                    { 0.5f, 1.0f, },
                    { 0.0f, 1.0f, }, },

            // right eye
            {       { 0.5f, 0.0f, },
                    { 1.0f, 0.0f, },
                    { 1.0f, 1.0f, },
                    { 0.5f, 1.0f, }, }
        };

        // load up the eye quad.
        glGenVertexArrays(1, &_vertexArrays);
        glBindVertexArray(_vertexArrays);

        glGenBuffers(1, &_vertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, _vertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data[eye]), &(g_vertex_buffer_data[eye][0][0]), GL_STATIC_DRAW);

        glGenBuffers(1, &_uvBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, _uvBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(g_uv_buffer_data[eye]), &(g_uv_buffer_data[eye][0][0]), GL_STATIC_DRAW);
    }

    void render()
    {
        // render the quad for the eye patch on Oculus display.
        glBindBuffer(GL_ARRAY_BUFFER, _vertexBuffer);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

        glBindBuffer(GL_ARRAY_BUFFER, _uvBuffer);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

        glDrawArrays(GL_QUADS, 0, 4);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
    }

    void cleanup()
    {
        glDeleteBuffers(1, &_vertexBuffer);
        glDeleteBuffers(1, &_uvBuffer);
        glDeleteVertexArrays(1, &_vertexArrays);
    }

    GLuint _vertexArrays;
    GLuint _vertexBuffer;
    GLuint _uvBuffer;
};

//////////////////////////////////////////////////////////////////////////////
// ContextWrapper

class ContextWrapper: public OVR::MessageHandler
{
public:

    ContextWrapper();
    virtual ~ContextWrapper();

    int version();

    static void chdir(const char* dir);
    static std::string getcwd();

    //////////////////////////////////////////////////////////////////////////////
    // init methods
    static bool initContext();
    bool init();

    bool isInit(){	return _initFlag; }

    void close();

    virtual void update()
    {}

    virtual void OnMessage(const OVR::Message& msg)
    {
        if(msg.pDevice == _sensor)
        {
            if (msg.Type == OVR::Message_BodyFrame)
            {
            }
        }
        else if(msg.pDevice == _deviceManager)
        {
            if (msg.Type == OVR::Message_DeviceAdded)
            {
                logOut(Msg_Info,"DeviceManager reported device added.");
            }
            else if (msg.Type == OVR::Message_DeviceRemoved)
            {
                logOut(Msg_Info,"DeviceManager reported device removed.");
            }
        }
    }

    bool sensorOrientation(float& yaw, float& pitch, float& roll) const
    {
        yaw = 0.0f;
        pitch = 0.0f;
        roll = 0.0f;

        if(_sensorFusion== NULL || !_sensorFusion->IsAttachedToSensor())
            return false;

        OVR::Quatf hmdOrient = _sensorFusion->GetOrientation();
        hmdOrient.GetEulerAngles<OVR::Axis_Y, OVR::Axis_X, OVR::Axis_Z>(&yaw, &pitch, &roll);

        return true;
    }

    bool sensorOrientation(float* orientation) const
    {
        sensorOrientation(orientation[0],orientation[1],orientation[2]);
    }

    const OVR::HMDInfo& hmdInfo() const
    {
        return _hmdInfo;
    }

    OVR::Util::Render::StereoConfig stereoConfig() const
    {
        return _stereoConfig;
    }

    void setupCamera(OVR::Util::Render::StereoEye eye);
    OVR::Matrix4f eyeView(OVR::Util::Render::StereoEye eye);

    virtual void draw();
    virtual void drawScene(int eye);

    void getMatrix(int eye,
                   float* proj,
                   float* modelView);

protected:    
    enum LogOutMsg{
        Msg_End         = 0,
        Msg_Info        = 1,
        Msg_Error       = 2,
        Msg_Deprecated  = 3,
    };

    static void logOut(int msgType,const char* msg,...);	// must end with null
    static void logDeprecated(int msgType,const char* oldFunc,const char* newFunc);

    void setupShaders();
    void setupFrameBuffer();

    void renderScene2Framebuffer();
    void renderEyePatch(OVR::Util::Render::StereoEye eye);
    void postprocessFramebuffer(void);


    void gl_uniform_2f       (const char* varname, float a, float b) { GLuint varid = glGetUniformLocation(_shaderProgram, varname); glUniform2f(varid, a, b); }
    void gl_uniform_4f       (const char* varname, float a, float b, float c, float d) { GLuint varid = glGetUniformLocation(_shaderProgram, varname); glUniform4f(varid, a, b, c, d); }
    void gl_uniform_1i       (const char* varname, int value) { GLuint varid = glGetUniformLocation(_shaderProgram, varname); glUniform1i(varid, value); }


    bool                    _initFlag;
    bool                    _firstTimeUpdate;

    static bool             _globalContextFlag;    
    

    OVR::Ptr<OVR::DeviceManager>        _deviceManager;
    OVR::Ptr<OVR::SensorDevice>         _sensor;
    OVR::Ptr<OVR::HMDDevice>            _hmd;
    OVR::HMDInfo                        _hmdInfo;
    //OVR::SensorFusion                   _sensorFusion;
    OVR::SensorFusion*                  _sensorFusion;
    OVR::Util::Render::StereoConfig     _stereoConfig;
  
    OVR::Vector3f                       _position;

    // opengl stuff
    GLuint _frameBuffer;
    GLuint _frameBufferTexture;
    GLuint _frameBufferDepth;

    // shader maintenance.
    GLuint _shaderProgram;

    EyePatch _eyePatch[3];
};


}; // namespace sOR




#endif // _CONTEXWRAPPER_H_
