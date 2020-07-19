#shader vertex
#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;
out vec4 FragPosLightSpace;
out mat3 TBN;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;

void main()
{
    mat3 normal_mat = mat3(transpose(inverse(model)));
    FragPos = vec3(model * vec4(aPos, 1.0));
    FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);

    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoords = aTexCoords;

    vec3 T = normalize(normal_mat * aTangent);
    vec3 B = normalize(normal_mat * aBitangent);
    vec3 N = normalize(normal_mat * aNormal);
    TBN = mat3(T, B, N);
    //TBN = transpose(mat3(T, B, N));

    gl_Position = projection * view * vec4(FragPos, 1.0);
}


// fragment shader
#shader fragment
#version 330 core
out vec4 FragColor;

struct Material {
    sampler2D diffuse1;
    sampler2D specular1;
    sampler2D normal1;
    int shininess;
};


const int DirLight = 0;
const int PointLight = 1;
const int SpotLight = 2;

struct Light
{
    int type;

    // dir
    vec3 direction;
    vec3 position;

    // point
    float constant;
    float linear;
    float quadratic;

    // spot
    float cutOff;
    float outerCutOff;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};


#define MAX_LIGHTS 5

in vec3 FragPos;
in vec4 FragPosLightSpace;

in vec3 Normal;
in vec2 TexCoords;
in mat3 TBN;

uniform vec3 viewPos;
uniform int normalmapping_flag;

// Light
uniform Light lights[MAX_LIGHTS];
uniform int nLights;

uniform Material material;
uniform sampler2D shadowMap;

uniform vec2 viewportSize;

// function prototypes
vec3 CalcDirLight(Light light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
    vec3 norm;
    if (normalmapping_flag == 0)
    {
        norm = normalize(Normal);
    }
    else
    {
        norm = texture(material.normal1, TexCoords).rgb;
        norm = normalize(norm * 2.0f - 1.0f);   
        norm = normalize(TBN * norm);
    }

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 result;
    for (int i = 0; i < nLights; i++)
    {
        switch (lights[i].type)
        {
            case DirLight:
                result += CalcDirLight(lights[i], norm, viewDir);
                break;

            case PointLight:
                result += CalcPointLight(lights[i], norm, FragPos, viewDir);
                break;

            case SpotLight:
                result += CalcSpotLight(lights[i], norm, FragPos, viewDir);
                break;
        }
    }
    
    vec2 screen_coords = gl_FragCoord.xy / viewportSize;
    float shadow_coef = texture(shadowMap, screen_coords).r;
    result *= shadow_coef;

    FragColor = vec4(result, 1.0);
}


// calculates the color when using a directional light.
vec3 CalcDirLight(Light light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);

    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // specular shading
    //vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    //vec3 specular = lightColor * spec;
    //float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    // combine results
    //vec3 color = texture(material.diffuse1, TexCoords).rgb;
    vec3 ambient = light.ambient * vec3(texture(material.diffuse1, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse1, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular1, TexCoords));

    // calculate shadow
    //float shadow = ShadowCalculation(FragPosLightSpace, normal, lightDir);
    //vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;
    //vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular));

    //return lighting;
    return (ambient + diffuse + specular);

    //vec3 color = texture(diffuseTexture, fs_in.TexCoords).rgb;
    //vec3 normal = normalize(fs_in.Normal);
    //vec3 lightColor = vec3(1.0);
    //// ambient
    //vec3 ambient = 0.15 * color;
    //// diffuse
    //vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    //float diff = max(dot(lightDir, normal), 0.0);
    //vec3 diffuse = diff * lightColor;
    //// specular
    //vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    //float spec = 0.0;
    //vec3 halfwayDir = normalize(lightDir + viewDir);
    //spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    //vec3 specular = spec * lightColor;
    //// calculate shadow
    //float shadow = ShadowCalculation(fs_in.FragPosLightSpace);
    //vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;
    //
    //FragColor = vec4(lighting, 1.0);
}

// calculates the color when using a point light.
vec3 CalcPointLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);

    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // specular shading
    //vec3 reflectDir = reflect(-lightDir, normal);
    //float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);

    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    // combine results
    vec3 ambient = light.ambient * vec3(texture(material.diffuse1, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse1, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular1, TexCoords));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

// calculates the color when using a spot light.
vec3 CalcSpotLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);

    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // specular shading
    //vec3 reflectDir = reflect(-lightDir, normal);
    //float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 halfwayDir = normalize(-light.direction + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);

    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    // combine results
    vec3 ambient = light.ambient * vec3(texture(material.diffuse1, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse1, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular1, TexCoords));
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
}