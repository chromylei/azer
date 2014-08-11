cbuffer cbPerObject {
  float4x4 WVP;
  float4x4 WORLD;
};

struct VS_OUTPUT {
  float4 Pos : SV_POSITION;
  float2 texcoord : TEXCOORD;
  float3 normal : NORMAL;
  float3 tangent : TANGENT;
};


VS_OUTPUT vs_main(float4 inPos : POSITION,
                  float2 inTex : TEXCOORD,
                  float3 normal : NORMAL,
                  float3 tangent: TANGENT) {
  VS_OUTPUT output;
  output.Pos = mul(inPos, WVP);
  output.texcoord = inTex;
  output.normal = mul(normal, WORLD);
  output.tangent = mul(tangent, WORLD);
  return output;
}



