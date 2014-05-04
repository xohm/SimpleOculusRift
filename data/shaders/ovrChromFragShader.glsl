#version 120

uniform vec2 LensCenter;
uniform vec2 ScreenCenter;
uniform vec2 Scale;
uniform vec2 ScaleIn;
uniform vec4 HmdWarpParam;
uniform vec4 ChromAbParam;
uniform sampler2D texture0;
varying vec2 oTexCoord;

void main(void)
{
        vec2 theta = (oTexCoord - LensCenter) * ScaleIn; // Scales texture coordinates to [-1, 1]
        float rSq = theta.x * theta.x + theta.y * theta.y;
        vec2 theta1 = theta * ( HmdWarpParam.x +
                                                        HmdWarpParam.y * rSq +
                                                        HmdWarpParam.z * rSq * rSq +
                                                        HmdWarpParam.w * rSq * rSq * rSq);
        vec2 thetaBlue = theta1 * (ChromAbParam.z + ChromAbParam.w * rSq);
        vec2 tcBlue = LensCenter + Scale * thetaBlue;

        if (any(bvec2(clamp(tcBlue, ScreenCenter - vec2(0.25, 0.5), ScreenCenter + vec2(0.25, 0.5)) - tcBlue))) {
                gl_FragColor = vec4(0, 0, 0, 0);
                return;
        }

        // Now do blue texture lookup.
        float blue = texture2D(texture0, tcBlue).b;
        // Do green lookup (no scaling).
        vec2 tcGreen = LensCenter + Scale * theta1;
        float green = texture2D(texture0, tcGreen).g;
        // Do red Scale and lookup.
        vec2 thetaRed = theta1 * (ChromAbParam.x + ChromAbParam.y * rSq);
        vec2 tcRed = LensCenter + Scale * thetaRed;
        float red = texture2D(texture0, tcRed).r;
        gl_FragColor = vec4(red, green, blue, 1);
};
