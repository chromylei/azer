#pragma once

#include "azer/scene/scene_mgr.h"
#include "base/basictypes.h"
#include "azer/render/render_export.h"

namespace azer {
/**
 * 静态场景
 * 此类场景确定以后不会改变，如建筑物之类。这类场景通过场景编辑器确定
 * 而后不再作出任何改变
 * 对于 Camera 来说，大部分静态场景可以通过 Camera 的位置直接 Culling 掉
 * * 作为资源管理器的辅助
 * 此外静态场景还作为加载工具使用，一般来说静态场景仅仅加载位置，变换，包围盒
 * 等数据，真正占用大量内存的模型数据贴图数据并不加载。在 Camera 运行过程中
 * Scene 将会选择合适的时机加载(或卸载)这些内容
 */
class AZER_EXPORT StaticSceneManager : public SceneManager {
 public:
 private:
  DISALLOW_COPY_AND_ASSIGN(StaticSceneManager);
};
}  // namespace azer
