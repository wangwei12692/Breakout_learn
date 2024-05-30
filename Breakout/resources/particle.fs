#version 330 core
in vec2 TexCoords;
in vec4 ParticleColor;
out vec4 color;

uniform sampler2D sprite;

void main()
{
    // ParticleColor 是片段着色器自动插值的
    color = (texture(sprite, TexCoords) * ParticleColor);
}