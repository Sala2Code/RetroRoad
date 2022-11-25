#version 330 core

out vec4 FragColor;
in vec2 UVCoords;

uniform sampler2D screenTexture;
uniform sampler2D bloomTexture;

void main()
{
    vec3 fragment = texture(screenTexture, UVCoords).rgb;
    vec3 bloom = texture(bloomTexture, UVCoords).rgb;

    vec3 color = fragment + bloom;

    float exposure = 1.2f;
    vec3 toneMapped = vec3(1.0f) - exp(-color * exposure);

    FragColor.rgb = pow(toneMapped, vec3(1.0f / 2.2f)); // 2.2 is gamma
}