#include "azer/render_system/d3d11/util.h"

#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>
#include <d3dcompiler.h>
#include <xnamath.h>

namespace azer {
#if defined(DEBUG)
const DWORD kCompileFlags =
    D3DCOMPILE_DEBUG
    | D3DCOMPILE_ENABLE_STRICTNESS
    | D3DCOMPILE_OPTIMIZATION_LEVEL0
    | D3DCOMPILE_PREFER_FLOW_CONTROL
    | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
const DWORD kCompileFlags =
    D3DCOMPILE_ENABLE_STRICTNESS
    | D3DCOMPILE_SKIP_VALIDATION
    ;
#endif

inline ID3DBlob* CompileHLSL(const std::string& shader, const std::string& target,
                             const std::string& entrypointer,
                             std::string* error_msg) {
  ID3DBlob* blob = NULL;
  ID3DBlob* msgblob = NULL;
  HRESULT hr;
  hr = D3DCompile(shader.c_str(),
                  shader.length(),
                  NULL,
                  NULL,
                  NULL,                 // pIncludes
                  entrypointer.c_str(), // pEntryPointer
                  target.c_str(),
                  kCompileFlags,
                  0,
                  &blob,
                  &msgblob);

  if (FAILED(hr)) {
    error_msg->append((const char*)msgblob->GetBufferPointer(),
                      msgblob->GetBufferSize());
    return NULL;
  }
  return blob;
}

ID3DBlob* CompileVertexShader(const std::string& shader, std::string* error_msg) {
  return CompileHLSL(shader, "vs_5_0", "vs_main", error_msg);
}
ID3DBlob* CompilePixelShader(const std::string& shader, std::string* error_msg) {
  return CompileHLSL(shader, "ps_5_0", "ps_main", error_msg);
}
ID3DBlob* CompileGeometryShader(const std::string& shader, std::string* error_msg) {
  return CompileHLSL(shader, "gs_5_0", "gs_main", error_msg);
}
}  // namespace azer
