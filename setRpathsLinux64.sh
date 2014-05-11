#!/bin/sh
# --------------------------------------------------------------------------
# rpathScript for linux 64bit
# --------------------------------------------------------------------------
# Processing Wrapper for the Oculus Rift library
# http://github.com/xohm/SimpleOculusRift
# --------------------------------------------------------------------------
# prog:  Max Rheiner / Interaction Design / zhdk / http://iad.zhdk.ch/
# date:  06/24/2014 (m/d/y)
# ----------------------------------------------------------------------------
# Set the rpaths for the libraries, the libs should have all the need shared
# objects in their folder	
# ----------------------------------------------------------------------------

cd ./dist/all/SimpleOculusRift/library

echo "--- change rpath for libFreenectDriver.so ---"
patchelf --set-rpath ./ SimpleOculusRift64.so

