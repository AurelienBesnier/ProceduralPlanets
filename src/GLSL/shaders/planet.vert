// --------------------------------------------------
// shader definition
// --------------------------------------------------
#version 420

layout (location=0) in vec3 i_position;
layout (location=1) in vec3 i_normals;
layout (location=2) in vec2 i_texCoords;
layout (location=3) in vec3 i_color;

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
out vec2 textCoord;
out vec3 color;
// --------------------------------------------------
// Vertex-Shader
// --------------------------------------------------


void main()
{
    position = i_position.xyz;
    normal = mat3(mv_matrix) * i_normals;
    textCoord = i_texCoords;
    color = i_color;
    gl_Position = proj_matrix * mv_matrix * vec4(position, 1.0);
}
