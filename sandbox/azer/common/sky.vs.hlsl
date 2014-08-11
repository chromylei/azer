cbuffer cbPerObject {
  float4x4 WVP;
  float4x4 World;
};

struct VS_OUTPUT {
  float4 Pos : SV_POSITION;
  float3 texcoord : TEXCOORD;
};

VS_OUTPUT vs_main(float4 inPos : POSITION) {
  VS_OUTPUT output;
  output.Pos = mul(inPos, WVP).xyww;
  output.texcoord = inPos;
  return output;
}
