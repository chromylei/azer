#pragma once


namespace azer {
struct AzerGLContext {
  void* display;
  void* context;
  void* surface;
  void* user_data;
  int width;
  int height;
  TexturePtr tex;
  DataFormat colorbuf_format;
  DataFormat depthbuf_format;
  AzerGLContext()
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
}  // namespace azer
