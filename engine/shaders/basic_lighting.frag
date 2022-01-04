#version 460 core
out vec4 FragColor;

uniform sampler2D diffuse;
uniform sampler2D specular;
uniform float shininess;

in vec3 Normal;
in vec3 Position;
in vec2 TexCoords;

uniform vec3 lightPosition;
uniform vec3 lightColor;
uniform vec3 viewPosition;

void main()
{
    float lightAmbient = 0.2;
    vec3 ambientColor = lightAmbient * texture(diffuse, TexCoords).rgb;

    vec3 norm = normalize(Normal);
    vec3 lightDirection = normalize(lightPosition - Position);
    float diff = max(dot(norm, lightDirection), 0.0);
    float lightDiffuse = 0.5;
    vec3 diffuseColor = lightDiffuse * diff * texture(diffuse, TexCoords).rgb;

    vec3 viewDirection = normalize(viewPosition - Position);
    vec3 reflectDirection = reflect(-lightDirection, norm);
    float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), shininess);
    float lightSpecular = 1.0;
    vec3 specularColor = lightSpecular * spec * texture(specular, TexCoords).rgb;

    vec3 result = ambientColor + diffuseColor + specularColor;
    FragColor = vec4(result, 1.0);
}