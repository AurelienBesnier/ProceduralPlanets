#version 460
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform bool lighting;

in vec3 position;
in vec3 normal;
in vec3 color;

out vec4 fragColor;

void main(void) {
    fragColor = vec4(color,0);
}
