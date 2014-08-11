cbuffer cbPerObject {
  float4x4 WVP[500];
  float4x4 WORLD[500];
};

struct VS_OUTPUT {
  float4 Pos : SV_POSITION;
  float2 texcoord : TEXCOORD;
  float3 normal : NORMAL;
};


VS_OUTPUT vs_main(float4 inPos : POSITION,
                  float2 inTex : TEXCOORD,
                  float3 normal : NORMAL,
                  uint instanceID : SV_InstanceID) {
  VS_OUTPUT output;
  output.Pos = mul(inPos, WVP[instanceID]);
  output.texcoord = inTex;
  output.normal = mul(normal, WORLD[instanceID]);
  return output;
}
