
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
  virtual bool Init(const D3D11_SUBRESOURCE_DATA* data, int num);
  virtual void GenerateMips(int level) OVERRIDE;
  virtual bool SetSamplerState(const SamplerState& sampler_state) OVERRIDE;

  virtual MapData map(MapType type) OVERRIDE;
  virtual void unmap() OVERRIDE;

  void UseForStage(RenderPipelineStage stage, int index, D3D11Renderer* renderer);
  void SetVSSampler(int index, D3D11Renderer* renderer);
  void SetPSSampler(int index, D3D11Renderer* renderer);

  bool Initialized() const { return NULL != resource_;}
  void Attach(ID3D11Texture2D* tex) { resource_ = tex;}
  void Detach() { resource_ = NULL; }

  ID3D11ShaderResourceView* GetResourceView() { return view_;}
  ID3D11Resource* GetResource() { return resource_;}
 protected:
  virtual void ModifyTextureDesc(D3D11_TEXTURE2D_DESC* desc) = 0;
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
 protected:
  virtual void ModifyTextureDesc(D3D11_TEXTURE2D_DESC* desc) OVERRIDE;
  virtual void InitResourceDesc(D3D11_SHADER_RESOURCE_VIEW_DESC* desc) OVERRIDE;
  friend class D3D11RenderTarget;
  friend class D3D11DepthBuffer;
  DISALLOW_COPY_AND_ASSIGN(D3D11Texture2D);
};

// for shared resource with other D3DDevice
// here, is use to create texture for ANGLE using
class D3D11Texture2DShared : public D3D11Texture2D {
 public:
  D3D11Texture2DShared(const Texture::Options& opt, D3D11RenderSystem* rs);
  virtual ~D3D11Texture2DShared();

  // create a resource for other device using
  virtual bool Init(const D3D11_SUBRESOURCE_DATA* data, int num);
  ID3D11Resource* GetSharedResource();
  HANDLE GetSharedHanle();
 protected:
  virtual void ModifyTextureDesc(D3D11_TEXTURE2D_DESC* desc) OVERRIDE;
  virtual bool InitFromImage(const Image* image) { return false;}

  bool InitSharedResource();
  HANDLE shared_handle_;
  ID3D11Resource* shared_resource_;
  DISALLOW_COPY_AND_ASSIGN(D3D11Texture2DShared);
};

class D3D11Texture2DExtern : public D3D11Texture2D {
 public:
  D3D11Texture2DExtern(const Texture::Options& opt, D3D11RenderSystem* rs)
      : D3D11Texture2D(opt, rs) {}

  // create from other device's resources
  static D3D11Texture2DExtern* Create(ID3D11Resource* resource,
                                      D3D11RenderSystem* rs);
 private:
  void Attach(ID3D11Texture2D* tex);
  virtual void ModifyTextureDesc(D3D11_TEXTURE2D_DESC* desc) OVERRIDE;
  virtual bool InitFromImage(const Image* image) { return false;}
  DISALLOW_COPY_AND_ASSIGN(D3D11Texture2DExtern);
};

class D3D11TextureCubeMap : public D3D11Texture {
 public:
  D3D11TextureCubeMap(const Texture::Options& opt, D3D11RenderSystem* rs)
      : D3D11Texture(opt, rs) {
    
  }
  virtual bool InitFromImage(const Image* image) OVERRIDE;
 protected:
  virtual void ModifyTextureDesc(D3D11_TEXTURE2D_DESC* desc) OVERRIDE;
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


inline ID3D11Resource* D3D11Texture2DShared::GetSharedResource() {
  DCHECK(shared_resource_ != NULL);
  return shared_resource_;
}

inline HANDLE D3D11Texture2DShared::GetSharedHanle() {
  DCHECK(shared_handle_ != NULL);
  return shared_handle_;
}
}  // namespace azer
