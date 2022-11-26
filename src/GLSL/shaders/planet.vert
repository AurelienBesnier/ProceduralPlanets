// --------------------------------------------------
// shader definition
// --------------------------------------------------
#version 420

layout (location=0) in vec3 i_position;
layout (location=1) in vec3 i_normals;
layout (location=2) in vec2 i_texCoords;

// --------------------------------------------------
// Uniform variables:
// --------------------------------------------------
uniform mat4 mv_matrix;
uniform mat4 proj_matrix;

// --------------------------------------------------
// varying variables
// --------------------------------------------------
out vec3 position;
out vec2 textCoord;
// --------------------------------------------------
// Vertex-Shader
// --------------------------------------------------


void main()
{
    position = i_position.xyz;
    textCoord = i_texCoords;
    gl_Position = proj_matrix * mv_matrix * vec4(position, 1.0);
}
