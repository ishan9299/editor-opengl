#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec4 aColor;

out vec2 TexCoord;
out vec4 Color;

uniform mat4 projection;

void main()
{
    vec3 position = vec3(aPos.x, aPos.y, 0.0);
    gl_Position = projection*vec4(position, 1.0);
    TexCoord = aTexCoord;
    Color = aColor;
}
