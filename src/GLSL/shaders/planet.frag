#version 420


// --------------------------------------------------
// shader definition
// --------------------------------------------------


uniform sampler2D Mask;

in vec3 position;
in vec2 textCoord;

out vec4 fragColor;


// --------------------------------------------------
// Fragment Shader:
// --------------------------------------------------
void main() {

    //fragColor = texture(Mask,textCoord);
    fragColor = vec4(0.5,0.1,0.5,0);
}
