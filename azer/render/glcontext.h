#pragma once

#include "base/logging.h"
#include "azer/render/texture.h"

namespace azer {

struct AzerEGLContext {
  void* display;
  void* context;
  void* surface;
  void* user_data;
  int width;
  int height;
  TexturePtr tex;
  DataFormat colorbuf_format;
  DataFormat depthbuf_format;
  AzerEGLContext()
      : display(NULL)
      , context(NULL)
      , surface(NULL)
      , user_data(NULL)
      , width(0)
      , height(0)
      , colorbuf_format(kBGRAn8)
      , depthbuf_format(kDepth24Stencil8) {
  }
};

class AzerGLTexture {
 public:
  AzerGLTexture(int width, int height)
      : width_(width)
      , height_(height){
  }

  virtual ~AzerGLTexture() {}

  TexturePtr& GetTexture() { return texture_;}
  int64 glid() const { return glid_;}
  int32 width() const { return width_;}
  int32 height() const { return height_;}
 protected:
  int64 glid_;
  TexturePtr texture_;
  int32 width_;
  int32 height_;
  DISALLOW_COPY_AND_ASSIGN(AzerGLTexture);
};

typedef std::shared_ptr<AzerGLTexture> AzerGLTexturePtr;

class AzerEGLInterface {
 public:
  AzerEGLInterface() {}
  virtual ~AzerEGLInterface() {}
  virtual bool Init(AzerEGLContext* context) = 0;
  virtual bool MakeCurrent(const AzerEGLContext* context) = 0;
  virtual void Destroy(AzerEGLContext* context) = 0;
  virtual Texture* CreateTexture(uint32 gltexid)  = 0;
};
}  // namespace azer
