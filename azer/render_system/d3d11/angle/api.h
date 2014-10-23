#pragma once

namespace gl {
class Texture;
}  // namespace gl

namespace azer {
class Texture;
class WindowHost;
class RenderSystem;

/**
 * 从 azer 初始化 ANGLE
 * Display 的创建及 Display 内部创建的 Renderer 是与 egl 实现不同的地方
 * 除此之外实现是相同的
 *
 */
bool InitAngle(RenderSystem* rs, WindowHost* host);

/**
 * 从 texture 获得 azer 可以操作的 texture
 * 注意: 获得指针之后是绝对不能释放的
 *
 */

Texture* GetTexture(gl::Texture* texture);
}  // namespace azer
