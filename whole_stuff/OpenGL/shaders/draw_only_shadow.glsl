#shader vertex
#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

out vec3 FragPos;
out vec3 Normal;
out vec4 FragPosLightSpace;

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
    gl_Position = projection * view * vec4(FragPos, 1.0);
}


// fragment shader
#shader fragment
#version 330 core
out vec4 FragColor;

// Light
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


in vec3 FragPos;
in vec4 FragPosLightSpace;
in vec3 Normal;

// Light
#define MAX_LIGHTS 5
uniform Light lights[MAX_LIGHTS];
uniform int nLights;

// Test
uniform sampler2D shadowMap;
uniform samplerCube depthMap;
uniform float far_plane;

uniform vec3 viewPos;

float CalcDirLight(Light light, vec3 normal, vec3 viewDir);
float CalcPointLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir);
float CalcSpotLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir);

float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir);
float ShadowCalculationCubemap(vec3 fragPos);


void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    float shadow = ShadowCalculation(FragPosLightSpace, norm, lights[0].direction);
    float cubemapShadoe = ShadowCalculationCubemap(FragPos);

    vec3 result;
    for (int i = 1; i < nLights; i++)
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

    result = max(vec3(1.0f - shadow) + vec3(1.0f - cubemapShadoe) + result, vec3(0));
    FragColor = vec4(result, 1.0);
}

float ShadowCalculationCubemap(vec3 fragPos)
{
    vec3 fragToLight = fragPos - lights[3].position;
    float closestDepth = texture(depthMap, fragToLight).r;
    closestDepth *= far_plane;
    float currentDepth = length(fragToLight);
    //float bias = 0.1f; 
    //float shadow = currentDepth -  bias > closestDepth ? 1.0 : 0.0;
    
    //float shadow  = 0.0;
    //float bias    = 0.05; 
    //float samples = 4.0;
    //float offset  = 0.1;
    //for(float x = -offset; x < offset; x += offset / (samples * 0.5))
    //{
    //    for(float y = -offset; y < offset; y += offset / (samples * 0.5))
    //    {
    //        for(float z = -offset; z < offset; z += offset / (samples * 0.5))
    //        {
    //            float closestDepth = texture(depthMap, fragToLight + vec3(x, y, z)).r; 
    //            closestDepth *= far_plane;   // обратное преобразование из диапазона [0;1]
    //            if(currentDepth - bias > closestDepth)
    //                shadow += 1.0;
    //        }
    //    }
    //}
    //shadow /= (samples * samples * samples);


    vec3 sampleOffsetDirections[20] = vec3[]
    (
       vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
       vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
       vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
       vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
       vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
    );  

    float shadow = 0.0;
    float bias   = 0.15;
    int samples  = 20;
    float viewDistance = length(viewPos - fragPos);
    float diskRadius = (1.0 + (viewDistance / far_plane)) / 25.0;  
    for(int i = 0; i < samples; ++i)
    {
        float closestDepth = texture(depthMap, fragToLight + sampleOffsetDirections[i] * diskRadius).r;
        closestDepth *= far_plane;   // обратное преобразование из диапазона [0;1]
        if(currentDepth - bias > closestDepth)
            shadow += 1.0;
    }
    shadow /= float(samples);  

    return shadow;
}

float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)
{
     // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5f + 0.5f;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
    //float bias = 0.001f;
    float bias = max(0.001f * (1.0f - dot(normal, lightDir)), 0.0005f);
    
    float shadow = 0.0f;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -2; x <= 2; ++x)
    {
        for(int y = -2; y <= 2; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0f : 0.0f;
        }
    }
    shadow /= 25.0f;
    
    if(projCoords.z > 1.0f)
        shadow = 0.0f;

    return shadow;
}


// calculates the color when using a directional light.
float CalcDirLight(Light light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);

    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // specular shading
    //vec3 reflectDir = reflect(-lightDir, normal);
    //vec3 halfwayDir = normalize(lightDir + viewDir);
    //float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    //vec3 specular = lightColor * spec;
    //float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    // combine results
    //vec3 ambient = light.ambient * vec3(texture(material.diffuse1, TexCoords));
    //vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse1, TexCoords));
    //vec3 specular = light.specular * spec * vec3(texture(material.specular1, TexCoords));

    //return lighting;
    return diff;
}

// calculates the color when using a point light.
float CalcPointLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);

    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // specular shading
    //vec3 halfwayDir = normalize(lightDir + viewDir);
    //float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);

    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    // combine results
    //vec3 ambient = light.ambient * vec3(texture(material.diffuse1, TexCoords));
    //vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse1, TexCoords));
    //vec3 specular = light.specular * spec * vec3(texture(material.specular1, TexCoords));
    //ambient *= attenuation;
    //diffuse *= attenuation;
    //specular *= attenuation;
    //return (ambient + diffuse + specular);

    return diff * attenuation;
}

// calculates the color when using a spot light.
float CalcSpotLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);

    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // specular shading
    //vec3 halfwayDir = normalize(-light.direction + viewDir);
    //float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);

    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    // combine results
    //vec3 ambient = light.ambient * vec3(texture(material.diffuse1, TexCoords));
    //vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse1, TexCoords));
    //vec3 specular = light.specular * spec * vec3(texture(material.specular1, TexCoords));
    //ambient *= attenuation * intensity;
    //diffuse *= attenuation * intensity;
    //specular *= attenuation * intensity;

    diff *= attenuation * intensity;
    
    return diff;
}