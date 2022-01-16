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
uniform sampler2D shadowMap;
uniform DirectionalLight directionalLight;

#define COUNT_POINT_LIGHTS 4
uniform PointLight pointLights[COUNT_POINT_LIGHTS];
uniform vec3 viewPosition;

uniform bool normals_enabled;

in VS_OUT
{
    vec3 Position;
    vec2 TexCoords;
    vec3 Normal;
    vec3 Tangent;
    vec4 LightSpacePosition;
} fs_in;

vec3 CalculateDirectionLight(vec3 normal, vec3 viewDirection, float shadow);
vec3 CalculatePointLight(int index, vec3 normal, vec3 viewDirection);
vec3 CalcBumpedNormal();

float GetShadow(vec4 lightSpacePosition);

void main()
{
    if(texture(material.diffuse, fs_in.TexCoords).a < 0.05)
    {
        discard;
    }

    vec3 norm = normalize(fs_in.Normal);
    if(normals_enabled)
    {
        norm = CalcBumpedNormal();
    }

    vec3 viewDirection = normalize(viewPosition - fs_in.Position);
    //norm = (norm + vec3(1))/2;
    //FragColor = vec4(norm.r, norm.g, norm.b, 1.0);

    float shadow = GetShadow(fs_in.LightSpacePosition);

    vec3 resultColor = CalculateDirectionLight(norm, viewDirection, shadow);

    for(int i = 0; i < COUNT_POINT_LIGHTS; ++i)
    {
        resultColor += CalculatePointLight(i, norm, viewDirection);
    }

    FragColor = vec4(resultColor, 1.0);
}

vec3 CalculateDirectionLight(vec3 normal, vec3 viewDirection, float shadow)
{
    vec3 lightDirection = normalize(-directionalLight.direction);
    float diff = max(dot(normal, lightDirection), 0.0);
    vec3 reflectDirection = reflect(-lightDirection, normal);
    float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), material.shininess);

    vec3 ambient = directionalLight.ambient * vec3(texture(material.diffuse, fs_in.TexCoords));
    vec3 diffuse = directionalLight.diffuse * diff * vec3(texture(material.diffuse, fs_in.TexCoords));
    vec3 specular = directionalLight.specular * spec * vec3(texture(material.specular, fs_in.TexCoords));

    return (ambient + (1.0 - shadow) * (diffuse + specular));
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

    vec3 Bitangent = cross(Normal, Tangent);
    vec3 BumpMapNormal = texture(material.normal, fs_in.TexCoords).xyz;
    BumpMapNormal = 2.0 * BumpMapNormal - vec3(1.0, 1.0, 1.0);

    // z = sqrt(1 - x^2 - y^2)
    BumpMapNormal.z = sqrt(1.0 - dot(BumpMapNormal.xy, BumpMapNormal.xy));

    vec3 NewNormal;
    mat3 TBN = mat3(Tangent, Bitangent, Normal);
    NewNormal = TBN * BumpMapNormal;
    NewNormal = normalize(NewNormal);
    return NewNormal;
}

#define EPSILON 0.0001

float GetShadow(vec4 lightSpacePosition)
{
    vec3 projCoords = lightSpacePosition.xyz / lightSpacePosition.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;

    vec3 normal = normalize(fs_in.Normal);
    vec3 light_direction = normalize(-directionalLight.direction);
    float bias = max(0.05 * (1.0 - dot(normal, light_direction)), 0.005);

    // PCF
    float shadow;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int y = -1; y <= 1; ++y)
    {
        for(int x = -1; x <= 1; ++x)
        {
            float pcf = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcf ? 1.0 : 0.0;
        }
    }
    shadow = shadow / 9.0;
    // end of PCF

    if(projCoords.z > 1.0)
    {
        shadow = 0;
    }

    return shadow;
}