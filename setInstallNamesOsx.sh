#!/bin/sh
# --------------------------------------------------------------------------
# Changes the path to the lib deps on OSX
# --------------------------------------------------------------------------
# Processing Wrapper for the OpenNI/Kinect 2.0 library
# http://code.google.com/p/simple-openni
# --------------------------------------------------------------------------
# prog:  Max Rheiner / Interaction Design / zhdk / http://iad.zhdk.ch/
# date:  08/25/2013 (m/d/y)
# ----------------------------------------------------------------------------

cd ./dist/all/SimpleOpenNI/library/osx

echo "--- change libpath for boost_thread ---"
install_name_tool -change /usr/local/lib/libboost_system-mt.dylib @loader_path/./libboost_system-mt.dylib libboost_thread-mt.dylib

echo "--- change libpath for libNiTE2 ---"
install_name_tool -change libOpenNI2.dylib @loader_path/./libOpenNI2.dylib libNiTE2.dylib

cd ./OpenNI2/Drivers
echo "--- change libpath for libFreenectDriver ---"
install_name_tool -change /usr/local/lib/libfreenect.0.1.2.dylib @loader_path/./libfreenect.0.1.2.dylib libFreenectDriver.dylib

echo "--- change libpath for libfreenect ---"
install_name_tool -change /usr/local/lib/libusb-1.0.0.dylib @loader_path/./libusb-1.0.0.dylib libfreenect.0.1.2.dylib