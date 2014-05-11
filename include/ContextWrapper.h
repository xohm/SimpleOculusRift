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

#define		STRING_BUFFER	255

// added this typedef because of problems with virtal methods and the %apply
typedef const float* constPFloat;

namespace sOR{

//////////////////////////////////////////////////////////////////////////////
// ContextWrapper


class ContextWrapper: public OVR::MessageHandler
{
public:

    enum{
        High_Quality    = 0,
        Middle_Quality  = 1,
        Low_Quality     = 2,
        VeryLow_Quality = 3,
        Def_Quality     = Middle_Quality,

    };

    ContextWrapper();
    virtual ~ContextWrapper();

    int version();

    static void chdir(const char* dir);
    static std::string getcwd();

    //////////////////////////////////////////////////////////////////////////////
    // init methods
    static bool initContext();
    bool init(int w,int h,int quality);
    void setDataFolder(const char* dir);

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

    void resetOrientation()
    {
        if(_sensorFusion)
            _sensorFusion->Reset();
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


    void setBknColor(float red,float green,float blue)
    {
        _clearColor[0] = red;
        _clearColor[1] = green;
        _clearColor[2] = blue;
    }

    void setBknColor(int red,int green,int blue)
    {
        _clearColor[0] = (float)red * 1.0f/255.0f;
        _clearColor[1] = (float)green * 1.0f/255.0f;
        _clearColor[2] = (float)blue * 1.0f/255.0f;
    }

    void enableHeadTracking(bool flag)
    {
        _headTracking = flag;
    }

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

    void setEyeUniform(OVR::Util::Render::StereoEye eye,const OVR::Util::Render::StereoEyeParams& params);
    void renderEye(OVR::Util::Render::StereoEye eye);

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
    int     _frameBufferTexW;
    int     _frameBufferTexH;

    // shader maintenance.
    GLuint _shaderProgram;

    std::string _dataFolder;

    float   _w;
    float   _h;

    float   _clearColor[3];
    bool    _headTracking;

};


}; // namespace sOR




#endif // _CONTEXWRAPPER_H_
