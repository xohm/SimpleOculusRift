#!/bin/sh
# --------------------------------------------------------------------------
# buildscript for osx 32bit/64bit
# --------------------------------------------------------------------------
# Processing Wrapper for the OpenNI/Kinect library
# http://code.google.com/p/simple-openni
# --------------------------------------------------------------------------
# prog:  Max Rheiner / Interaction Design / zhdk / http://iad.zhdk.ch/
# date:  06/11/2011 (m/d/y)
# ----------------------------------------------------------------------------
# Change those vars to the folders you have on your system:
#	-DOPEN_NI_INCLUDE 	= folder of OpenNI headers
#	-DXN_NITE_INCLUDE 	= folder of Nite headers
#	-DXN_NITE_LIB 		= folder of Nite libraries
#	-DEIGEN3D_INCLUDE 	= folder of Eigen3d headers
#	-DBOOST_ROOT 		= folder of Boost root
#	-DBOOST_LIBRARYDIR 	= folder of Boost library folder
#	-DP5_JAR 			= filepath to your core.jar (Processing)
# ----------------------------------------------------------------------------

# optional, but gives a clean build
rm -r build

# check if build folder exists
if [ ! -d "build" ]; then
    mkdir build
fi

cd ./build
 
echo "--- generate cmake ---"
cmake -DCMAKE_BUILD_TYPE=Release \
      -DOPEN_NI_BASE=/Users/max/Documents/pDev/openni/OpenNI-MacOSX-x64-2.2/ \
	  -DOPEN_NI_LIBDIR=./dist/all/SimpleOpenNI/library/osx/ \
	  -DNITE_BASE=/Users/max/Documents/pDev/openni/NiTE-MacOSX-x64-2.2/ \
	  -DEIGEN3D_INCLUDE=/usr/local/include/eigen3/ \
	  -DP5_JAR=/Applications/Processing.app/Contents/Resources/Java/core/library/core.jar \
	  -DCMAKE_OSX_ARCHITECTURES="i386;x86_64" \
	  ..


echo "--- build ---"
# build with 6 threads, verbose is optional, but otherwise you can't see the compiler directives
#make -j 6 VERBOSE=1
make -j6 

# change name path for a local library
# boost
install_name_tool -change /usr/local/lib/libboost_thread-mt.dylib @loader_path/./osx/libboost_thread-mt.dylib libSimpleOpenNI.jnilib
install_name_tool -change /usr/local/lib/libboost_system-mt.dylib @loader_path/./osx/libboost_system-mt.dylib libSimpleOpenNI.jnilib
# nite
install_name_tool -change libOpenNI2.dylib @loader_path/./osx/libOpenNI2.dylib libSimpleOpenNI.jnilib
install_name_tool -change libNiTE2.dylib @loader_path/./osx/libNiTE2.dylib libSimpleOpenNI.jnilib


echo "Lib paths:"
otool -L libSimpleOpenNI.jnilib



echo "--- copy ---"
# copy the library
cp SimpleOpenNI.jar ../dist/all/SimpleOpenNI/library
cp libSimpleOpenNI.jnilib ../dist/all/SimpleOpenNI/library

# copy the doc
cp -r ./doc/* ../dist/all/SimpleOpenNI/documentation/