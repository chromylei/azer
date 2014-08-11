cbuffer cbPerObject {
  float4x4 WVP;
};

struct VS_OUTPUT {
  float4 Pos : SV_POSITION;
};

VS_OUTPUT vs_main(float3 inPos : POSITION) {
  VS_OUTPUT output;
  output.Pos = mul(float4(inPos, 1.0), WVP);
  return output;
}



