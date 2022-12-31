#version 460


// --------------------------------------------------
// shader definition
// --------------------------------------------------

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform bool lighting;

in vec3 position;
in vec3 normal;
in vec3 color;

out vec4 fragColor;


// --------------------------------------------------
// Fragment Shader:
// --------------------------------------------------

void main() {
    if(lighting){
        float ambientStrength = 0.1;
        vec3 ambient = ambientStrength * lightColor;

        vec3 norm = normalize(normal);
        vec3 lightDir = normalize(lightPos - position);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = diff * lightColor;

        float specularStrength = 0.1;
        vec3 viewDir = normalize(viewPos - position);
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
        vec3 specular = specularStrength * spec * lightColor;

        vec3 result = (ambient + diffuse + specular) * color;
        fragColor = vec4(result,0);
    }
    else
        fragColor = vec4(color,0);
}
