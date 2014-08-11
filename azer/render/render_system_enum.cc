#include "azer/render/render_system_enum.h" 

#include "base/logging.h"
#include "base/basictypes.h"

namespace azer {
const char* RenderStageName(RenderPipelineStage stage) {
  switch (stage) {
    case kInputAssemblerStage: return "InputAssemblerStage";
    case kVertexStage: return "VertexStage";
    case kHullStage: return "HullStage";
    case kTessellatorStage: return "TessellatorStage";
    case kDomainStage: return "DomainStage";
    case kGeometryStage: return "GeometryStage";
    case kRasterizerStage: return "RasterizerStage";
    case kPixelStage: return "PixelStage";
    case kOutputMergerStage: return "OutputMergerStage";
    case kRenderPipelineStageNum: return "RenderPipelineStageNum";
    default: NOTREACHED(); return "";
  }
}

uint32 SizeofDataFormat(DataFormat format) {
  switch (format) {
    case kFloat: return sizeof(float);
    case kVec2: return sizeof(float) * 4;
    case kVec3: return sizeof(float) * 9;
    case kVec4: return sizeof(float) * 16;
    case kMat2: return sizeof(float) * 4;
    case kMat3: return sizeof(float) * 9;
    case kMat4: return sizeof(float) * 16;
    case kInt: return sizeof(int32);
    case kIntVec2: return sizeof(int32) * 2;
    case kIntVec3: return sizeof(int32) * 3;
    case kIntVec4: return sizeof(int32) * 4;
    case kUint: return sizeof(uint32);
    case kUintVec2: return sizeof(uint32) * 2;
    case kUintVec3: return sizeof(uint32) * 3;
    case kUintVec4: return sizeof(uint32) * 4;
    case kBool: return sizeof(int32);
    case kBoolVec2: return sizeof(int32) * 2;
    case kBoolVec3: return sizeof(int32) * 3;
    case kBoolVec4: return sizeof(int32) * 4;
    case kRGBA8: return sizeof(int8) * 4;
    case kRGBAn8: return sizeof(uint8) * 4;
    case kRGBA32: return sizeof(int32) * 4;
    case kRGBAn32: return sizeof(uint32) * 4;
    case kRGBAf: return sizeof(float) * 4;
    case kDepth24Stencil8: return sizeof(uint32);
      // case kRGBA: return sizeof(float) * 4;
    default:
      NOTREACHED();
      return 0;
  }
};
}  // namespace azer
