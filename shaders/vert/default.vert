#version 330 core

layout(location = 0) in vec3 aPos;      // posição do vértice
layout(location = 1) in vec2 aTexCoord; // coordenada de textura

uniform mat4 uMVP; // matriz Model-View-Projection

out vec2 TexCoord;

void main()
{
    gl_Position = uMVP * vec4(aPos, 1.0);
    TexCoord = aTexCoord;
}
