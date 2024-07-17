#version 430 core

uniform mat4 model_matrix;
uniform mat4 projection_matrix;

layout (location = 0) in vec4 in_position;
layout (location = 1) in vec2 in_tex_coord;
out vec2 tex_coord;
void main(void)
{
    gl_Position = projection_matrix * (model_matrix * in_position);
    tex_coord = in_tex_coord;
}