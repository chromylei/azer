struct VS_OUTPUT {
  float4 Pos : SV_POSITION;
  float2 texcoord : TEXCOORD;
  float3 normal : NORMAL;
  float3 tangent : TANGENT;
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
Texture2D BumpTexture;
SamplerState ObjSamplerState;

float3 CalcBumpNormal(float3 normal, float3 tangent, float3 normalmap) {
  normalmap = (2.0 * normalmap) - 1.0;
  normal = normalize(normal);
  tangent = normalize(tangent - dot(tangent, normal) * normal);
  float3 bitangent = cross(normal, tangent);
  float3x3 texspace = float3x3(tangent, bitangent, normal);
  return normalize(mul(normalmap, texspace));
}

float4 ps_main(VS_OUTPUT input) : SV_TARGET {
  float4 sample_normal = BumpTexture.Sample(ObjSamplerState, input.texcoord);
  float3 normal = CalcBumpNormal(input.normal, input.tangent, sample_normal.xyz);
  normal = normalize(normal);
  float4 diffuse = ObjTexture.Sample(ObjSamplerState, input.texcoord);
  float3 final_color = diffuse * light.ambient;

  float3 ambient_factor = 0;
  if (dot(light.directional, normal) > 0) {
    ambient_factor = saturate(dot(light.directional, normal) *
                              light.diffuse * diffuse);
  }

  final_color += ambient_factor;
  return float4(final_color, diffuse.a);
}

