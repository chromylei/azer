struct VS_OUTPUT {
  float4 Pos : SV_POSITION;
  float3 Tex : TEXTURE;
};

TextureCube ObjTexture;
SamplerState ObjSamplerState;

float4 ps_main(VS_OUTPUT input) : SV_TARGET {
  return ObjTexture.Sample(ObjSamplerState, input.Tex);
}

