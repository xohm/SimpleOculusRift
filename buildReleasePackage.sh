#!/bin/sh
# --------------------------------------------------------------------------
# buildscript for linux 64bit
# --------------------------------------------------------------------------
# Processing Wrapper for the OpenNI/Kinect 2.0 library
# http://code.google.com/p/simple-openni
# --------------------------------------------------------------------------
# prog:  Max Rheiner / Interaction Design / zhdk / http://iad.zhdk.ch/
# date:  08/25/2013 (m/d/y)
# ----------------------------------------------------------------------------
# Build a release package
# ----------------------------------------------------------------------------

# copy the zip folder to remove all svn files
echo "--- copy SimpleOpenNI to zip ---"
mkdir ./dist/all/tmp
cp  -r ./dist/all/SimpleOpenNI ./dist/all/tmp
cd ./dist/all/tmp

# remove all svn files
find . -name .svn -exec rm -rf {} \;

# remove all svn files
echo "--- compress SimpleOpenNI ---"
zip -r SimpleOpenNI ./SimpleOpenNI
cp SimpleOpenNI.zip ../

rm -rf ../tmp
echo "--- package done ---"
