struct VS_OUTPUT {
  float4 Pos : SV_POSITION;
  float2 texcoord : TEXCOORD;
};

Texture2D ObjTexture;
SamplerState ObjSamplerState;

float4 ps_main(VS_OUTPUT input) : SV_TARGET {
  float4 diffuse = ObjTexture.Sample(ObjSamplerState, input.texcoord );
  clip(diffuse.a - .10);
  return diffuse;
}

