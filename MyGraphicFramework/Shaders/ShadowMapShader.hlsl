struct VS_IN
{
  float4 pos : POSITION0;
  float2 tex : TEXCOORD0;
  float3 normal : NORMAL;
};

struct PS_IN
{
  float4 pos : SV_POSITION;
};

cbuffer ConstBuf : register(b0)
{
  matrix transformations;
  matrix view;
  matrix projection;
  float4 color;
  float time;
  float amplitude;
};

PS_IN VSMain(VS_IN input)
{
  PS_IN output = (PS_IN) 0;
  float4 worldPosition = mul(input.pos, transformations);
  output.pos = mul(worldPosition, mul(view, projection));
  return output;
};

float4 PSMain(PS_IN input) : SV_Target
{
  float depthValue = input.pos.z / input.pos.w;
  return float4(depthValue, depthValue, depthValue, 1.0f);
  //return float4(0, 0, depthValue, 1.0f);
};