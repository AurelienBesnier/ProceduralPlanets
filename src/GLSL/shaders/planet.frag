#version 420


// --------------------------------------------------
// shader definition
// --------------------------------------------------

uniform sampler2D Mask;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform vec3 objectColor;

in vec3 position;
in vec3 normal;
in vec2 textCoord;

out vec4 fragColor;


// --------------------------------------------------
// Fragment Shader:
// --------------------------------------------------

void main() {
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

    //fragColor = texture(Mask,textCoord);

    vec3 result = (ambient + diffuse + specular) * objectColor;
    fragColor = vec4(result,0);
    //fragColor = vec4(objectColor,0);
}
