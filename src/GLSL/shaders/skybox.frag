#version 420
in vec3 TexCoords;
uniform samplerCube skybox;

out vec4 fragColor;

void main(void) {
    fragColor = texture(skybox, TexCoords);
}
