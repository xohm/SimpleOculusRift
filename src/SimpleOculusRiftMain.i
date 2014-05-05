# -----------------------------------------------------------------------------
# SimpleOculusRiftMain
# -----------------------------------------------------------------------------
# Processing Wrapper for the Oculus Rift library
# http://github.com/xohm/SimpleOculusRift
# --------------------------------------------------------------------------
# prog:  Max Rheiner / Interaction Design / zhdk / http://iad.zhdk.ch/
# date:  06/24/2014 (m/d/y)
# -----------------------------------------------------------------------------

%module(directors="1") SimpleOculusRift

%{
#include "ContextWrapper.h"

#include <string>
#include <vector>

#include <OVR.h>
%}

%include "arrays_java.i"
%include "cpointer.i"
%include "typemaps.i"
%include "carrays.i"

%apply int[] { int * };
%apply float[] { float * };


# ----------------------------------------------------------------------------
# stl

%include "std_vector.i"
%include "std_string.i"
 
%template(IntVector)	std::vector<int>;
/*
%template(Vector3D)		std::vector<XnVector3D>;
%template(Point3D)		std::vector<XnPoint3D>;
*/

%template(StrVector)	std::vector<std::string>;
 
 
# ----------------------------------------------------------------------------
# ContextWrapper

//typedef const float* constPFloat;

namespace sOR{


class Vec3f
{
public:
    Vec3f(float x,float y,float z);
    Vec3f(const Vec3f& copy);

    float x() const;
    float y() const;
    float z() const;

    //onst float* get();
};


%constant int StereoEye_Left		= OVR::Util::Render::StereoEye_Left;
%constant int StereoEye_Right		= OVR::Util::Render::StereoEye_Right;


%feature("director") ContextWrapper;
class ContextWrapper
{
public:
 
    ContextWrapper();
    virtual ~ContextWrapper();

    int version();

    static void chdir(const char* dir);
    static std::string getcwd();

    bool init();
    void close();
    void setDataFolder(const char* dir);

    virtual void draw();
    virtual void drawScene(int eye);

    bool sensorOrientation(float* orientation) const;
    void resetOrientation();
    void getMatrix(int eye,
                   float* proj,
                   float* modelView);


    void setBknColor(float red,float green,float blue);
    void setBknColor(int red,int green,int blue);

};

};
