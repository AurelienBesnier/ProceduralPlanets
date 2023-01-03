#version 460
layout (location=0) in vec3 i_position;
layout (location=1) in vec3 i_normals;
layout (location=2) in vec4 i_color;
layout (location=3) in vec2 i_texCoord;
layout (location=4) in double i_elevation;

uniform mat4 mv_matrix;
uniform mat4 proj_matrix;

out vec3 position;
out vec3 normal;
out vec3 color;
out vec2 texCoord;
out double elevation;

void main(void)
{
    position = i_position.xyz;
    normal = vec3(mat3(proj_matrix * mv_matrix) * i_normals.xyz);
    color = i_color.xyz;
    texCoord=i_texCoord;
    elevation = i_elevation;
    gl_Position = proj_matrix * mv_matrix * vec4(position, 1.0);
}
