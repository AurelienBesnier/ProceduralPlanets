#version 460
layout (location=0) in vec3 i_position;
layout (location=1) in vec3 i_normals;
layout (location=2) in vec2 i_texCoord;
layout (location=3) in float i_elevation;
layout (location=4) in float i_plate_id;

uniform mat4 mv_matrix;
uniform mat4 proj_matrix;

out vec3 position;
out vec3 normal;
out vec2 texCoord;
out float elevation;
out float plate_id;

void main(void)
{
    position = i_position.xyz;
    normal = vec3(mat3(proj_matrix * mv_matrix) * i_normals.xyz);
    texCoord=i_texCoord;
    elevation = i_elevation;
    plate_id = i_plate_id;
    gl_Position = proj_matrix * mv_matrix * vec4(position, 1.0);
}
