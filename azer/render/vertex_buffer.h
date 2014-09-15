#pragma once

#include <memory>
#include <string.h>

#include "base/basictypes.h"
#include "azer/render/vertex_data.h"
#include "azer/base/render_export.h"
#include "azer/render/render_system_enum.h"
#include "azer/render/hardware_buffer.h"

namespace azer {

class Renderer;
class AZER_EXPORT VertexBuffer : public HardwareBuffer {
 public:
  struct Options {
    char name[128];
    GraphicBuffer::Usage usage;
    CPUAccess cpu_access;  // defined render_system
    Options()
        : usage(GraphicBuffer::kDefault)
        , cpu_access(kCPUNoAccess) {
      memset(name, 0, sizeof(name));
    }
  };

  VertexBuffer(const Options &opt)
      : options_(opt)
      , element_size_(-1)
      , buffer_size_(-1)
      , vertex_num_(-1) {
  }

  virtual ~VertexBuffer() {}

  /**
   * 从 Lockable 继承的借口
   */
  virtual HardwareBufferDataPtr map(MapType flags) = 0;
  virtual void unmap() = 0;

  int32 vertex_num() const { return vertex_num_;}
  int32 buffer_size() const { return vertex_num_;}
  int32 element_size() const { return element_size_;}
 protected:
  const Options options_;
  int32 element_size_;
  int32 buffer_size_;
  int32 vertex_num_;
  DISALLOW_COPY_AND_ASSIGN(VertexBuffer);
};

typedef std::shared_ptr<VertexBuffer> VertexBufferPtr;
}  // namespace azer
