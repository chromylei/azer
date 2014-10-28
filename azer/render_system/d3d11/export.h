
#pragma once

#if defined(BUILD_D3D11_AZER_RENDER_SYSTEM)
#     define AZER_D3D11RS_EXPORT __declspec(dllexport)
#     define AZER_D3D11RS_EXPORT_PRIVATE __declspec(dllexport)
#else
#     define AZER_D3D11RS_EXPORT __declspec(dllimport)
#     define AZER_D3D11RS_EXPORT_PRIVATE __declspec(dllimport)
#endif
