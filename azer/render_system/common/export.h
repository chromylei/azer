
#pragma once

#if defined(BUILD_AZER_RENDER_SYSTEM)
#     define AZER_RS_EXPORT __declspec(dllexport)
#     define AZER_RS_EXPORT_PRIVATE __declspec(dllexport)
#else
#     define AZER_RS_EXPORT __declspec(dllimport)
#     define AZER_RS_EXPORT_PRIVATE __declspec(dllimport)
#endif
