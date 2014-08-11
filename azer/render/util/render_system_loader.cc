#include "azer/render/util/render_system_loader.h"

#include "base/files/file_path.h"
#include "base/command_line.h"
#include "base/strings/utf_string_conversions.h"
#include "azer/render/render.h"

namespace azer {
bool LoadRenderSystem(WindowHost* host) {
  DCHECK(RenderSystem::Current() == NULL);
  DCHECK(NULL != CommandLine::ForCurrentProcess());
  ::base::FilePath path = CommandLine::ForCurrentProcess()->GetProgram();
  ::base::FilePath dllpath = path.DirName();
  dllpath = dllpath.Append(::base::UTF8ToWide("d3d11_render_system.dll"));

  return RenderSystem::InitRenderSystem(dllpath, host);
}
}  // namespace azer
