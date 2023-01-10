#version 420
layout (location=0) in vec3 i_position;

uniform mat4 view;
uniform mat4 proj_matrix;

out vec3 TexCoords;

void main(void)
{
    TexCoords = i_position.xyz;
    vec4 pos = proj_matrix * view * vec4(i_position, 1.0);
    gl_Position = pos.xyww;
}
