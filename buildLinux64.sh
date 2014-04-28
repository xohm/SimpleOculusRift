#!/bin/sh
# --------------------------------------------------------------------------
# buildscript for linux 64bit
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
rm -r build64

# check if build folder exists
if [ ! -d "build64" ]; then
    mkdir build64
fi

cd ./build64

echo "--- generate cmake ---"
# changes this according to your environment
cmake -DCMAKE_BUILD_TYPE=Release \
	  -DEIGEN3D_INCLUDE=/usr/local/include/eigen3/ \
	  -DBOOST_ROOT=/usr/include/boost/ \
	  -DBOOST_LIBRARYDIR=/usr/lib/ \
	  -DOCULUSRIFT_DIR=/media/dataDisk/ownDev/libs/oculusRift/OculusSDK/ \
          -DP5_JAR=/media/dataDisk/ownDev/locApp/processing-2.1.2/core/library/core.jar \
          ..


echo "--- build ---"
# build with 6 threads, verbose is optional, but otherwise you can't see the compiler directives
#make -j 6 VERBOSE=1
make -j6

echo "--- copy ---"
# copy the library
cp SimpleOculusRift.jar ../dist/all/SimpleOculusRift/library
cp libSimpleOculusRift*.so ../dist/all/SimpleOculusRift/library

# copy the doc
cp -r ./doc/* ../dist/all/SimpleOculusRift/documentation/

