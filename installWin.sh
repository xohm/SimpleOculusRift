#!/bin/sh
# --------------------------------------------------------------------------
# install script for Windows, uses msys
# --------------------------------------------------------------------------
# Processing Wrapper for the Oculus Rift library
# http://github.com/xohm/SimpleOculusRift
# --------------------------------------------------------------------------
# prog:  Max Rheiner / Interaction Design / zhdk / http://iad.zhdk.ch/
# date:  06/24/2014 (m/d/y)

# ----------------------------------------------------------------------------
# Change P5_Path to the folder where Processing stores the libraries
# On Windows it should be in '~/Documents/Processing' (Processing 1.5.1)
# ----------------------------------------------------------------------------

# copy the libs/doc/examples to the processing folders
P5_Path=~/Documents/Processing

# check if libraries folder exists
if [ ! -d $P5_Path/libraries ]; then
    mkdir $P5_Path/libraries
fi

# copie the files
cp -r ./dist/all/SimpleOculusRift  $P5_Path/libraries/

# remove all subversion folders
cd $P5_Path/libraries/SimpleOculusRift
rm -rf `find . -type d -name .git`

echo "--- installed SimpleOculusRift ---"