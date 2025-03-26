// �������������� ������ (�� ����������)
struct ConstantData
{
  float4 offset;
  float4 color;
};

// ����� �������������� (��� � ������� �������)
cbuffer Transform : register(b0)
{
  matrix transformations;
  matrix view;
  matrix projection;
  float4 color;
  float time;
  float amplitude;
};

// ��������� ������� ������ ���������� �������. 
// ��������� ���� normal ��� ���������� ���������.
struct VS_IN
{
  float4 pos : POSITION0;
  float2 tex : TEXCOORD0;
  float3 normal : NORMAL;
};

// �������� ��������� ���������� ������� � ����������� SV_POSITION � TEXCOORD0.
// ��������� ���� normal � worldPos ��� ������������� � ���������� �������.
struct PS_IN
{
  float4 pos : SV_POSITION;
  float2 tex : TEXCOORD0;
  float3 normal : TEXCOORD1;
  float3 worldPos : TEXCOORD2;
};

Texture2D diffuseMap : register(t0);
SamplerState samp : register(s0);

// ��������� ��� �������� ���������� � ���������� �����
struct Material
{
  float4 ambient;
  float4 diffuse;
  float4 specular;
};

struct DirectionalLight
{
  float4 ambient;
  float4 diffuse;
  float4 specular;
  float4 direction;
};

struct PointLight
{
  float4 ambient;
  float4 diffuse;
  float4 specular;
  float3 position;
  float range;
  float4 attentuation;
};

// ����� � ����������� ���������
cbuffer LightBuf : register(b1)
{
  Material material;
  DirectionalLight dirLight;
  PointLight pntLight;
  float4 spectatorLocation;
};

//
// �������������� ������� ��� ������� ��������� �� ������������� ���������,
// ������������� ������ ������������� ��� ���������� specular.
//
void DirectionalLightComputing(
    Material material,
    DirectionalLight light,
    float3 normal,
    float3 vector2spectator,
    out float4 ambient,
    out float4 diffuse,
    out float4 specular
)
{
    // �������������� ���������� ������
  ambient = float4(0, 0, 0, 0);
  diffuse = float4(0, 0, 0, 0);
  specular = float4(0, 0, 0, 0);

    // Ambient: ������� �����
  ambient = material.ambient * light.ambient;
    
    // Diffuse: ������������ ������ � ����� (��� ������������, ��� � ������� �������������)
  float3 vector2light = -light.direction.xyz;
  float diffuseFactor = max(dot(vector2light, normal), 0.0f);
  diffuse = diffuseFactor * material.diffuse * light.diffuse;
    
    // Specular: ������������ ��������� ������ � �������������� ��������� angle
  float3 reflectedLight = reflect(-vector2light, normal);
  float angle = min(max(dot(normalize(reflectedLight), normalize(vector2light)) + 1.0f, 0.0f), 1.0f);
  float specularFactor = pow(max(dot(reflectedLight, vector2spectator), 0.0f), material.specular.w) * angle;
  specular = specularFactor * material.specular * light.specular;
}

//
// �������������� ������� ��� ������� ��������� �� ��������� ���������,
// �� ������� �������������.
//
void PointLightComputing(
    Material material,
    PointLight light,
    float3 position,
    float3 normal,
    float3 vector2spectator,
    out float4 ambient,
    out float4 diffuse,
    out float4 specular
)
{
  ambient = float4(0, 0, 0, 0);
  diffuse = float4(0, 0, 0, 0);
  specular = float4(0, 0, 0, 0);
    
  float3 vector2light = light.position - position;
  float distance2light = length(vector2light);
  if (distance2light > light.range)
    return;
    
  vector2light /= distance2light;
    
    // Ambient
  ambient = material.ambient * light.ambient;
    
    // Diffuse
  float diffuseFactor = max(dot(vector2light, normal), 0.0f);
  diffuse = diffuseFactor * material.diffuse * light.diffuse;
    
    // Specular: ���������� ������������� ���������
  float3 reflectedLight = reflect(-vector2light, normal);
  float angle = min(max(dot(normalize(reflectedLight), normalize(vector2light)) + 1.0f, 0.0f), 1.0f);
  float specularFactor = pow(max(dot(reflectedLight, vector2spectator), 0.0f), material.specular.w) * angle;
  specular = specularFactor * material.specular * light.specular;
    
    // ��������� ��������� (attenuation)
  float att = 1.0 / dot(light.attentuation.xyz, float3(1.0f, distance2light, distance2light * distance2light));
  diffuse *= att;
  specular *= att;
}

//
// ��������� ������
//
PS_IN VSMain(VS_IN input)
{
  PS_IN output = (PS_IN) 0;
    
    // ��������� ������� �������
  float4 worldPos = mul(input.pos, transformations);
  output.worldPos = worldPos.xyz;
    
    // ��������� ������� � clip-space
  float4 clipPos = mul(worldPos, view);
  clipPos = mul(clipPos, projection);
  output.pos = clipPos;
    
    // ������� ���������� ����������
  output.tex = input.tex;
    
    // ����������� �������
  output.normal = normalize(mul(float4(input.normal, 0.0), transformations).xyz);
    
  return output;
}

//
// ���������� ������ � ����������� ���������
//
float4 PSMain(PS_IN input) : SV_Target
{
    // ���������� ���� ��������
  float4 texColor = diffuseMap.Sample(samp, input.tex);
    
    // �������������� �������� ���������
  float3 lightingColor = float3(0, 0, 0);
    
    // ��������� ������ �� ����� �� ������ (�����������)
  float3 vector2spectator = normalize(spectatorLocation.xyz - input.worldPos);
    
    // ������ ������ ������������� ���������
  float4 ambientDir, diffuseDir, specularDir;
  DirectionalLightComputing(material, dirLight, input.normal, vector2spectator, ambientDir, diffuseDir, specularDir);
  lightingColor += ambientDir.rgb + diffuseDir.rgb + specularDir.rgb;
    
    // ������ ������ ��������� ���������
  float4 ambientPoint, diffusePoint, specularPoint;
  PointLightComputing(material, pntLight, input.worldPos, input.normal, vector2spectator, ambientPoint, diffusePoint, specularPoint);
  lightingColor += ambientPoint.rgb + diffusePoint.rgb + specularPoint.rgb;
    
    // �������� ���� = ���������� ���� * ��������� ���������
  float3 finalColor = texColor.rgb * lightingColor;
  return float4(finalColor, texColor.a);
}
