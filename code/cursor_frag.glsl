#version 330 core

out vec4 frag_color;
uniform vec4 c_color;

void main()
{
    frag_color = c_color;
}