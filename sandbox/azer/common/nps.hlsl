struct VS_OUTPUT {
  float4 Pos : SV_POSITION;
};

float4 ps_main(VS_OUTPUT input) : SV_TARGET {
  return float4(1.0, 0.0, 0.0, 1.0);
}

