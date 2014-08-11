
#include "azer/math/math.h"
#include "d3d11.h"
#include <xnamath.h>

int main(int argc, char *argv[]) {
  using azer::Matrix4;
  using azer::PerspectiveRHD3D;
  float aspect = 4.0f / 3.0f;
  float radians = azer::kPI / 4.0f;
  Matrix4 proj = PerspectiveRHD3D(azer::Radians(radians), aspect, 1.0f, 1000.0f);

  XMMATRIX xna_proj = XMMatrixPerspectiveFovRH(radians, aspect, 1.0f, 1000.0f);
  XMFLOAT4X4A proj_arr;
  XMStoreFloat4x4A(&proj_arr, xna_proj);
  return 0;
}
