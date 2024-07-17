#version 430 core
in vec2 tex_coord;
layout (location = 0) out vec4 color;
uniform sampler2D tex;
void main(void)
{
    color = texture(tex, tex_coord);
    //color = vec4(1.0,0,0,1.0);
}