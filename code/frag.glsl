#version 330 core

out vec4 frag_color;
in vec2 tex_coord;

// texture sampler
uniform sampler2D our_texture;

void main()
{
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(our_texture, tex_coord).r);
    frag_color = sampled;
}
