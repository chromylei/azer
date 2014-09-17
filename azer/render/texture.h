#pragma once

#include "base/files/file_path.h"
#include "azer/math/math.h"
#include "azer/base/render_export.h"
#include "azer/render/render_system_enum.h"

namespace azer {

class RenderSystem;

class Texture {
 public:
  enum BindTarget {
    kUnknown        = 0,
    kShaderResource = 0x8L,
    kRenderTarget   = 0x20L,
    kDepthStencil   = 0x40L,
  };

  enum WrapMode {
    kWrap = 1,
    kMirror,
    kClamp,
    kBorder,
    kMirrorOnce,
  };

  enum FilterMode {
    kPoint = 1,
    kLinear,
  };

  enum Type {
    k1D,
    k1DArray,
    k2D,
    k2DArray,
    k2DMultiSample,
    k2DArrayMultiSample,
    k3D,
    kCubeMap,
    kCubeMapArray,
  };

  struct SamplerState {
    Texture::WrapMode wrap_u;
    Texture::WrapMode wrap_v;
    Texture::WrapMode wrap_w;
    Texture::FilterMode mag_filter;
    Texture::FilterMode min_filter;
    Texture::FilterMode mipmin_filter;
    Texture::FilterMode mipmag_filter;
    CompareFunc::Type compare_func;
    Vector4 border_color;
    int mip_level;
    int max_anisotropy;

    SamplerState()
        : wrap_u(Texture::kWrap)
        , wrap_v(Texture::kWrap)
        , wrap_w(Texture::kWrap)
        , mag_filter(Texture::kPoint)
        , min_filter(Texture::kPoint)
        , mipmin_filter(Texture::kPoint)
        , mipmag_filter(Texture::kPoint)
        , compare_func(CompareFunc::kNever)
        , border_color(0.0f, 0.0f, 0.0f, 0.0f)
        , mip_level(1)
        , max_anisotropy(1) {
    }
  };

  struct Options {
    int width;
    int height;
    SamplerState sampler;
    DataFormat format;
    GraphicBuffer::Usage usage;
    CPUAccess cpu_access;
    Texture::BindTarget target;
    Type type;

    Options()
        : width(0)
        , height(0)
        , format(kRGBAn8)
        , usage(GraphicBuffer::kDefault)
        , cpu_access(kCPUNoAccess)
        , target(Texture::kUnknown)
        , type(k2D) {
    }
  };

  explicit Texture(const Options& opt) : options_(opt) {}
  virtual ~Texture() {}
  
  static Texture* CreateShaderTexture(const ::base::FilePath& path,
                                      RenderSystem* rs);
  static Texture* CreateShaderTexture(const ::base::FilePath::StringType& path,
                                      RenderSystem* rs);

  const Options& option() const { return options_;}

  /**
   * NOTE: 此函数的设置将在下一次调用 UseForStage 或者 RenderSystem::Use 时生效
   */ 
  void SetSamplerState(const SamplerState& sampler_state) {
    options_.sampler = sampler_state;
  }

  /**
   * Note: map 返回 MapData 结构体，返回的数据当中 width 和 depth
   * 不一定和 option 当中制定的大小是一致的
   * 导致这个问题的原因是，有可能 GPU 采用更大的对其方式
   * 因此操作内存时，应以 MapData 给定的数据为准，不要使用 Options 的数据
   */
  struct MapData {
    uint8* pdata;
    uint32 row_pitch;
    uint32 depth_pitch;
  };
  virtual MapData map(MapType maptype) = 0;
  virtual void unmap() = 0;
  virtual bool InitFromData(const uint8* data, uint32 size) = 0;
 protected:
  Options options_;
  DISALLOW_COPY_AND_ASSIGN(Texture);
};

typedef std::shared_ptr<Texture> TexturePtr;

inline Texture* Texture::CreateShaderTexture(
    const ::base::FilePath::StringType& path, RenderSystem* rs) {
  return CreateShaderTexture(::base::FilePath(path), rs);
}
}  // namespace azer
