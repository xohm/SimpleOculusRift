 
#version 110
attribute vec3 Position;
attribute vec2 TexCoord;
varying vec2 oTexCoord;

void main()
{
   gl_Position = vec4(Position, 1);
   oTexCoord = TexCoord;
}

