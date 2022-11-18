#version 430


// --------------------------------------------------
// shader definition
// --------------------------------------------------


uniform sampler2D Mask; // declaration de la map mask

in vec3 position;
in vec3 textCoord;

out vec4 fragColor;


// --------------------------------------------------
// Fragment Shader:
// --------------------------------------------------
void main() {

    fragColor = texture(Mask,textCoord);
}
