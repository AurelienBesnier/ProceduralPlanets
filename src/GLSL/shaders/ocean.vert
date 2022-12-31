#version 460
// --------------------------------------------------
// shader definition
// --------------------------------------------------

layout (location=0) in vec3 i_position;
layout (location=1) in vec3 i_normals;

// --------------------------------------------------
// Uniform variables:
// --------------------------------------------------
uniform mat4 mv_matrix;
uniform mat4 proj_matrix;

// --------------------------------------------------
// varying variables
// --------------------------------------------------
out vec3 position;
out vec3 normal;
out vec3 color;
// --------------------------------------------------
// Vertex-Shader
// --------------------------------------------------


void main()
{
    position = i_position.xyz;
    normal = mat3(mv_matrix) * i_normals;
    color = vec3(0,0,1);
    gl_Position = proj_matrix * mv_matrix * vec4(position, 1.0);
}
