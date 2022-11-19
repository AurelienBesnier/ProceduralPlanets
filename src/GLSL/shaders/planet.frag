#version 420


// --------------------------------------------------
// shader definition
// --------------------------------------------------


uniform sampler2D Mask; // declaration de la map mask

in vec3 position;
in vec2 textCoord;

out vec4 fragColor;


// --------------------------------------------------
// Fragment Shader:
// --------------------------------------------------
void main() {

    //fragColor = texture(Mask,textCoord);
    fragColor = vec4(1,0,0,0);
}
