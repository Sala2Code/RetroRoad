#version 330 core

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

in vec2 UVCoords;
uniform sampler2D diffuseTexture;

void main()
{           
    // vec3 color = texture(diffuseTexture, UVCoords).rgb;
    FragColor = vec4(1, 0.1f, 1, 1);
    
    float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 0.15f){
        BrightColor = vec4(FragColor.rgb/2.0f, 1.0);
    }else{
        BrightColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
    }   
}