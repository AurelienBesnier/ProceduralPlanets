#version 460
layout (location=0) in vec3 i_position;
layout (location=1) in vec3 i_normals;
layout (location=2) in vec4 i_color;

uniform mat4 mv_matrix;
uniform mat4 proj_matrix;

out vec3 position;
out vec3 normal;
out vec3 color;


void main(void)
{
    position = i_position.xyz;
    normal = vec3(mat3(proj_matrix * mv_matrix) * i_normals.xyz);
    color = i_color.xyz;
    gl_Position = proj_matrix * mv_matrix * vec4(position, 1.0);
}
