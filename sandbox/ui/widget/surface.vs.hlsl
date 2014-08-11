struct VS_OUTPUT {
  float4 Pos : SV_POSITION;
  float2 texcoord : TEXCOORD;
};

VS_OUTPUT vs_main(float4 inPos : POSITION,
                  float2 inTex : TEXCOORD) {
  VS_OUTPUT output;
  output.Pos = inPos;
  output.texcoord = inTex;
  return output;
}



