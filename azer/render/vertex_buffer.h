#pragma once

#include <memory>
#include <string.h>

#include "base/basictypes.h"
#include "azer/render/vertex_data.h"
#include "azer/base/render_export.h"
#include "azer/render/render_system_enum.h"
#include "azer/render/lockable.h"

namespace azer {

class Renderer;
class AZER_EXPORT VertexBuffer : public Lockable {
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

  VertexBuffer(const Options &opt, VertexDataPtr dataptr)
      : options_(opt)
      , data_ptr_(dataptr){}

  virtual ~VertexBuffer() {}

  /**
   * 从 Lockable 继承的借口
   */
  virtual LockDataPtr map(MapType flags) = 0;
  virtual void unmap() = 0;

  const VertexDescPtr& desc() const { return data_ptr_->desc();}
  const VertexDataPtr& data() const { return data_ptr_;}
 protected:
  VertexDataPtr data_ptr_;
  const Options options_;
  DISALLOW_COPY_AND_ASSIGN(VertexBuffer);
};

typedef std::shared_ptr<VertexBuffer> VertexBufferPtr;
}  // namespace azer
