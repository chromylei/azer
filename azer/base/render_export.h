#pragma once

#if defined(BUILD_AZER_RENDER)
#     define AZER_EXPORT __declspec(dllexport)
#     define AZER_EXPORT_PRIVATE __declspec(dllexport)
#else
#     define AZER_EXPORT __declspec(dllimport)
#     define AZER_EXPORT_PRIVATE __declspec(dllimport)
#endif
