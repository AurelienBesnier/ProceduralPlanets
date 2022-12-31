#version 460
layout (location=0) in vec3 i_position;

uniform mat4 view;
uniform mat4 proj_matrix;

out vec3 TexCoords;

void main(void)
{
    TexCoords = i_position.xyz;
    gl_Position = proj_matrix * view * vec4(i_position.xyz, 1.0);
}
