struct ConstantData
{
  matrix transformations;
  matrix view;
  matrix projection;
  float4 targetColor;
  float time;
  float amplitude;
};

cbuffer Transform : register(b0)
{
  ConstantData cb;
}

struct VS_IN
{
  float4 pos : POSITION;
  float4 col : COLOR;
  uint vertexID : SV_VertexID;
};

struct VS_OUT
{
  float4 pos : SV_POSITION;
  float4 col : COLOR;
};

VS_OUT VSMain(VS_IN input)
{
  VS_OUT output;
  float4 pos = input.pos;
  float4 col = input.col;

  // Calculate sin function
  //float modTime = fmod(cb.time, 2.0f);
  //float t = sin(3.14159265 * modTime); // use π
  //float t = sin(3.14159265 * cb.time);
  float t = sin(cb.time);
  
  // Vertex pulsation
  if (input.vertexID % 5 == 0)
  {
    float3 norm = normalize(input.pos.xyz);
    //pos.xyz += norm * sin(cb.time + pos.x) * cb.amplitude;
    pos.xyz += norm * t * cb.amplitude;
    //pos.xyz = lerp(pos.xyz, pos.xyz + norm * cb.amplitude, t);
    
    col = lerp(input.col, cb.targetColor, t);
    
  }

  pos = mul(pos, cb.transformations);
  pos = mul(pos, cb.view);
  pos = mul(pos, cb.projection);
  output.pos = pos;

  // Color pulsation
  //output.col = lerp(input.col, cb.targetColor, t);
  output.col = col;
  
  return output;
}

float4 PSMain(VS_OUT input) : SV_Target
{
  return input.col;
}
