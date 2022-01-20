#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform vec3 lightColor;
uniform sampler2D texture_diffuse;

void main()
{
    vec3 result = lightColor;
    FragColor = vec4(result, 1.0);
}