#include "azer/render_system/d3d11/gpu_program.h"
#include "azer/render_system/d3d11/render_system.h"
#include "azer/render_system/d3d11/enum_transform.h"
#include "azer/render_system/d3d11/util.h"

#include "azer/base/string.h"

namespace azer {

bool D3D11VertexGpuProgram::Init(RenderSystem* vrs) {
  D3D11RenderSystem* rs = (D3D11RenderSystem*)vrs;
  std::string msg;
  ID3D11Device* d3d_device = rs->GetDevice();

  // D3DBlobPtr blob(CompilePixelShader(program_, &msg));
  ID3DBlob* blob = CompileVertexShader(program_, &msg);
  if (NULL == blob) {
    LOG(ERROR) << "Failed to compile vertex shader: " << msg;
    return false;
  }

  DCHECK(NULL == shader_);
  HRESULT hr = d3d_device->CreateVertexShader(blob->GetBufferPointer(),
                                              blob->GetBufferSize(),
                                              NULL,
                                              &shader_);
  HRESULT_HANDLE(hr, ERROR, "CreateVertexShader failed ");

  const VertexDesc::Desc* desc = desc_ptr_->descs();
  std::unique_ptr<D3D11_INPUT_ELEMENT_DESC[]>
      layout_ptr(new D3D11_INPUT_ELEMENT_DESC[desc_ptr_->element_num()]);
  D3D11_INPUT_ELEMENT_DESC* curr_layout = layout_ptr.get();
  for (int i = 0; i < desc_ptr_->element_num(); ++i, ++curr_layout, ++desc) {
    curr_layout->SemanticName = desc->name;
    curr_layout->SemanticIndex = desc->semantic_index;
    curr_layout->Format = TranslateFormat(desc->type);
    curr_layout->InputSlot = 0;
    curr_layout->AlignedByteOffset = desc_ptr_->offset(i);
    curr_layout->InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    curr_layout->InstanceDataStepRate = 0;
  }

  hr = d3d_device->CreateInputLayout(layout_ptr.get(),
                                     desc_ptr_->element_num(),
                                     blob->GetBufferPointer(),
                                     blob->GetBufferSize(),
                                     &input_layout_);
  HRESULT_HANDLE(hr, ERROR, "CreateInputLayout failed");
  return true;
}

bool D3D11PixelGpuProgram::Init(RenderSystem* vrs) {
  D3D11RenderSystem* rs = (D3D11RenderSystem*)vrs;
  std::string msg;
  ID3D11Device* d3d_device = rs->GetDevice();
  D3DBlobPtr blob(CompilePixelShader(program_, &msg));
  if (NULL == blob) {
    LOG(ERROR) << "Failed to compiler pixel shader: " << msg;
    return false;
  }

  DCHECK(NULL == shader_);
  HRESULT hr = d3d_device->CreatePixelShader(blob->GetBufferPointer(),
                                             blob->GetBufferSize(),
                                             NULL,
                                             &shader_);
  HRESULT_HANDLE(hr, ERROR, "CreateVertexShader failed ");
  return true;
}

bool D3D11GeometryGpuProgram::Init(RenderSystem* vrs) {
  D3D11RenderSystem* rs = (D3D11RenderSystem*)vrs;
  DCHECK(NULL == shader_);
  std::string msg;
  ID3D11Device* d3d_device = rs->GetDevice();
  D3DBlobPtr blob(CompileGeometryShader(program_, &msg));
  if (NULL == blob) {
    LOG(ERROR) << "Failed to compiler shader: " << msg;
    return false;
  }

  HRESULT hr = d3d_device->CreateGeometryShader(blob->GetBufferPointer(),
                                                blob->GetBufferSize(),
                                                NULL,
                                                &shader_);
  HRESULT_HANDLE(hr, ERROR, "CreateVertexShader failed ");
  DCHECK(NULL != shader_);
  return true;
}
}  // namespace azer
