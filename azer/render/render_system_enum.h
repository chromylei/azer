#pragma once

#include "azer/base/render_export.h"
#include "base/basictypes.h"

namespace azer {

enum CullingMode {
  kCullNone = 1,     // none
  kCullFront,        //
  kCullBack,         // counter clock-wise
};

enum FillMode {
  kWireFrame = 2,
  kSolid,
};

enum FrontFace {
  kCounterClockwise = 1,
  kClockwise,
};

enum CPUAccess {
  kCPUNoAccess = 0x0,
  kCPUWrite    = 0x10000L,
  kCPURead     = 0x20000L,
  kCPUAccess   = kCPUWrite | kCPURead,
};

enum DataFormat {
  kScalar = 1,
  kFloat = kScalar,
  kVec2,
  kVec3,
  kVec4,
  kMat2,
  kMat3,
  kMat4,
  kInt,
  kIntVec2,
  kIntVec3,
  kIntVec4,
  kUint,
  kUintVec2,
  kUintVec3,
  kUintVec4,
  kBool,
  kBoolVec2,
  kBoolVec3,
  kBoolVec4,
  kRGBA8,             // all element are uint8
  kRGBAn8,            // all element are uint8
  kRGBA32,            // all element are uint32
  kRGBAn32,           // all element are uint32
  kRGBAf,             // all element are float
  kBGRA8,
  kBGRAn8,
  kBGRA32,
  kBGRAn32,
  kBGRAnf,
  kDepth24Stencil8,
  kRGBA = kRGBAf,
};

enum PrimitiveTopology {
  kPointList = 1,
  kLineStrip,
  kLineList,
  kTriangleStrip,
  kTriangleList,
};

enum RenderPipelineStage {
  kStageNotSpec = -1,
  kInputAssemblerStage = 1,
  kVertexStage,
  kHullStage,
  kTessellatorStage,
  kDomainStage,
  kGeometryStage,
  kRasterizerStage,
  kPixelStage,
  kOutputMergerStage,
  kRenderPipelineStageNum,
};

/**
 *
 */
class GraphicBuffer {
 public:
  enum Usage {
    kDefault,      //  require read and write acceed by th GPU
    kGPUReayOnly,  // can only read by GPU, 
    kDynamic,      // GPU(read only), CPU(write only)
    kStaging,      // supports transform resource from GPU to CPU
  };
};

class CompareFunc {
 public:
  enum Type {
    kNever = 1,
    kLess,
    kEqual,
    kLessEqual,
    kGreater,
    kNotEqueal,
    kGreaterEqua,
    kAlways,
  };
};

enum MapType {
  kReadOnly,
  kWrite,
  kReadWrite,
  kWriteDiscard,
  kWriteNoOverwrite,
};

enum VisibleState {
  kNoneVisible    = -1,
  kFullyVisible   = 0,
  kPartialVisible = 1,
  
};

AZER_EXPORT const char* RenderStageName(RenderPipelineStage stage);
AZER_EXPORT uint32 SizeofDataFormat(DataFormat format);
}  // namespace azer
