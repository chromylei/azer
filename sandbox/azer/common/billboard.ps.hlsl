struct VS_OUTPUT {
  float4 pos : SV_POSITION;
  float2 tex : TEXCOORD;
};

Texture2D ObjTexture;
SamplerState ObjSamplerState;

float4 ps_main(VS_OUTPUT input) : SV_TARGET {
  float4 diffuse = ObjTexture.Sample(ObjSamplerState, input.tex);
  clip(diffuse.a - 0.1f);
  return diffuse;
}

