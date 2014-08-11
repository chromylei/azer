
struct VS_OUTPUT {
  float4 pos : SV_POSITION;
  float2 tex : TEXCOORD;
};


VS_OUTPUT vs_main(float3 inPos : POSITION) {
  VS_OUTPUT output;
  output.pos = float4(inPos, 1.0);
  return output;
}
