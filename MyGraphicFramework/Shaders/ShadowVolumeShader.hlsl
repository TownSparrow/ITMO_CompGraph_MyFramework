struct VS_IN
{
  float4 pos : POSITION0;
  float2 tex : TEXCOORD0;
  float3 normal : NORMAL;
};

struct GS_IN
{
  float4 pos : SV_POSITION;
  float2 tex : TEXCOORD0;
  float3 normal : TEXCOORD1;
  float3 worldPosition : TEXCOORD2;
};

struct GS_OUT
{
  float4 position : SV_POSITION;
};

cbuffer ConstBuf : register(b0)
{
  matrix transformations;
  matrix view;
  matrix projection;
  float4 color;
};

//Lighting
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

cbuffer LightBuf : register(b1)
{
  Material material;
  DirectionalLight dirLight;
  PointLight pntLights[8];
  float4 spectatorLocation;
  Matrix lightSpace;
};

Texture2D diffuseMap : register(t0);
SamplerState samp : register(s0);

Texture2D shadowMap : register(t1);
SamplerComparisonState shadowSamp : register(s1);

//Shaders themselves
GS_IN VSMain(VS_IN input)
{
  GS_IN output = (GS_IN) 0;
	
  float4 pos = float4(input.pos.xyzw);
  pos = mul(pos, transformations);
    
  output.worldPosition = pos;
    
  pos = mul(pos, view);
  pos = mul(pos, projection);
    
  output.pos = pos;
    
  output.tex = input.tex.xy;
  output.normal = normalize(mul(float4(input.normal, 0.0f), transformations));
	
  return output;
};

bool IsSilhouetteEdge(float3 normal1, float3 normal2, float3 lightDir)
{
    
  bool dot1 = dot(normal1, lightDir) > 0;
  bool dot2 = dot(normal2, lightDir) > 0;
  return dot1 != dot2;
};

[maxvertexcount(6)]
void GSMain(triangleadj GS_IN input[6], inout TriangleStream<GS_OUT> triStream)
{
  float3 lightDir = normalize(dirLight.direction);
  float3 faceNormal = normalize(cross(input[2].worldPosition - input[0].worldPosition, input[4].worldPosition - input[2].worldPosition));
  bool isThisFaceFront = dot(faceNormal, lightDir) < 0;
  
  for (int i = 0; i < 3; i++)
  {
    bool isSilhouette = false;
        
    int current = i * 2;
    int next = (current + 2) % 6;
    int third = (current + 4) % 6;
    int neighborIdx = current + 1;
        
    float3 faceNormal = normalize(cross(input[current].worldPosition - input[next].worldPosition, input[third].worldPosition - input[next].worldPosition));
    faceNormal = normalize(input[current].normal + input[next].normal + input[third].normal);
    isThisFaceFront = dot(faceNormal, lightDir) <= 0;
        
    if (neighborIdx < 6)
    {
      float3 neighborFaceNormal = normalize(cross(input[neighborIdx].worldPosition - input[third].worldPosition,
                                input[next].worldPosition - input[third].worldPosition));
      neighborFaceNormal = normalize(input[neighborIdx].normal + input[next].normal + input[third].normal);
            //neighborFaceNormal = input[neighborIdx].normal;
      bool isNeighborFrontFace = dot(neighborFaceNormal, lightDir) <= 0;
      isSilhouette = (isThisFaceFront && !isNeighborFrontFace);
    }
    
    if (isSilhouette)
    {
      GS_OUT output;
            
      float3 extrudeWorld1 = input[next].worldPosition + lightDir * 10000.0;
      float3 extrudeWorld2 = input[third].worldPosition + lightDir * 10000.0;
            
      float4 extrudePos1 = mul(float4(extrudeWorld1, 1.0f), mul(view, projection));
      float4 extrudePos2 = mul(float4(extrudeWorld2, 1.0f), mul(view, projection));
            
      output.position = input[next].pos;
      triStream.Append(output);
      output.position = input[third].pos;
      triStream.Append(output);
      output.position = extrudePos1;
      triStream.Append(output);
      triStream.RestartStrip();
            
      output.position = input[third].pos;
      triStream.Append(output);
      output.position = extrudePos2;
      triStream.Append(output);
      output.position = extrudePos1;
      triStream.Append(output);
      triStream.RestartStrip();
    };
  };
};

[maxvertexcount(6)]
void main(triangleadj GS_IN input[6], inout TriangleStream<GS_OUT> triStream)
{
  float3 lightDir = normalize(dirLight.direction);
    
  GS_IN v0 = input[0];
  GS_IN v2 = input[2];
  GS_IN v4 = input[4];
   // bool isSilhouette = IsSilhouetteEdge(v0.normal, v2.normal, lightDir)  || IsSilhouetteEdge(v2.normal, v4.normal, lightDir) || IsSilhouetteEdge(v4.normal, v0.normal, lightDir);
    
  float3 faceNormal = normalize(cross(input[2].worldPosition - input[0].worldPosition, input[2].worldPosition - input[4].worldPosition));
  bool isFrontFace = dot(faceNormal, lightDir) < 0;
    
    //if (!isFrontFace) return;
   
  for (int i = 0; i < 3; i++)
  {
    int current = i * 2;
    int next = (current + 2) % 6;
    int third = (current + 4) % 6;
    bool isSilhouette = false;
        
    int neighborIdx = third + 1;
        
    if (neighborIdx < 6)
    {
      float3 neighborFaceNormal = normalize(cross(input[current].worldPosition - input[next].worldPosition, input[current].worldPosition - input[neighborIdx].worldPosition));
      float3 thisFaceNormal = normalize(cross(input[next].worldPosition - input[current].worldPosition, input[next].worldPosition - input[third].worldPosition));
           
      bool isNeighborFrontFace = dot(neighborFaceNormal, lightDir) < 0;
      bool isThisFrontFace = dot(thisFaceNormal, lightDir) < 0;
      isSilhouette = (isThisFrontFace != isNeighborFrontFace);
    }
    
    if (isSilhouette)
    {
      GS_OUT output;
            
      float3 extrudeWorld1 = input[i].worldPosition + lightDir * 10000.0;
      float3 extrudeWorld2 = input[next].worldPosition + lightDir * 10000.0;
            
      float4 extrudePos1 = mul(float4(extrudeWorld1, 1.0f), mul(view, projection));
      float4 extrudePos2 = mul(float4(extrudeWorld2, 1.0f), mul(view, projection));
            
      output.position = input[i].pos;
      triStream.Append(output);
            
      output.position = input[next].pos;
      triStream.Append(output);
            
      output.position = extrudePos1;
      triStream.Append(output);
            
      triStream.RestartStrip();
            
      output.position = input[next].pos;
      triStream.Append(output);
            
      output.position = extrudePos2;
      triStream.Append(output);
            
      output.position = extrudePos1;
      triStream.Append(output);
            
      triStream.RestartStrip();
            
      return;
    };
  };
};

[maxvertexcount(6)]
void GSMain2(triangleadj GS_IN input[6], inout TriangleStream<GS_OUT> triStream)
{
  float3 L = normalize(dirLight.direction.xyz);
  const float dist = 10000.0;
  
  [unroll]
  for (int e = 0; e < 3; ++e)
  {
    int i0 = e * 2;
    int i1 = (i0 + 2) % 6;
    int i2 = (i0 + 4) % 6;
    int inbr = i0 + 1;
    
    GS_IN V0 = input[i0];
    GS_IN V1 = input[i1];
    GS_IN V2 = input[i2];
    GS_IN Vnbr = input[inbr];
    
    float3 nfn = normalize(cross(V1.worldPosition - Vnbr.worldPosition, V2.worldPosition - Vnbr.worldPosition));
    float3 tfn = normalize(cross(V1.worldPosition - V0.worldPosition, V2.worldPosition - V0.worldPosition));
        
       // float3 nfn = normalize(cross(V1.pos.xyz - Vnbr.pos.xyz, V2.pos.xyz - Vnbr.pos.xyz));
        //float3 tfn = normalize(cross(V1.pos.xyz - V0.pos.xyz, V2.pos.xyz - V0.pos.xyz));        
        
        //float4 nfn = normalize(mul(float4(Vnbr.normal, 1.0f), mul(view, projection)) + 
        //   mul(float4(V2.normal, 1.0f), mul(view, projection)) + mul(float4(V1.normal, 1.0f), mul(view, projection)));
        //float4 tfn = normalize(mul(float4(V0.normal, 1.0f), mul(view, projection)) +
        //   mul(float4(V2.normal, 1.0f), mul(view, projection)) + mul(float4(V1.normal, 1.0f), mul(view, projection)));
        
    if (IsSilhouetteEdge(nfn, tfn, L))
    {
      float3 E0w = V1.worldPosition + L * dist;
      float3 E1w = V2.worldPosition + L * dist;
      
      float4 E0 = mul(float4(E0w, 1), view);
      E0 = mul(E0, projection);
      float4 E1 = mul(float4(E1w, 1), view);
      E1 = mul(E1, projection);
      
      GS_OUT o;
      
      o.position = V1.pos;
      triStream.Append(o);
      o.position = V2.pos;
      triStream.Append(o);
      o.position = E1;
      triStream.Append(o);
      triStream.RestartStrip();
      
      o.position = V1.pos;
      triStream.Append(o);
      o.position = E1;
      triStream.Append(o);
      o.position = E0;
      triStream.Append(o);
      triStream.RestartStrip();
    }
  };
};