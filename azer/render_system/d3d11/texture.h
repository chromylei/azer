
#pragma once

#include <d3d11.h>

#include "base/basictypes.h"
#include "base/logging.h"
#include "azer/render/texture.h"
#include "azer/render_system/d3d11/util.h"

namespace azer {
class D3D11Renderer;
class D3D11RenderSystem;

class D3D11Texture2D : public Texture {
 public:
  virtual ~D3D11Texture2D() {
    SAFE_RELEASE(texture_);
    SAFE_RELEASE(view_);
    SAFE_RELEASE(sampler_state_);
  }

  bool Init(const D3D11_SUBRESOURCE_DATA* data);
  void UseForStage(RenderPipelineStage stage, int index, D3D11Renderer* renderer);

  virtual void GenerateMips(int level) OVERRIDE;
  virtual bool SetSamplerState(const SamplerState& sampler_state) OVERRIDE;

  virtual MapData map(MapType type) OVERRIDE;
  virtual void unmap() OVERRIDE;
  virtual bool InitFromData(const uint8* data, uint32 size) OVERRIDE;

  bool LoadFromFile(const base::FilePath& path);
  bool Initialized() const { return NULL != texture_;}

  void SetVSSampler(int index, D3D11Renderer* renderer);
  void SetPSSampler(int index, D3D11Renderer* renderer);
 private:
  D3D11Texture2D(const Texture::Options& opt, D3D11RenderSystem* rs)
      : Texture(opt)
      , render_system_(rs)
      , texture_(NULL)
      , view_(NULL)
      , sampler_state_(NULL) {
    DCHECK(opt.type == Texture::k2D
           || opt.type == Texture::kCubeMap);
#ifdef DEBUG
    mapped_ = false;
#endif
  }

  bool InitResourceView();
  void InitTexture2DDesc();
  void InitTextureCubeMapDesc();

  D3D11RenderSystem* render_system_;
  ID3D11Texture2D* texture_;
  ID3D11ShaderResourceView* view_;
  ID3D11SamplerState* sampler_state_;
  D3D11_TEXTURE2D_DESC tex_desc_;
  D3D11_SHADER_RESOURCE_VIEW_DESC res_view_desc_;
#ifdef DEBUG
  bool mapped_;
#endif

  friend class D3D11RenderSystem;
  friend class D3D11RenderTarget;
  friend class D3D11DepthBuffer;
  DISALLOW_COPY_AND_ASSIGN(D3D11Texture2D);
};

}  // namespace azer
