// reference: 
// 1. Introduction To Textures in Direct3D 11[http://msdn.microsoft.com/en-us/library/windows/desktop/ff476906(v=vs.85).aspx]
// 2. Creating Texture Resources (Direct3D 10) [http://msdn.microsoft.com/en-us/library/windows/desktop/bb205131(v=vs.85).aspx]
// 3. Coordinate Systems (Direct3D 10) [http://msdn.microsoft.com/en-us/library/windows/desktop/cc308049(v=vs.85).aspx]
// 4. How to: Initialize a Texture From a File [http://msdn.microsoft.com/en-us/library/windows/desktop/ff476904(v=vs.85).aspx]

#include "azer/render_system/d3d11/texture.h"

#include <d3d11.h>
#include <d3dx11.h>

#include "base/logging.h"
#include "base/strings/string16.h"
#include "azer/base/image.h"
#include "azer/render_system/d3d11/enum_transform.h"
#include "azer/render_system/d3d11/render_system.h"
#include "azer/render_system/d3d11/renderer.h"
#include "azer/render_system/d3d11/util.h"

namespace azer {
bool D3D11Texture::Init(const D3D11_SUBRESOURCE_DATA* data, int num) {
  HRESULT hr;
  DCHECK(NULL == resource_);
  ID3D11Device* d3d_device = render_system_->GetDevice();

  ZeroMemory(&tex_desc_, sizeof(D3D11_TEXTURE2D_DESC));
  tex_desc_.Width     = options_.width;
  tex_desc_.Height    = options_.height;
  tex_desc_.MipLevels = options_.sampler.mip_level;
  tex_desc_.ArraySize = num;
  tex_desc_.Format    = TranslateFormat(options_.format);
  tex_desc_.SampleDesc.Count   = options_.sampler.sample_level;
  tex_desc_.SampleDesc.Quality = options_.sampler.sample_qualifiy;
  tex_desc_.Usage          = TranslateUsage(options_.usage);
  tex_desc_.BindFlags      = TranslateBindTarget(options_.target);
  tex_desc_.CPUAccessFlags = TranslateCPUAccess(options_.cpu_access);
  tex_desc_.MiscFlags      = 0;
  InitTextureDesc(&tex_desc_);

  ID3D11Texture2D* tex = NULL;
  hr = d3d_device->CreateTexture2D(&tex_desc_, data, &tex);
  HRESULT_HANDLE(hr, ERROR, "CreateTexture2D failed ");

  resource_ = tex;
  if (options_.target & Texture::kShaderResource) {
    return InitResourceView();
  } else {
    return true;
  }
}

void D3D11Texture::SetPSSampler(int index, D3D11Renderer* renderer) {
  DCHECK(NULL != view_);
  DCHECK_GE(index, 0);
  ID3D11DeviceContext* d3d_context = renderer->GetContext();
  d3d_context->PSSetShaderResources(index, 1, &view_);
  if (sampler_state_) {
    d3d_context->PSSetSamplers(index, 1, &sampler_state_);
  }
}

void D3D11Texture::SetVSSampler(int index, D3D11Renderer* renderer) {
  DCHECK(NULL != view_);
  DCHECK_GE(index, 0);
  ID3D11DeviceContext* d3d_context = renderer->GetContext();
  d3d_context->VSSetShaderResources(index, 1, &view_);
  if (sampler_state_) {
    d3d_context->VSSetSamplers(index, 1, &sampler_state_);
  }
}

void D3D11Texture::GenerateMips(int level) {
  DCHECK(view_ != NULL);
  ID3D11Device* d3d_device = render_system_->GetDevice();
  D3D11Renderer* renderer = (D3D11Renderer*)(render_system_->GetDefaultRenderer());
  ID3D11DeviceContext* d3d_context = renderer->GetContext();
  // D3DX11FilterTexture(d3d_context, (ID3D11Resource*)view_, 0, level);
  d3d_context->GenerateMips(view_);
}

bool D3D11Texture::InitResourceView() {
  ID3D11Device* d3d_device = render_system_->GetDevice();
  InitResourceDesc(&res_view_desc_);
  HRESULT hr = d3d_device->CreateShaderResourceView(resource_, &res_view_desc_,
                                                    &view_);
  HRESULT_HANDLE(hr, ERROR, "CreateResourceView failed for texture");
  return SetSamplerState(options_.sampler);
}

bool D3D11Texture::SetSamplerState(const SamplerState& sampler_state) {
  ID3D11Device* d3d_device = render_system_->GetDevice();
  if (sampler_state_) {
    SAFE_RELEASE(sampler_state_);
  }
  D3D11_SAMPLER_DESC sampler_desc;
  ZeroMemory(&sampler_desc, sizeof(sampler_desc));
  sampler_desc.Filter = TranslateSamplerState(sampler_state);
  sampler_desc.AddressU = TranslateTexWrapMode(options_.sampler.wrap_u);
  sampler_desc.AddressV = TranslateTexWrapMode(options_.sampler.wrap_v);
  sampler_desc.AddressW = TranslateTexWrapMode(options_.sampler.wrap_w);
  sampler_desc.ComparisonFunc = TranslateCompareFunc(sampler_state.compare_func);
  sampler_desc.MaxAnisotropy = options_.sampler.max_anisotropy;
  sampler_desc.MipLODBias = 0.0f;
  sampler_desc.MinLOD = 0;
  sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;
  sampler_desc.BorderColor[0] = sampler_state.border_color.x;
  sampler_desc.BorderColor[1] = sampler_state.border_color.y;
  sampler_desc.BorderColor[2] = sampler_state.border_color.z;
  sampler_desc.BorderColor[3] = sampler_state.border_color.w;
  HRESULT hr = d3d_device->CreateSamplerState(&sampler_desc, &sampler_state_);
  HRESULT_HANDLE(hr, ERROR, "CreateSamplerState failed ");

  D3D11_SAMPLER_DESC sampler_desc2;
  sampler_state_->GetDesc(&sampler_desc2);
  return true;
}

void D3D11Texture::UseForStage(RenderPipelineStage stage, int index,
                               D3D11Renderer* renderer) {
  DCHECK(NULL != render_system_);
  if (stage == kVertexStage) {
    SetVSSampler(index, renderer);
  } else if (stage == kPixelStage) {
    SetPSSampler(index, renderer);
  } else {
    CHECK(false);
  }
}

// reference: MSDN "How to: Use dynamic resources"
Texture::MapData D3D11Texture::map(MapType maptype) {
  DCHECK(NULL != resource_);
  MapData mapdata;
  ZeroMemory(&mapdata, sizeof(mapdata));
  D3D11Renderer* renderer = (D3D11Renderer*)(render_system_->GetDefaultRenderer());
  ID3D11DeviceContext* d3d_context = renderer->GetContext();

  D3D11_MAPPED_SUBRESOURCE mapped;
  ZeroMemory(&mapped, sizeof(D3D11_MAPPED_SUBRESOURCE));
  HRESULT hr = d3d_context->Map(resource_, 0, TranslateMapType(maptype), 0, &mapped);
  if (FAILED(hr)) {
    LOG(ERROR) << "Map Texture failed.";
    return mapdata;
  }
  
  mapdata.pdata = (uint8*)mapped.pData;
  mapdata.row_pitch = mapped.RowPitch;
  mapdata.depth_pitch = mapped.DepthPitch;

#ifdef DEBUG
  DCHECK(!mapped_);
  mapped_ = true;
#endif
  return mapdata;
}

void D3D11Texture::unmap() {
#ifdef DEBUG
  DCHECK(mapped_);
  mapped_ = false;
#endif
  DCHECK(NULL != resource_);
  D3D11Renderer* renderer = (D3D11Renderer*)(render_system_->GetDefaultRenderer());
  ID3D11DeviceContext* d3d_context = renderer->GetContext();
  d3d_context->Unmap(resource_, 0);
}

// class D3D11Texture2D
void D3D11Texture2D::InitTextureDesc(D3D11_TEXTURE2D_DESC* desc) {
}

bool D3D11Texture2D::InitFromImage(const Image* image) {
  // [reference] MSDN: How to: Initialize a Texture Programmatically
  const ImageDataPtr& data = image->data(0);
  uint32 expect_size = SizeofDataFormat(options_.format)
      * options_.width * options_.height;
  if (data->data_size() != expect_size) {
    LOG(ERROR) << "unexpected size: " << data->data_size()
               << " expected: " << expect_size;
    return false;
  }
  
  
  D3D11_SUBRESOURCE_DATA subres;
  subres.pSysMem = data->data();
  subres.SysMemPitch = options_.width * SizeofDataFormat(options_.format);
  subres.SysMemSlicePitch = 0;  // no meaning for 2D
  return Init(&subres, 1);
}

void D3D11Texture2D::InitResourceDesc(D3D11_SHADER_RESOURCE_VIEW_DESC* desc) {
  DCHECK(resource_ != NULL);
  DCHECK_EQ(GetViewDimensionFromTextureType(options_.type),
            D3D11_SRV_DIMENSION_TEXTURE2D);
  desc->Format = tex_desc_.Format;
  desc->ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
  desc->Texture2D.MipLevels = tex_desc_.MipLevels;
  desc->Texture2D.MostDetailedMip = 0;
}

void D3D11TextureCubeMap::InitTextureDesc(D3D11_TEXTURE2D_DESC* desc) {
  desc->MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
} 

// class D3D11TextureCubeMap
bool D3D11TextureCubeMap::InitFromImage(const Image* image) {
  // [reference] MSDN: How to: Initialize a Texture Programmatically
  const ImageDataPtr& data = image->data(0);
  uint32 expect_size = SizeofDataFormat(options_.format)
      * options_.width * options_.height;
  if (data->data_size() != expect_size) {
    LOG(ERROR) << "unexpected size: " << data->data_size()
               << " expected: " << expect_size;
    return false;
  }
  
  
  D3D11_SUBRESOURCE_DATA subres[6];
  for (int i = 0; i < 6; ++i) {
    const ImageDataPtr& data = image->data(i);

    subres[i].pSysMem = data->data();
    subres[i].SysMemPitch = options_.width * SizeofDataFormat(options_.format);
    subres[i].SysMemSlicePitch = 0;  // no meaning for 2D
  }
  return Init(subres, 6);
}

void D3D11TextureCubeMap::InitResourceDesc(D3D11_SHADER_RESOURCE_VIEW_DESC* desc) {
  DCHECK(resource_ != NULL);
  DCHECK_EQ(GetViewDimensionFromTextureType(options_.type),
            D3D11_SRV_DIMENSION_TEXTURECUBE);
  res_view_desc_.Format = tex_desc_.Format;
  res_view_desc_.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
  res_view_desc_.TextureCube.MipLevels = tex_desc_.MipLevels;
  res_view_desc_.TextureCube.MostDetailedMip = 0;
}

}  // namespace azer
