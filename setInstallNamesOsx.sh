#!/bin/sh
# --------------------------------------------------------------------------
# Changes the path to the lib deps on OSX
# --------------------------------------------------------------------------
# Processing Wrapper for the Oculus Rift library
# http://github.com/xohm/SimpleOculusRift
# --------------------------------------------------------------------------
# prog:  Max Rheiner / Interaction Design / zhdk / http://iad.zhdk.ch/
# date:  06/24/2014 (m/d/y)
# ----------------------------------------------------------------------------

cd ./dist/all/SimpleOculusRift/library

echo "--- change libpath for boost_thread ---"
install_name_tool -change /usr/local/lib/libboost_system-mt.dylib @loader_path/./libboost_system-mt.dylib libSimpleOculusRift.jnilib
install_name_tool -change /usr/local/lib/libGLEW.1.10.0.dylib @loader_path/./libGLEW.1.10.0.dylib libSimpleOculusRift.jnilib

