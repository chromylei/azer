#pragma once

#ifdef DEBUG
#define RENDER_SYSTEM_DLL  TEXT("out/dbg/d3d11_render_system.dll")
#else
#define RENDER_SYSTEM_DLL  TEXT("out/opt/d3d11_render_system.dll")
#endif
