#include "azer/render_system/d3d11/blending.h"

#include <d3d11.h>
#include "azer/render_system/d3d11/hr_msg.h"
#include "azer/render_system/d3d11/enum_transform.h"
#include "azer/render_system/d3d11/render_system.h"
#include "azer/render_system/d3d11/renderer.h"

namespace azer {

bool D3D11Blending::Init() {
  D3D11_BLEND_DESC blend_desc;
  ZeroMemory(&blend_desc, sizeof(blend_desc));

  D3D11_RENDER_TARGET_BLEND_DESC rtbd;
  ZeroMemory(&rtbd, sizeof(rtbd));

  rtbd.BlendEnable = true;
  rtbd.SrcBlend = TranslateBlending(desc_.src);
  rtbd.DestBlend = TranslateBlending(desc_.dest);
  rtbd.BlendOp = TranslateBlendingOper(desc_.oper);
  rtbd.SrcBlendAlpha = TranslateBlending(desc_.src_alpha);
  rtbd.DestBlendAlpha = TranslateBlending(desc_.dest_alpha);
  rtbd.BlendOpAlpha = TranslateBlendingOper(desc_.alpha_oper);
  rtbd.RenderTargetWriteMask = TranslateWriteMask(desc_.mask);

  blend_desc.AlphaToCoverageEnable = false;
  blend_desc.RenderTarget[0] = rtbd;
  
  ID3D11Device* d3d_device = render_system_->GetDevice();
  DCHECK(NULL != d3d_device);
  ID3D11BlendState* state = NULL;
  HRESULT hr = d3d_device->CreateBlendState(&blend_desc, &blending_state_);
  HRESULT_HANDLE(hr, ERROR, "CreateBlendState failed ");
  
  return true;
}
}  // namespace azer
