struct VS_OUTPUT {
  float4 Pos : SV_POSITION;
};

cbuffer cbPerObject {
  float4x4 WVP;
};

VS_OUTPUT vs_main(float4 inPos : POSITION) {
  VS_OUTPUT output;
  output.Pos = mul(inPos, WVP);
  return output;
}
