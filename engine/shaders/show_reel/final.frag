#version 460 core
out vec4 FragColor;

#define COUNT_POINT_LIGHTS 64
#define EPSILON 0.0001

struct Material
{
    sampler2D diffuse;
    sampler2D specular;
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

in VertexData
{
    vec3 position;
    vec2 texCoords;
    vec3 normal;
    vec3 tangent;
    vec4 lightSpacePosition;
} inData;

uniform Material material;
uniform DirectionalLight directionalLight;
uniform PointLight pointLights[COUNT_POINT_LIGHTS];
uniform vec3 viewPosition;
uniform vec3 color;
uniform sampler2D shadowMap;
uniform samplerCube skybox;

uniform bool normals_enabled;
uniform bool directional_enabled;
uniform int count_point_lights;
uniform bool skylight_enabled;

vec3 CalculateDirectionLight(vec3 normal, vec3 viewDirection, float shadow);
vec3 CalculatePointLight(int index, vec3 normal, vec3 viewDirection);
float GetShadow(vec4 lightSpacePos);
vec3 CalculateBumped();
vec3 SkyLight(vec3 normal, vec3 viewDirection);

void main()
{
    if(texture(material.diffuse, inData.texCoords).a < 0.05)
    {
        discard;
    }

    vec3 viewDirection = normalize(viewPosition - inData.position);

    vec3 calculatedNormal = normalize(inData.normal);
    if(normals_enabled)
    {
        calculatedNormal = CalculateBumped();
    }

    float shadow = GetShadow(inData.lightSpacePosition);

    vec3 resultColor;
    if(directional_enabled)
    {
        resultColor = CalculateDirectionLight(calculatedNormal, viewDirection, shadow);
    }

    if(skylight_enabled)
    {
        resultColor += SkyLight(calculatedNormal, viewDirection);
    }

    for(int i = 0; i < min(COUNT_POINT_LIGHTS, count_point_lights); ++i)
    {
        resultColor += CalculatePointLight(i, calculatedNormal, viewDirection);
    }

    float exposure = 1.2;
    resultColor = resultColor * exposure;
    resultColor = resultColor / (resultColor + 1.0);

    FragColor = vec4(color * resultColor, 1.0);
}

vec3 SkyLight(vec3 normal, vec3 viewDirection)
{
    // skybox reflections
    float diff = 1.0 - abs(dot(normal, viewDirection));
    vec3 I = normalize(inData.position - viewPosition);
    vec3 R = reflect(I, normalize(normal));
    return texture(skybox, R).rgb * diff;
}

vec3 CalculateDirectionLight(vec3 normal, vec3 viewDirection, float shadow)
{
    vec3 lightDirection = normalize(-directionalLight.direction);
    float diff = max(dot(normal, lightDirection), 0.0);

    vec3 reflectDirection = reflect(-lightDirection, normal);
    float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), material.shininess);

    vec3 ambient = directionalLight.ambient * vec3(texture(material.diffuse, inData.texCoords));
    vec3 diffuse = directionalLight.diffuse * diff * vec3(texture(material.diffuse, inData.texCoords));
    vec3 specular = directionalLight.specular * spec * vec3(texture(material.specular, inData.texCoords));

    return (ambient + (1.0 - shadow) * (diffuse + specular));
}

vec3 CalculatePointLight(int index, vec3 normal, vec3 viewDirection)
{
    vec3 lightDirection = normalize(pointLights[index].position - inData.position);
    float diff = max(dot(normal, lightDirection), 0.0);

    vec3 reflectDirection = reflect(-lightDirection, normal);
    float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), material.shininess);


    float distance = length(pointLights[index].position - inData.position);
    float constant = 1;
    float attenuation = 1.0 / (constant + pointLights[index].linear * distance + pointLights[index].quadratic * (distance * distance));
    
    vec3 ambient = pointLights[index].ambient * pointLights[index].color * vec3(texture(material.diffuse, inData.texCoords));
    vec3 diffuse = pointLights[index].diffuse * pointLights[index].color * diff * vec3(texture(material.diffuse, inData.texCoords));
    vec3 specular = pointLights[index].specular * pointLights[index].color * spec * vec3(texture(material.specular, inData.texCoords));
    
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
}

vec3 CalculateBumped()
{
    vec3 Normal = normalize(inData.normal);
    vec3 Tangent = normalize(inData.tangent);

    Tangent = normalize(Tangent - dot(Tangent, Normal) * Normal);

    vec3 Bitangent = cross(Normal, Tangent);
    vec3 BumpMapNormal = texture(material.normal, inData.texCoords).xyz;
    BumpMapNormal = 2.0 * BumpMapNormal - vec3(1.0, 1.0, 1.0);

    // z = sqrt(1 - x^2 - y^2)
    BumpMapNormal.z = sqrt(1.0 - dot(BumpMapNormal.xy, BumpMapNormal.xy));

    vec3 ResultNormal;
    mat3 TBN = mat3(Tangent, Bitangent, Normal);
    ResultNormal = TBN * BumpMapNormal;
    ResultNormal = normalize(ResultNormal);
    return ResultNormal;
}

float GetShadow(vec4 lightSpacePos)
{
    vec3 projCoords = lightSpacePos.xyz / lightSpacePos.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;

    vec3 normal = normalize(inData.normal);
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