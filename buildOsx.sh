#!/bin/sh
# --------------------------------------------------------------------------
# buildscript for osx 32bit/64bit
# --------------------------------------------------------------------------
# Processing Wrapper for the Oculus Rift library
# http://github.com/xohm/SimpleOculusRift
# --------------------------------------------------------------------------
# prog:  Max Rheiner / Interaction Design / zhdk / http://iad.zhdk.ch/
# date:  06/24/2014 (m/d/y)
# ----------------------------------------------------------------------------
# Change those vars to the folders you have on your system:
#	-DEIGEN3D_INCLUDE 	= folder of Eigen3d headers
#	-DBOOST_ROOT 		= folder of Boost root
#	-DBOOST_LIBRARYDIR 	= folder of Boost library folder
#	-DP5_JAR 		= filepath to your core.jar (Processing)
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
	  -DEIGEN3D_INCLUDE=/usr/local/include/eigen3/ \
          -DOCULUSRIFT_DIR=/Users/max/Documents/libs/OculusSDK \
          -DP5_JAR=/Applications/Processing.app/Contents/Java/core/library/core.jar \
          -DCMAKE_OSX_ARCHITECTURES="x86_64" \
	  ..

#          -DCMAKE_OSX_ARCHITECTURES="i386;x86_64" \


echo "--- build ---"
# build with 6 threads, verbose is optional, but otherwise you can't see the compiler directives
#make -j 6 VERBOSE=1
make -j6 

# change name path for a local library
# boost
#install_name_tool -change /usr/local/lib/libboost_system-mt.dylib @loader_path/./osx/libboost_system-mt.dylib libSimpleOculusRift.jnilib

echo "Lib paths:"
otool -L libSimpleOculusRift.jnilib



echo "--- copy ---"
# copy the library
cp SimpleOculusRift.jar ../dist/all/SimpleOculusRift/library
cp libSimpleOculusRift.jnilib ../dist/all/SimpleOculusRift/library

# copy the doc
cp -r ./doc/* ../dist/all/SimpleOculusRift/documentation/
