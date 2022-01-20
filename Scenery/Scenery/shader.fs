#version 330 core
out vec4 FragColor;
in vec3 Position;
in vec3 Normal;
in vec2 TexCoords;
uniform sampler2D texture_diffuse1;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;

void main()
{
   float ambientStrength = 0.2;
   vec3 ambient = ambientStrength * lightColor;
   vec3 norm = normalize(Normal);
   vec3 lightDir = normalize(lightPos - Position);
   float diff = max(dot(norm, lightDir), 0.0);
   vec3 diffuse = diff * lightColor;
   vec4 texCol = texture(texture_diffuse1, TexCoords);
   vec3 result = (ambient + diffuse) * texCol.rgb;
   FragColor = vec4(result, texCol.a);
};