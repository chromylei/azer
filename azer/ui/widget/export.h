#pragma once

#if defined(BUILD_AZER_WIDGET)
#     define AZER_WIDGET_EXPORT __declspec(dllexport)
#     define AZER_WIDGET_EXPORT_PRIVATE __declspec(dllexport)
#else
#     define AZER_WIDGET_EXPORT __declspec(dllimport)
#     define AZER_WIDGET_EXPORT_PRIVATE __declspec(dllimport)
#endif
