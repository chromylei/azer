cbuffer cbPerFrame {
  float4x4 WVP;
  float4 CPOS;
  float2 SIZE;
};

struct VS_OUTPUT {
  float4 pos : SV_POSITION;
  float2 tex : TEXCOORD;
};


[maxvertexcount(4)]
void gs_main(point VS_OUTPUT input[1], inout TriangleStream<VS_OUTPUT> stream) {
  float half_width = SIZE.x / 2.0f;
  float3 normal = input[0].pos.xyz - CPOS;
  normal.y = 0.0f;
  float3 up_vec = float3(0.0f, SIZE.y, 0.0f);
  float3 right_vec = normalize(cross(normal, up_vec)) * half_width;

  float3 vert[4];
  vert[0] = input[0].pos.xyz  - right_vec;
  vert[1] = input[0].pos.xyz  + right_vec;
  vert[2] = input[0].pos.xyz  - right_vec - up_vec;
  vert[3] = input[0].pos.xyz  + right_vec - up_vec;

  float2 texcoord[4];
  texcoord[0] = float2(0, 0);
  texcoord[1] = float2(1, 0);
  texcoord[2] = float2(0, 1);
  texcoord[3] = float2(1, 1);

  for (int i = 0; i < 4; ++i) {
    VS_OUTPUT output;
    output.pos = mul(float4(vert[i], 1.0), WVP);
    output.tex = texcoord[i];
    stream.Append(output);
  }
}
