#include "azer/render_system/d3d11/technique.h"
#include "azer/render_system/d3d11/gpu_program.h"
#include "azer/render_system/d3d11/render_system.h"
#include "azer/render_system/d3d11/renderer.h"
#include "azer/render_system/d3d11/util.h"

namespace azer {
void D3D11Technique::Use(Renderer* renderer) {
  D3D11VertexGpuProgram* vs = (D3D11VertexGpuProgram*)pline_[kVertexStage].get();
  D3D11PixelGpuProgram* ps = (D3D11PixelGpuProgram*)pline_[kPixelStage].get();
  D3D11GeometryGpuProgram* gs = (D3D11GeometryGpuProgram*)pline_[kGeometryStage].get();
  DCHECK(vs != NULL);
  DCHECK(ps != NULL);
  DCHECK_EQ(vs->stage(), kVertexStage);
  DCHECK_EQ(ps->stage(), kPixelStage);

  ID3D11DeviceContext* d3d_context = ((D3D11Renderer*)renderer)->GetContext();

  DCHECK(vs->input_layout_ != NULL);
  d3d_context->IASetInputLayout(vs->input_layout_);

  d3d_context->VSSetShader(vs->shader_, 0, 0);
  d3d_context->PSSetShader(ps->shader_, 0, 0);

  if (gs) {
    DCHECK(gs->shader_ != NULL);
    DCHECK_EQ(gs->stage(), kGeometryStage);
    d3d_context->GSSetShader(gs->shader_, 0, 0);
  } else {
    d3d_context->GSSetShader(NULL, 0, 0);
  }
  // TODO(yanglei)
}
}  // namespace azer
