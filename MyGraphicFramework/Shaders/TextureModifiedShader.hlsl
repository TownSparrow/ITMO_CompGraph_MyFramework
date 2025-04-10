#define MAX_POINT_LIGHTS 8

struct ConstantData {
  float4 offset;
  float4 color;
};

cbuffer Transform : register(b0) {
  matrix transformations;
  matrix view;
  matrix projection;
  float4 color;
  float time;
  float amplitude;
};

struct VS_IN {
  float4 pos : POSITION0;
  float2 tex : TEXCOORD0;
  float3 normal : NORMAL;
};


struct PS_IN {
  float4 pos : SV_POSITION;
  float2 tex : TEXCOORD0;
  float3 normal : TEXCOORD1;
  float3 worldPos : TEXCOORD2;
};

struct Material {
  float4 ambient;
  float4 diffuse;
  float4 specular;
};

struct DirectionalLight {
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

cbuffer LightBuf : register(b1) {
  Material material;
  DirectionalLight dirLight;
  //PointLight pntLight;
  PointLight pntLights[MAX_POINT_LIGHTS];
  float4 spectatorLocation;
  matrix lightSpace;
};

Texture2D diffuseMap : register(t0);
SamplerState samp : register(s0);

Texture2D shadowMap : register(t1);
SamplerComparisonState shadowSamp : register(s1);

void DirectionalLightComputing(
    Material material,
    DirectionalLight light,
    float3 normal,
    float3 vector2spectator,
    out float4 ambient,
    out float4 diffuse,
    out float4 specular
) {
  ambient = float4(0, 0, 0, 0);
  diffuse = float4(0, 0, 0, 0);
  specular = float4(0, 0, 0, 0);

  ambient = material.ambient * light.ambient;
    
  float3 vector2light = -light.direction.xyz;
  float diffuseFactor = max(dot(vector2light, normal), 0.0f);
  diffuse = diffuseFactor * material.diffuse * light.diffuse;
    
  float3 reflectedLight = reflect(-vector2light, normal);
  float angle = min(max(dot(normalize(reflectedLight), normalize(vector2light)) + 1.0f, 0.0f), 1.0f);
  float specularFactor = pow(max(dot(reflectedLight, vector2spectator), 0.0f), material.specular.w) * angle;
  specular = specularFactor * material.specular * light.specular;
}

void PointLightComputing(
    Material material,
    PointLight light,
    float3 position,
    float3 normal,
    float3 vector2spectator,
    out float4 ambient,
    out float4 diffuse,
    out float4 specular
) {
  ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
  diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
  specular = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
  float3 vector2light = light.position - position;
  float distance2light = length(vector2light);
  if (distance2light > light.range) return;
    
  vector2light /= distance2light;

  ambient = material.ambient * light.ambient;

  float diffuseFactor = max(dot(vector2light, normal), 0.0f);
  diffuse = diffuseFactor * material.diffuse * light.diffuse;
    
  float3 reflectedLight = reflect(-vector2light, normal);
  float angle = min(max(dot(normalize(reflectedLight), normalize(vector2light)) + 1.0f, 0.0f), 1.0f);
  float specularFactor = pow(max(dot(reflectedLight, vector2spectator), 0.0f), material.specular.w) * angle;
  specular = specularFactor * material.specular * light.specular;
    
  float att = 1.0 / dot(light.attentuation.xyz, float3(1.0f, distance2light, pow(distance2light, 2)));
  att = max(0.0f, 1 - distance2light / light.range);
  ambient *= att;
  diffuse *= att;
  specular *= att;
  
  //specular *= 0.01; // - (distance2light / light.range);
}

float CalculateShadow(float4 lightSpacePos)
{
  float3 projCoords = lightSpacePos.xyz / lightSpacePos.w;
    
  projCoords.xy = projCoords.xy * 0.5 + 0.5;
  projCoords.y = 1.0 - projCoords.y;
    
  float currentDepth = projCoords.z;
    
  if (currentDepth > 1.0 || projCoords.x < 0 || projCoords.x > 1 ||
        projCoords.y < 0 || projCoords.y > 1)
  {
    return 1.0;
  }
    
  float shadow = 0.0;
  float2 texelSize = 1.0 / float2(2048, 2048);
    
  for (int x = -1; x <= 1; ++x)
  {
    for (int y = -1; y <= 1; ++y)
    {
      shadow += shadowMap.SampleCmpLevelZero(shadowSamp, (projCoords.xy + float2(x, y) * texelSize), currentDepth);
    }
  }
    
  shadow /= 9.0;
    
  return shadow;
}

PS_IN VSMain(VS_IN input) {
  PS_IN output = (PS_IN) 0;
  
  float4 worldPos = mul(float4(input.pos.xyzw), transformations);
  output.worldPos = worldPos.xyz;
    
  worldPos = mul(worldPos, view);
  worldPos = mul(worldPos, projection);
  output.pos = worldPos;
  
  output.tex = input.tex.xy;
  output.normal = normalize(mul(float4(input.normal, 0.0), transformations).xyz);
    
  return output;
}

float4 PSMain(PS_IN input) : SV_Target {
  float4 texColor = diffuseMap.Sample(samp, input.tex);
  float3 lightingColor = float3(1.0f, 1.0f, 1.0f);
  float3 vector2spectator = normalize(spectatorLocation.xyz - input.worldPos);
    
  float4 ambientDir, diffuseDir, specularDir;
  float4 ambientPt, diffusePt, specularPt;
  
  float4 lightSpacePos = mul(float4(input.worldPos, 1.0f), lightSpace);
  float shadowFactor = CalculateShadow(lightSpacePos);
  
  DirectionalLightComputing(material, dirLight, input.normal, vector2spectator, ambientDir, diffuseDir, specularDir);
  //lightingColor += ambientDir.rgb + diffuseDir.rgb + specularDir.rgb;
  lightingColor += ambientDir.xyz  + diffuseDir.xyz + specularDir.xyz;
  lightingColor *= shadowFactor;
  
  // For single point light
  //float4 ambientPoint, diffusePoint, specularPoint;
  //PointLightComputing(material, pntLight, input.worldPos, input.normal, vector2spectator, ambientPoint, diffusePoint, specularPoint);
  //lightingColor += ambientPoint.rgb + diffusePoint.rgb + specularPoint.rgb;
  // For point lights array

  for (int i = 0; i < MAX_POINT_LIGHTS; i++) {
    PointLightComputing(material, pntLights[i], input.worldPos, input.normal, vector2spectator, ambientPt, diffusePt, specularPt);
    lightingColor += ambientPt.xyz + diffusePt.xyz + specularPt.xyz;
  }
  
  float3 finalColor = texColor.rgb * lightingColor;
  return float4(finalColor, texColor.a);
  //return float4(finalColor, material.diffuse.a);
}
