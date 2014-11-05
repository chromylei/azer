#pragma once

#if defined(BUILD_AZER_EVENT)
#     define AZER_EVENT_EXPORT __declspec(dllexport)
#     define AZER_EVENT_EXPORT_PRIVATE __declspec(dllexport)
#else
#     define AZER_EVENT_EXPORT __declspec(dllimport)
#     define AZER_EVENT_EXPORT_PRIVATE __declspec(dllimport)
#endif
