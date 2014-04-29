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

#include "oculusRiftShaders.h"


const char* oculusRiftVertexShader =
"#version 110\n"
"\n"
"attribute vec3 Position;\n"
"attribute vec2 TexCoord;\n"
"varying vec2 oTexCoord;\n"
"\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(Position, 1);\n"
"   oTexCoord = TexCoord;\n"
"}\n";

/*
"#version 330 core\n"
"\n"
"layout(location = 0) in vec3 Position;\n"
"layout(location = 1) in vec2 TexCoord;\n"
"out vec2 oTexCoord;\n"
"\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(Position, 1);\n"
"   oTexCoord = TexCoord;\n"
"};\n";
*/

const char* oculusRiftFragmentShader =
"#version 110"
"\n"
"uniform vec2 LensCenter;\n"
"uniform vec2 ScreenCenter;\n"
"uniform vec2 Scale;\n"
"uniform vec2 ScaleIn;\n"
"uniform vec4 HmdWarpParam;\n"
"uniform sampler2D texture0;\n"
"in vec2 oTexCoord;\n"
"out vec4 outcolor;\n"
"\n"
"vec2 HmdWarp(vec2 in01)\n"
"{\n"
"   vec2  theta = (in01 - LensCenter) * ScaleIn; // Scales to [-1, 1]\n"
"   float rSq = theta.x * theta.x + theta.y * theta.y;\n"
"   vec2  theta1 = theta * (HmdWarpParam.x + HmdWarpParam.y * rSq + \n"
"                           HmdWarpParam.z * rSq * rSq + HmdWarpParam.w * rSq * rSq * rSq);\n"
"   return LensCenter + Scale * theta1;\n"
"}\n"
"void main()\n"
"{\n"
"   vec2 tc = HmdWarp(oTexCoord);\n"
"   if (!all(equal(clamp(tc, ScreenCenter-vec2(0.25,0.5), ScreenCenter+vec2(0.25,0.5)), tc)))\n"
"       outcolor = vec4(0);\n"
"   else\n"
"          outcolor = texture2D(texture0, tc);\n"
"}\n";


const char* oculusRiftChromaticFragmentShader =
"#version 110\n"
"\n"
"uniform vec2 LensCenter;\n"
"uniform vec2 ScreenCenter;\n"
"uniform vec2 Scale;\n"
"uniform vec2 ScaleIn;\n"
"uniform vec4 HmdWarpParam;\n"
"uniform vec4 ChromAbParam;\n"
"uniform sampler2D texture0;\n"
"varying vec2 oTexCoord;\n"
"\n"
"void main(void)\n"
"{\n"
"	vec2 theta = (oTexCoord - LensCenter) * ScaleIn; // Scales texture coordinates to [-1, 1]\n"
"	float rSq = theta.x * theta.x + theta.y * theta.y;\n"
"	vec2 theta1 = theta * ( HmdWarpParam.x +\n"
"							HmdWarpParam.y * rSq +\n"
"							HmdWarpParam.z * rSq * rSq +\n"
"							HmdWarpParam.w * rSq * rSq * rSq);\n"
"	vec2 thetaBlue = theta1 * (ChromAbParam.z + ChromAbParam.w * rSq);\n"
"	vec2 tcBlue = LensCenter + Scale * thetaBlue;\n"
"\n"
"	if (any(bvec2(clamp(tcBlue, ScreenCenter - vec2(0.25, 0.5), ScreenCenter + vec2(0.25, 0.5)) - tcBlue))) {\n"
"		gl_FragColor = vec4(0, 0, 0, 0);\n"
"		return;\n"
"	}\n"
"\n"
"	// Now do blue texture lookup.\n"
"	float blue = texture2D(texture0, tcBlue).b;\n"
"	// Do green lookup (no scaling).\n"
"	vec2 tcGreen = LensCenter + Scale * theta1;\n"
"	float green = texture2D(texture0, tcGreen).g;\n"
"	// Do red Scale and lookup.\n"
"	vec2 thetaRed = theta1 * (ChromAbParam.x + ChromAbParam.y * rSq);\n"
"	vec2 tcRed = LensCenter + Scale * thetaRed;\n"
"	float red = texture2D(texture0, tcRed).r;\n"
"	gl_FragColor = vec4(red, green, blue, 1);\n"
"}\n";

/*

const char* oculusRiftVertexShader =
"#version 110\n"
"\n"
"varying vec2 theta;\n"
"\n"
"uniform vec2 LensCenter;\n"
"uniform vec2 ScaleIn;\n"
"\n"
"void main(void) {\n"
"    // pre-transform the texture coordinates\n"
"    theta = (gl_MultiTexCoord0.xy - LensCenter) * ScaleIn;\n"
"    gl_Position = gl_Vertex;\n"
"}\n";

const char* oculusRiftChromaticFragmentShader =
"#version 110\n"
"\n"
"varying vec2 theta;\n"
"\n"
"uniform vec2 LensCenter;\n"
"uniform vec2 ScreenCenter;\n"
"uniform vec2 Scale;\n"
"uniform vec4 HmdWarpParam;\n"
"uniform vec4 ChromAbParam;\n"
"\n"
"uniform sampler2D tex;\n"
"\n"
"// Scales input texture coordinates for distortion.\n"
"// ScaleIn maps texture coordinates to Scales to ([-1, 1]), although top/bottom will be\n"
"// larger due to aspect ratio.\n"
"void main()\n"
"{\n"
"    float rSq = theta.x*theta.x + theta.y*theta.y;\n"
"    vec2 theta1 = theta*(HmdWarpParam.x + HmdWarpParam.y*rSq + HmdWarpParam.z*rSq*rSq + HmdWarpParam.w*rSq*rSq*rSq);\n"
"\n"
"    // Detect whether blue texture coordinates are out of range since these will Scaled out the furthest.\n"
"    vec2 thetaBlue = theta1 * (ChromAbParam.z + ChromAbParam.w * rSq);\n"
"    vec2 tcBlue = LensCenter + Scale * thetaBlue;\n"
"\n"
"	if(any(bvec2(clamp(tcBlue, ScreenCenter - vec2(0.25, 0.5), ScreenCenter + vec2(0.25, 0.5)) - tcBlue)))\n"
"    {\n"
"        gl_FragColor = vec4(1.0,0.0,1.0,1.0);\n"
"        return;\n"
"    }\n"
"\n"
"    // Now do blue texture lookup.\n"
"    float blue = texture2D(tex, tcBlue).b;\n"
"\n"
"    // Do green lookup (no scaling).\n"
"    vec2  tcGreen = LensCenter + Scale * theta1;\n"
"\n"
"    float green = texture2D(tex, tcGreen).g;\n"
"\n"
"    // Do red Scale and lookup.\n"
"    vec2  thetaRed = theta1 * (ChromAbParam.x + ChromAbParam.y * rSq);\n"
"    vec2  tcRed = LensCenter + Scale * thetaRed;\n"
"\n"
"    float red = texture2D(tex, tcRed).r;\n"
"\n"
"    gl_FragColor = vec4(red, green, blue, 1.0);\n"
"}\n";

*/
