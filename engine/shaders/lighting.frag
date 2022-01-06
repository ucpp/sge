#version 460 core
out vec4 FragColor;

struct Material
{
    sampler2D diffuse;
    sampler2D specular;
    sampler2D ambient;
    sampler2D normal;
    float shininess;
};

struct DirectionalLight
{
    vec3 direction;

    float ambient;
    float diffuse;
    float specular;
};

struct PointLight
{
    vec3 position;

    float linear;
    float quadratic;

    vec3 color;
    float ambient;
    float diffuse;
    float specular;
};

uniform Material material;
uniform DirectionalLight directionalLight;

#define COUNT_POINT_LIGHTS 4
uniform PointLight pointLights[COUNT_POINT_LIGHTS];
uniform vec3 viewPosition;

in VS_OUT 
{
    vec3 Position;
    vec2 TexCoords;
    vec3 Normal;
    vec3 Tangent;
} fs_in;

vec3 CalculateDirectionLight(vec3 normal, vec3 viewDirection);
vec3 CalculatePointLight(int index, vec3 normal, vec3 viewDirection);
vec3 CalcBumpedNormal();

void main()
{
    if(texture(material.diffuse, fs_in.TexCoords).a < 0.05)
    {
        discard;
    }

    vec3 norm = CalcBumpedNormal();
    vec3 viewDirection = normalize(viewPosition - fs_in.Position);

    vec3 resultColor = CalculateDirectionLight(norm, viewDirection);

    for(int i = 0; i < COUNT_POINT_LIGHTS; ++i)
    {
        resultColor += CalculatePointLight(i, norm, viewDirection);
    }

    FragColor = vec4(resultColor, 1.0);
}

vec3 CalculateDirectionLight(vec3 normal, vec3 viewDirection)
{
    vec3 lightDirection = normalize(-directionalLight.direction);
    float diff = max(dot(normal, lightDirection), 0.0);
    vec3 reflectDirection = reflect(-lightDirection, normal);
    float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), material.shininess);

    vec3 ambient = directionalLight.ambient * vec3(texture(material.diffuse, fs_in.TexCoords));
    vec3 diffuse = directionalLight.diffuse * diff * vec3(texture(material.diffuse, fs_in.TexCoords));
    vec3 specular = directionalLight.specular * spec * vec3(texture(material.specular, fs_in.TexCoords));
    
    return (ambient + diffuse + specular);
}

vec3 CalculatePointLight(int index, vec3 normal, vec3 viewDirection)
{
    vec3 lightDirection = normalize(pointLights[index].position - fs_in.Position);
    float diff = max(dot(normal, lightDirection), 0.0);
    vec3 reflectDirection = reflect(-lightDirection, normal);
    float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), material.shininess);

    float distance = length(pointLights[index].position - fs_in.Position);
    float constant = 1;
    float attenuation = 1.0 / (constant + pointLights[index].linear * distance + pointLights[index].quadratic * (distance * distance));
    vec3 ambient = pointLights[index].ambient * vec3(texture(material.diffuse, fs_in.TexCoords));
    vec3 diffuse = pointLights[index].diffuse * diff * vec3(texture(material.diffuse, fs_in.TexCoords));
    vec3 specular = pointLights[index].specular * spec * vec3(texture(material.specular, fs_in.TexCoords));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
}


vec3 CalcBumpedNormal()
{
    vec3 Normal = normalize(fs_in.Normal);
    vec3 Tangent = normalize(fs_in.Tangent);
    Tangent = normalize(Tangent - dot(Tangent, Normal) * Normal);
    vec3 Bitangent = cross(Tangent, Normal);
    vec3 BumpMapNormal = texture(material.normal, fs_in.TexCoords).xyz;
    BumpMapNormal = 2.0 * BumpMapNormal - vec3(1.0, 1.0, 1.0);
    vec3 NewNormal;
    mat3 TBN = mat3(Tangent, Bitangent, Normal);
    NewNormal = TBN * BumpMapNormal;
    NewNormal = normalize(NewNormal);
    return NewNormal;
}
