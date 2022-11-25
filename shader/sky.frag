#version 330 core

out vec4 FragColor;
in vec3 UVCoords;

uniform samplerCube skybox;

void main(){    
    FragColor = texture(skybox, UVCoords);
}