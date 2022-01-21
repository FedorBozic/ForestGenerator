#version 330 core
out vec4 FragColor;

in vec3 Position;
in vec3 Normal;
in vec2 TexCoords;

uniform sampler2D texture_diffuse;
uniform sampler2D texture_specular;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;

void main()
{
   float ambientStrength = 0.2;
   vec3 ambient = ambientStrength * lightColor * texture(texture_diffuse, TexCoords).rgb;


   vec3 norm = normalize(Normal);
   vec3 lightDir = normalize(lightPos - Position);
   float diff = max(dot(norm, lightDir), 0.0);
   vec3 diffuse = diff * lightColor * texture(texture_diffuse, TexCoords).rgb;

   // specular
   vec3 viewDir = normalize(viewPos - Position);
   vec3 reflectDir = reflect(-lightDir, norm);  
   float spec = pow(max(dot(viewDir, reflectDir), 0.0), 0.5);
   //vec3 specular = 1.0 * spec * texture(texture_specular, TexCoords).rgb;
   vec3 specular = vec3(0.0, 0.0, 0.0);

   vec3 result = ambient + diffuse + specular;
   FragColor = vec4(result, texture(texture_diffuse, TexCoords).a);
   FragColor = texture(texture_diffuse, TexCoords);
};