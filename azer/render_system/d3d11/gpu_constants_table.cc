#include "azer/render_system/d3d11/gpu_constants_table.h"

#include "azer/render_system/d3d11/render_system.h"
#include "azer/render_system/d3d11/renderer.h"
#include "azer/render_system/d3d11/util.h"

namespace azer {
bool D3D11GpuConstantsTable::Init(D3D11RenderSystem* rs) {
  D3D11_BUFFER_DESC cbbd;
  ID3D11Device* d3d_device = rs->GetDevice();

  ZeroMemory(&cbbd, sizeof(D3D11_BUFFER_DESC));
  cbbd.Usage = D3D11_USAGE_DEFAULT;
  cbbd.ByteWidth = size();
  cbbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
  cbbd.CPUAccessFlags = 0;
  cbbd.MiscFlags = 0;

  HRESULT hr = d3d_device->CreateBuffer(&cbbd, NULL, &buffer_);
  HRESULT_HANDLE(hr, ERROR, "CreateBuffer for Constants failed ");

  return true;
}

void D3D11GpuConstantsTable::flush(Renderer* renderer) {
  DCHECK(buffer_ != NULL);
  ID3D11DeviceContext* d3d_context = ((D3D11Renderer*)renderer)->GetContext();
  d3d_context->UpdateSubresource(buffer_, 0, NULL, data_.get(), 0, 0);
}

}  // namespace azer
