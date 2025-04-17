cbuffer ConstBuf : register(b0)
{
  matrix transformations;
  matrix view;
  matrix projection;
  float4 color;
};

struct VS_IN
{
  float4 pos : POSITION;
  float2 uv : TEXCOORD0;
};

struct VS_OUT
{
  float4 pos : SV_POSITION;
  float2 uv : TEXCOORD0;
};

struct PS_IN
{
  float4 pos : SV_POSITION;
  float2 uv : TEXCOORD0;
};

VS_OUT VSMain(VS_IN input)
{
  VS_OUT o;
  o.pos = input.pos;
  o.uv = input.uv;
  return o;
}

float4 PSMain(PS_IN input) : SV_Target
{
  return float4(0.0, 0.0, 0.0, 0.5);
}