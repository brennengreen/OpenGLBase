#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 depthMVP;

void main()
{
    gl_Position = depthMVP * vec4(aPos, 1.0);
} 