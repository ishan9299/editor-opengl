#version 330 core

out vec4 FragColor;

in vec2 TexCoord;
in vec4 Color;

uniform sampler2D texture0;

void main()
{
   float color = texture(texture0, TexCoord).r;
   FragColor = vec4(1.0, 1.0, 1.0, color)*Color;
}
