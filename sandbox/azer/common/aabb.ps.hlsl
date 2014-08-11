struct VS_OUTPUT {
  float4 Pos : SV_POSITION;
};

float4 ps_main(VS_OUTPUT input) : SV_TARGET {
  return float4(0.57f, 0.5f, 0.89f, 0.4f);
}

