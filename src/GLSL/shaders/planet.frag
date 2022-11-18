#version 430


// --------------------------------------------------
// shader definition
// --------------------------------------------------


uniform sampler3D Mask; // declaration de la map mask

uniform float xCutPosition;
uniform float yCutPosition;
uniform float zCutPosition;

uniform int xCutDirection;
uniform int yCutDirection;
uniform int zCutDirection;

in vec3 position;
in vec3 textCoord;

out vec4 fragColor;


// --------------------------------------------------
// Fragment Shader:
// --------------------------------------------------
void main() {

//TODO fetch color in texture
        fragColor = texture(Mask,textCoord);
}
