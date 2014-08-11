struct VS_OUTPUT {
  float4 Pos : SV_POSITION;
  float2 texcoord : TEXCOORD;
};

Texture2D ObjTexture;
SamplerState ObjSamplerState;

float4 ps_main(VS_OUTPUT input) : SV_TARGET {
  return ObjTexture.Sample(ObjSamplerState, input.texcoord );
  // return float4(1.0, 0.0, 1.0, 1.0);
}

