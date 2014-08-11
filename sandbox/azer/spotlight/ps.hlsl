struct VS_OUTPUT {
  float4 Pos : SV_POSITION;
  float2 texcoord : TEXCOORD;
  float3 normal : NORMAL;
  float4 world_pos: POSITION;
};

struct DirectionalLight {
  float4 ambient;
  float4 diffuse;
  float4 directional;
};

struct Attenuation {
  float c;
  float l;
  float e;
};

struct SpotLight {
  float4 ambient;
  float4 diffuse;
  float4 pos;
  float4 dir;
  Attenuation atten;
  float range;
  float cone;
};

cbuffer cbPerFrame {
  SpotLight light;
}

Texture2D ObjTexture;
SamplerState ObjSamplerState;

float3 spot_light(float3 worldpos, float3 normal, SpotLight l) {
  float3 light_to_pixel = light.pos - worldpos;
  float dist = length(light_to_pixel);
  if (dist > l.range) {
    return float3(0.0, 0.0, 0.0);
  }

  // normalize
  light_to_pixel /= dist;

  float light_power = dot(light_to_pixel, normal);
  if (light_power > 0.0) {
    float3 factor = l.diffuse;
    factor /= (light.atten.c
               + light.atten.l * dist
               + light.atten.e * dist * dist);
    float reflect_factor = dot(-light_to_pixel, light.dir.xyz);
    factor *= pow(max(reflect_factor, 0.0), light.cone); // light.cone);
    return factor + l.ambient;
  } else {
    return float3(0.0, 0.0, 0.0);
  }
}

float4 ps_main(VS_OUTPUT input) : SV_TARGET {
  input.normal = normalize(input.normal);
  float4 diffuse = ObjTexture.Sample(ObjSamplerState, input.texcoord);
  
  float3 spot_light_factor = spot_light(input.world_pos.xyz, input.normal, light);

  float3 final_color = diffuse.xyz * spot_light_factor;
  final_color = saturate(final_color);
  return float4(final_color, diffuse.a);
}

