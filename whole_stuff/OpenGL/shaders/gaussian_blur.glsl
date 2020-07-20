#shader vertex
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

void main()
{
    TexCoords = aTexCoords;
    gl_Position = vec4(aPos, 1.0);
}

#shader fragment
#version 330 core
out vec4 FragColor;
  
in vec2 TexCoords;

uniform sampler2D image;
uniform bool horizontal;
//uniform float weight[11] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);
float weight[11] = float[] (0.0093,	0.028002, 0.065984, 0.121703, 0.175713, 0.198596, 0.175713, 0.121703, 0.065984, 0.028002, 0.0093);

void main()
{
    vec2 tex_offset = 1.0f / textureSize(image, 0); 
    
    vec3 result = texture(image, TexCoords).rgb * weight[0]; 
    if(horizontal)
    {
        for(int i = -5; i < 5; ++i)
        {
            result += texture(image, TexCoords + vec2(tex_offset.x * i, 0.0)).rgb * weight[i + 5];
        }
    }
    else
    {
        for(int i = -5; i < 5; ++i)
        {
            result += texture(image, TexCoords + vec2(0.0, tex_offset.y * i)).rgb * weight[i + 5];
        }
    }
    FragColor = vec4(result, 1.0);
}