struct VS_OUTPUT {
  float4 Pos : SV_POSITION;
  float2 texcoord : TEXCOORD;
  float3 normal : NORMAL;
};

struct DirectionalLight {
  float4 ambient;
  float4 diffuse;
  float4 directional;
};

cbuffer cbPerFrame {
  DirectionalLight light;
}

Texture2D ObjTexture;
SamplerState ObjSamplerState;

float4 ps_main(VS_OUTPUT input) : SV_TARGET {
  input.normal = normalize(input.normal);
  float4 diffuse = ObjTexture.Sample(ObjSamplerState, input.texcoord );
  float3 final_color = diffuse * light.ambient;
  final_color += saturate(dot(light.directional, input.normal)
                          * light.diffuse *diffuse);
  return float4(final_color, diffuse.a);
}
