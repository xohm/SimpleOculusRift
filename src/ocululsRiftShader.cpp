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
"#version 120\n"
"\n"
"void main()\n"
"{\n"
"        gl_TexCoord[0] = gl_MultiTexCoord0;\n"
"        gl_Position = gl_Vertex;\n"
"}\n";

const char* oculusRiftFragmentShader =
"#version 120\n"
"\n"
"uniform sampler2D Texture;\n"
"uniform vec2 LensCenter;\n"
"uniform vec2 ScreenCenter;\n"
"uniform vec2 Scale;\n"
"uniform vec2 ScaleIn;\n"
"uniform vec4 HmdWarpParam;\n"
"\n"
"// Scales input texture coordinates for distortion.\n"
"vec2 HmdWarp(vec2 in01)\n"
"{\n"
"    vec2 theta = (in01 - LensCenter) * ScaleIn; // Scales to [-1, 1]\n"
"    float rSq = theta.x * theta.x + theta.y * theta.y;\n"
"    vec2 rvector = theta * (HmdWarpParam.x + HmdWarpParam.y * rSq +\n"
"                            HmdWarpParam.z * rSq * rSq +\n"
"                            HmdWarpParam.w * rSq * rSq * rSq);\n"
"    return LensCenter + Scale * rvector;\n"
"}\n"
"\n"
"void main()\n"
"{\n"
"    vec2 tc = HmdWarp(gl_TexCoord[0].st);\n"
"    if (any(bvec2(clamp(tc,ScreenCenter-vec2(0.25,0.5), ScreenCenter+vec2(0.25,0.5)) - tc)))\n"
"    {\n"
"        gl_FragColor = vec4(vec3(0.0), 1.0);\n"
"        return;\n"
"    }\n"
"\n"
"    gl_FragColor = texture2D(Texture, tc);\n"
"}\n";
