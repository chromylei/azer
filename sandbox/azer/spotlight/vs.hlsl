cbuffer cbPerObject {
  float4x4 WVP;
  float4x4 WORLD;
};

struct VS_OUTPUT {
  float4 Pos : SV_POSITION;
  float2 texcoord : TEXCOORD;
  float3 normal : NORMAL;
  float4 world_pos : POSITION;
};

VS_OUTPUT vs_main(float4 inPos : POSITION,
                  float2 inTex : TEXCOORD,
                  float3 normal : NORMAL) {
  VS_OUTPUT output;
  output.Pos = mul(inPos, WVP);
  output.world_pos = mul(inPos, WORLD);
  output.texcoord = inTex;
  output.normal = mul(float4(normal, 0.0), WORLD).xyz;
  return output;
}



