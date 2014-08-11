#include "azer/render_system/d3d11/indices_buffer.h"

#include <d3dx11.h>

#include "azer/render/indices_buffer.h"
#include "azer/render_system/d3d11/util.h"
#include "azer/render_system/d3d11/enum_transform.h"
#include "azer/render_system/d3d11/render_system.h"

namespace azer {
bool D3D11IndicesBuffer::Init() {
  DCHECK(NULL == indices_buffer_);
  DCHECK(indices_data_.get() != NULL);

  ID3D11Device* d3d_device = render_system_->GetDevice();

  D3D11_BUFFER_DESC indices_buffer_desc;
  ZeroMemory(&indices_buffer_desc, sizeof(indices_buffer_desc));
  indices_buffer_desc.Usage = TranslateUsage(options_.usage);
  indices_buffer_desc.ByteWidth = indices_data_->size();
  indices_buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
  indices_buffer_desc.CPUAccessFlags = TranslateCPUAccess(options_.cpu_access);;
  indices_buffer_desc.MiscFlags = 0;

  D3D11_SUBRESOURCE_DATA d3d_vdata;
  ZeroMemory(&d3d_vdata, sizeof(d3d_vdata));
  d3d_vdata.pSysMem = indices_data_->pointer();
  HRESULT hr;
  hr = d3d_device->CreateBuffer(&indices_buffer_desc, &d3d_vdata, &indices_buffer_);
  HRESULT_HANDLE(hr, ERROR, "D3D11: CreateIndexBuffer failed ");

  return true;
}
}  // namespace azer
