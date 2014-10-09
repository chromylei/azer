#include "azer/angle/renderer.h"

#include "azer/render_system/d3d11/render_system.h"
#include "azer/render_system/d3d11/renderer.h"
#include "base/logging.h"

namespace azer {

namespace {
static const DXGI_FORMAT RenderTargetFormats[] =
{
  DXGI_FORMAT_B8G8R8A8_UNORM,
  DXGI_FORMAT_R8G8B8A8_UNORM
};

static const DXGI_FORMAT DepthStencilFormats[] =
{
  DXGI_FORMAT_UNKNOWN,
  DXGI_FORMAT_D24_UNORM_S8_UINT,
  DXGI_FORMAT_D16_UNORM
};
}

AzerRenderer::AzerRenderer(egl::Display *display, HDC hDc, D3D11RenderSystem* rs) 
    : rx::Renderer11(display, hDc) {
  mDevice = rs->GetDevice();
  mDeviceContext = rs->GetContext();
  mFeatureLevel = rs->feature_level();
}

EGLint AzerRenderer::initialize() {
  HRESULT result = S_OK;
  if (!initializeCompiler()) {
    return EGL_NOT_INITIALIZED;
  }

  mDxgiModule = LoadLibrary(TEXT("dxgi.dll"));
  mD3d11Module = LoadLibrary(TEXT("d3d11.dll"));

  if (mD3d11Module == NULL || mDxgiModule == NULL) {
    LOG(ERROR) << "Could not load D3D11 or DXGI library - aborting!\n";
    return EGL_NOT_INITIALIZED;
  }

  #if !ANGLE_SKIP_DXGI_1_2_CHECK
  // In order to create a swap chain for an HWND owned by another process, DXGI 1.2 is required.
  // The easiest way to check is to query for a IDXGIDevice2.
  bool requireDXGI1_2 = false;
  HWND hwnd = WindowFromDC(mDc);
  if (hwnd)
  {
    DWORD currentProcessId = GetCurrentProcessId();
    DWORD wndProcessId;
    GetWindowThreadProcessId(hwnd, &wndProcessId);
    requireDXGI1_2 = (currentProcessId != wndProcessId);
  }
  else
  {
    requireDXGI1_2 = true;
  }

  if (requireDXGI1_2)
  {
    IDXGIDevice2 *dxgiDevice2 = NULL;
    result = mDevice->QueryInterface(__uuidof(IDXGIDevice2), (void**)&dxgiDevice2);
    if (FAILED(result))
    {
      ERR("DXGI 1.2 required to present to HWNDs owned by another process.\n");
      return EGL_NOT_INITIALIZED;
    }
    SafeRelease(dxgiDevice2);
  }
#endif

  IDXGIDevice *dxgiDevice = NULL;
  result = mDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice);

  if (FAILED(result))
  {
    ERR("Could not query DXGI device - aborting!\n");
    return EGL_NOT_INITIALIZED;
  }

  result = dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&mDxgiAdapter);

  if (FAILED(result))
  {
    ERR("Could not retrieve DXGI adapter - aborting!\n");
    return EGL_NOT_INITIALIZED;
  }

  dxgiDevice->Release();

  mDxgiAdapter->GetDesc(&mAdapterDescription);
  memset(mDescription, 0, sizeof(mDescription));
  wcstombs(mDescription, mAdapterDescription.Description, sizeof(mDescription) - 1);

  result = mDxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&mDxgiFactory);

  if (!mDxgiFactory || FAILED(result))
  {
    ERR("Could not create DXGI factory - aborting!\n");
    return EGL_NOT_INITIALIZED;
  }

  // Disable some spurious D3D11 debug warnings to prevent them from flooding the output log
#if defined(ANGLE_SUPPRESS_D3D11_HAZARD_WARNINGS) && defined(_DEBUG)
  ID3D11InfoQueue *infoQueue;
  result = mDevice->QueryInterface(__uuidof(ID3D11InfoQueue),  (void **)&infoQueue);

  if (SUCCEEDED(result))
  {
        D3D11_MESSAGE_ID hideMessages[] =
        {
            D3D11_MESSAGE_ID_DEVICE_DRAW_RENDERTARGETVIEW_NOT_SET
        };

        D3D11_INFO_QUEUE_FILTER filter = {0};
        filter.DenyList.NumIDs = ArraySize(hideMessages);
        filter.DenyList.pIDList = hideMessages;

        infoQueue->AddStorageFilterEntries(&filter);

        infoQueue->Release();
  }
#endif

  unsigned int maxSupportedSamples = 0;
  unsigned int rtFormatCount = ArraySize(RenderTargetFormats);
  unsigned int dsFormatCount = ArraySize(DepthStencilFormats);
  for (unsigned int i = 0; i < rtFormatCount + dsFormatCount; ++i)
  {
    DXGI_FORMAT format = (i < rtFormatCount) ? RenderTargetFormats[i] : DepthStencilFormats[i - rtFormatCount];
    if (format != DXGI_FORMAT_UNKNOWN)
    {
      UINT formatSupport;
      result = mDevice->CheckFormatSupport(format, &formatSupport);
      if (SUCCEEDED(result) && (formatSupport & D3D11_FORMAT_SUPPORT_MULTISAMPLE_RENDERTARGET))
      {
        MultisampleSupportInfo supportInfo;

        for (unsigned int j = 1; j <= D3D11_MAX_MULTISAMPLE_SAMPLE_COUNT; j++)
        {
          result = mDevice->CheckMultisampleQualityLevels(format, j, &supportInfo.qualityLevels[j - 1]);
          if (SUCCEEDED(result) && supportInfo.qualityLevels[j - 1] > 0)
          {
            maxSupportedSamples = std::max(j, maxSupportedSamples);
          }
          else
          {
            supportInfo.qualityLevels[j - 1] = 0;
          }
        }

        mMultisampleSupportMap.insert(std::make_pair(format, supportInfo));
      }
    }
  }
  mMaxSupportedSamples = maxSupportedSamples;

  initializeDevice();

  // BGRA texture support is optional in feature levels 10 and 10_1
  UINT formatSupport;
  result = mDevice->CheckFormatSupport(DXGI_FORMAT_B8G8R8A8_UNORM, &formatSupport);
  if (FAILED(result))
  {
    ERR("Error checking BGRA format support: 0x%08X", result);
  }
  else
  {
    const int flags = (D3D11_FORMAT_SUPPORT_TEXTURE2D | D3D11_FORMAT_SUPPORT_RENDER_TARGET);
    mBGRATextureSupport = (formatSupport & flags) == flags;
  }

  // Check floating point texture support
  static const unsigned int requiredTextureFlags = D3D11_FORMAT_SUPPORT_TEXTURE2D | D3D11_FORMAT_SUPPORT_TEXTURECUBE;
  static const unsigned int requiredRenderableFlags = D3D11_FORMAT_SUPPORT_RENDER_TARGET;
  static const unsigned int requiredFilterFlags = D3D11_FORMAT_SUPPORT_SHADER_SAMPLE;

    DXGI_FORMAT float16Formats[] =
    {
      DXGI_FORMAT_R16_FLOAT,
      DXGI_FORMAT_R16G16_FLOAT,
      DXGI_FORMAT_R16G16B16A16_FLOAT,
    };

    DXGI_FORMAT float32Formats[] =
    {
      DXGI_FORMAT_R32_FLOAT,
      DXGI_FORMAT_R32G32_FLOAT,
      DXGI_FORMAT_R32G32B32A32_FLOAT,
    };

    mFloat16TextureSupport = true;
    mFloat16FilterSupport = true;
    mFloat16RenderSupport = true;
    for (unsigned int i = 0; i < ArraySize(float16Formats); i++)
    {
      if (SUCCEEDED(mDevice->CheckFormatSupport(float16Formats[i], &formatSupport)))
      {
        mFloat16TextureSupport = mFloat16TextureSupport && (formatSupport & requiredTextureFlags) == requiredTextureFlags;
        mFloat16FilterSupport = mFloat16FilterSupport && (formatSupport & requiredFilterFlags) == requiredFilterFlags;
        mFloat16RenderSupport = mFloat16RenderSupport && (formatSupport & requiredRenderableFlags) == requiredRenderableFlags;
      }
      else
      {
        mFloat16TextureSupport = false;
        mFloat16RenderSupport = false;
        mFloat16FilterSupport = false;
      }
    }

    mFloat32TextureSupport = true;
    mFloat32FilterSupport = true;
    mFloat32RenderSupport = true;
    for (unsigned int i = 0; i < ArraySize(float32Formats); i++)
    {
      if (SUCCEEDED(mDevice->CheckFormatSupport(float32Formats[i], &formatSupport)))
      {
        mFloat32TextureSupport = mFloat32TextureSupport && (formatSupport & requiredTextureFlags) == requiredTextureFlags;
        mFloat32FilterSupport = mFloat32FilterSupport && (formatSupport & requiredFilterFlags) == requiredFilterFlags;
        mFloat32RenderSupport = mFloat32RenderSupport && (formatSupport & requiredRenderableFlags) == requiredRenderableFlags;
      }
      else
      {
        mFloat32TextureSupport = false;
        mFloat32FilterSupport = false;
        mFloat32RenderSupport = false;
      }
    }

    // Check compressed texture support
    const unsigned int requiredCompressedTextureFlags = D3D11_FORMAT_SUPPORT_TEXTURE2D;

    if (SUCCEEDED(mDevice->CheckFormatSupport(DXGI_FORMAT_BC1_UNORM, &formatSupport)))
    {
      mDXT1TextureSupport = (formatSupport & requiredCompressedTextureFlags) == requiredCompressedTextureFlags;
    }
    else
    {
      mDXT1TextureSupport = false;
    }

    if (SUCCEEDED(mDevice->CheckFormatSupport(DXGI_FORMAT_BC3_UNORM, &formatSupport)))
    {
      mDXT3TextureSupport = (formatSupport & requiredCompressedTextureFlags) == requiredCompressedTextureFlags;
    }
    else
    {
      mDXT3TextureSupport = false;
    }

    if (SUCCEEDED(mDevice->CheckFormatSupport(DXGI_FORMAT_BC5_UNORM, &formatSupport)))
    {
      mDXT5TextureSupport = (formatSupport & requiredCompressedTextureFlags) == requiredCompressedTextureFlags;
    }
    else
    {
      mDXT5TextureSupport = false;
    }
    // Check depth texture support
    DXGI_FORMAT depthTextureFormats[] =
    {
      DXGI_FORMAT_D16_UNORM,
      DXGI_FORMAT_D24_UNORM_S8_UINT,
    };

    static const unsigned int requiredDepthTextureFlags = D3D11_FORMAT_SUPPORT_DEPTH_STENCIL |
        D3D11_FORMAT_SUPPORT_TEXTURE2D;

    mDepthTextureSupport = true;
    for (unsigned int i = 0; i < ArraySize(depthTextureFormats); i++)
    {
      if (SUCCEEDED(mDevice->CheckFormatSupport(depthTextureFormats[i], &formatSupport)))
      {
        mDepthTextureSupport = mDepthTextureSupport && ((formatSupport & requiredDepthTextureFlags) == requiredDepthTextureFlags);
      }
      else
      {
        mDepthTextureSupport = false;
      }
    }

    return EGL_SUCCESS;

}

}  // namespace azer
