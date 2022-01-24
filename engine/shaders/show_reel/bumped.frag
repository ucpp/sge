#version 460 core
out vec4 FragColor;

struct Material
{
    sampler2D diffuse;
    sampler2D normal;
};

struct DirectionalLight
{
    vec3 direction;

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
} inData;

uniform Material material;
uniform DirectionalLight directionalLight;
uniform vec3 viewPosition;
uniform vec3 color;

uniform bool normals_enabled;
uniform bool directional_enabled;

vec3 CalculateDirectionLight(vec3 normal, vec3 viewDirection);
vec3 CalculateBumped();

void main()
{
    vec3 viewDirection = normalize(viewPosition - inData.position);

    vec3 calculatedNormal = normalize(inData.normal);
    if(normals_enabled)
    {
        calculatedNormal = CalculateBumped();
    }

    vec3 resultColor = vec3(texture(material.diffuse, inData.texCoords));
    if(directional_enabled)
    {
        resultColor = CalculateDirectionLight(calculatedNormal, viewDirection);
    }

    FragColor = vec4(color * resultColor, 1.0);
}

vec3 CalculateDirectionLight(vec3 normal, vec3 viewDirection)
{
    vec3 lightDirection = normalize(-directionalLight.direction);
    float diff = max(dot(normal, lightDirection), 0.0);

    return directionalLight.diffuse * diff * vec3(texture(material.diffuse, inData.texCoords));
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