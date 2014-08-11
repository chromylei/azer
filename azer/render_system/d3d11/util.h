#pragma once

#include <string>
#include <memory>

#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>

#include "base/basictypes.h"
#include "azer/render_system/d3d11/hr_msg.h"
#include "azer/base/string.h"

#define SAFE_RELEASE(res) \
  if ((res)) {            \
    (res)->Release();     \
    (res) = NULL;         \
  }

#define HRESULT_HANDLE(HR, LEVEL, MSG)                                  \
  if (FAILED(HR)) {                                                     \
    LOG(##LEVEL) << ##MSG << "(" << ::base::HRMessage(HR) << ")";       \
        return false;                                                   \
  }

#define HRESULT_HANDLE_NORET(HR, LEVEL, MSG)                            \
  if (FAILED(HR)) {                                                     \
    LOG(##LEVEL) << ##MSG << "(" << ::base::HRMessage(HR) << ")";       \
  }


namespace azer {
ID3DBlob* CompileVertexShader(const std::string& shader, std::string* error_msg);
ID3DBlob* CompilePixelShader(const std::string& shader, std::string* error_msg);
ID3DBlob* CompileGeometryShader(const std::string& shader, std::string* error_msg);

struct COMDeleter {
  template<typename T> void operator()(T* ptr) {
    ptr->Release();
  }
};

typedef std::unique_ptr<ID3DBlob, COMDeleter> D3DBlobPtr;
typedef std::unique_ptr<IUnknown, COMDeleter> D3DObjPtr;

inline uint32 Align16BindWidth(uint32 size) {
  if (size & 0x000F) {
    size &= 0xFFF0;
    size += 0x0010;
  }
  return std::min(size, (uint32)D3D11_REQ_CONSTANT_BUFFER_ELEMENT_COUNT);
}

class ScopedRefCOM {
 public:
  ScopedRefCOM(IUnknown* ptr)
      : ptr_(ptr){
    DCHECK(ptr_ != NULL);
  }

  ~ScopedRefCOM() {
    DCHECK(ptr_ != NULL);
    ptr_->Release();
  }
 private:
  IUnknown* ptr_;
  DISALLOW_COPY_AND_ASSIGN(ScopedRefCOM);
};

}  // namespace azer
