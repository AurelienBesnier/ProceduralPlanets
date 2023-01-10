#version 460
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform bool lighting;
uniform float selected_plate;

in vec3 position;
in vec3 normal;
in vec2 texCoord;
smooth in float elevation;
smooth in float plate_id;

out vec4 fragColor;

vec3 elevationToColor ()
{
    if(elevation < 0.2 && elevation > 0.0)
        return vec3(0.6,0.5,0.1);
    if(elevation >= 0.6 && elevation < 0.8)
        return vec3(0.1,0.4,0.1);
    if(elevation >= 0.2 && elevation < 0.6)
        return vec3(0.1,0.7,0.1);
    if(elevation >= 0.8)
        return vec3(1.0,1.0,1.0);

    if(elevation < -0.0 && elevation > -0.2)
        return vec3(0.0,0.3,0.6);
    if(elevation <= -0.2 && elevation > -0.8)
        return vec3(0.0,0.0,0.6);
    else
        return vec3(0.0,0.0,0.2);
}

void main(void) {
    vec3 color = elevationToColor();
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
        fragColor = vec4(result,1);
    }
    else{
        if(selected_plate == plate_id)
        {
            vec4 front = vec4(1,0,1,0.4);
            vec4 back = vec4(color, 0.9);
            vec3 col = 0.4 * front.rgb + (1-0.4)*back.a*back.rgb;

            fragColor = vec4(col,1);
        }
        else
        {
            fragColor = vec4(color,1);
        }
    }
}
