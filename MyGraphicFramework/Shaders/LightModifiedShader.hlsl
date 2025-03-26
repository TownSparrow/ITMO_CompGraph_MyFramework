struct ConstantData
{
  float4 offset;
  float4 color;
};

cbuffer Transform : register(b0)
{
  matrix transformations;
  matrix view;
  matrix projection;
  float4 color;
  float time;
  float amplitude;
}

struct VS_IN
{
  float4 pos : POSITION0;
  //float4 col : COLOR0;
  float2 tex : TEXCOORD0;
};

struct PS_IN
{
  float4 pos : SV_POSITION;
  //float4 col : COLOR;
  float2 tex : TEXCOORD0;
};

Texture2D diffuseMap : register(t0);
SamplerState samp : register(s0);

PS_IN VSMain(VS_IN input)
{
  //PS_IN output;
  PS_IN output = (PS_IN) 0;
  
  // Default
  //output.pos = input.pos;
  //output.col = input.col;
  
  // Modiefied with Constant Buffer
  // default:
  float4 pos = float4(input.pos.xyz, 1.0f);
  pos = mul(pos, transformations);
  pos = mul(pos, view);
  pos = mul(pos, projection);
    
  output.pos = pos;
  //output.col = (input.col + color);
  output.tex = input.tex.xy;
  //output.tex = float2(1.0f - input.tex.x, input.tex.y);
  
  return output;
}

float4 PSMain(PS_IN input) : SV_Target
{
  //float4 col = input.col;
  //return col;
  
  float4 texColor = diffuseMap.Sample(samp, input.tex);
  return texColor;
}