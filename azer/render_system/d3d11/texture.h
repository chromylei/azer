
#pragma once

#include <d3d11.h>

#include "base/basictypes.h"
#include "base/logging.h"
#include "azer/render/texture.h"
#include "azer/render_system/d3d11/util.h"

namespace azer {
class D3D11Renderer;
class D3D11RenderSystem;

class D3D11Texture: public Texture {
 public:
  D3D11Texture(const Texture::Options& opt, D3D11RenderSystem* rs);

  virtual ~D3D11Texture();
  bool Init(const D3D11_SUBRESOURCE_DATA* data, int num);
  virtual void GenerateMips(int level) OVERRIDE;
  virtual bool SetSamplerState(const SamplerState& sampler_state) OVERRIDE;

  virtual MapData map(MapType type) OVERRIDE;
  virtual void unmap() OVERRIDE;

  void UseForStage(RenderPipelineStage stage, int index, D3D11Renderer* renderer);
  void SetVSSampler(int index, D3D11Renderer* renderer);
  void SetPSSampler(int index, D3D11Renderer* renderer);

  bool Initialized() const { return NULL != resource_;}
  ID3D11Resource* GetResource() { return resource_; }
 protected:
  virtual void InitTextureDesc(D3D11_TEXTURE2D_DESC* desc) = 0;
  virtual void InitResourceDesc(D3D11_SHADER_RESOURCE_VIEW_DESC* desc) = 0;
  bool InitResourceView();

  ID3D11Resource* resource_;
  D3D11RenderSystem* render_system_;
  ID3D11ShaderResourceView* view_;
  ID3D11SamplerState* sampler_state_;
  D3D11_SHADER_RESOURCE_VIEW_DESC res_view_desc_;
  D3D11_TEXTURE2D_DESC tex_desc_;

#ifdef DEBUG
  bool mapped_;
#endif
  DISALLOW_COPY_AND_ASSIGN(D3D11Texture);
};

class D3D11Texture2D : public D3D11Texture {
 public:
  D3D11Texture2D(const Texture::Options& opt, D3D11RenderSystem* rs)
      : D3D11Texture(opt, rs) {
  }

  virtual bool InitFromImage(const Image* image) OVERRIDE;
 private:
  virtual void InitTextureDesc(D3D11_TEXTURE2D_DESC* desc) OVERRIDE;
  virtual void InitResourceDesc(D3D11_SHADER_RESOURCE_VIEW_DESC* desc) OVERRIDE;
  friend class D3D11RenderTarget;
  friend class D3D11DepthBuffer;
  DISALLOW_COPY_AND_ASSIGN(D3D11Texture2D);
};

class D3D11TextureCubeMap : public D3D11Texture {
 public:
  D3D11TextureCubeMap(const Texture::Options& opt, D3D11RenderSystem* rs)
      : D3D11Texture(opt, rs) {
  }

  virtual bool InitFromImage(const Image* image) OVERRIDE;
 private:
  virtual void InitTextureDesc(D3D11_TEXTURE2D_DESC* desc) OVERRIDE;
  virtual void InitResourceDesc(D3D11_SHADER_RESOURCE_VIEW_DESC* desc) OVERRIDE;
  DISALLOW_COPY_AND_ASSIGN(D3D11TextureCubeMap);
};

inline D3D11Texture::D3D11Texture(const Texture::Options& opt,
                                  D3D11RenderSystem* rs)
    : Texture(opt)
    , render_system_(rs)
    , view_(NULL)
    , resource_(NULL)
    , sampler_state_(NULL) {
#ifdef DEBUG
  mapped_ = false;
#endif
}

inline D3D11Texture::~D3D11Texture() {
  SAFE_RELEASE(resource_);
  SAFE_RELEASE(view_);
  SAFE_RELEASE(sampler_state_);
}

}  // namespace azer
