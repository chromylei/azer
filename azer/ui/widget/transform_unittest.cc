#include "azer/ui/widget/coordinate_transform.h"
#include "azer/ui/gfx/gfx.h"
#include "gtest/gtest.h"

TEST(WindowToRenderer, Case) {
  gfx::Rect win(0, 0, 800, 600);
  gfx::Rect rc1(400, 0, 400, 300);
  gfx::RectF rcf1 = azer::WindowToRenderer(rc1, win);
  ASSERT_EQ(rcf1, gfx::RectF(0.0f, 0.0f, 1.0f, 1.0f));

  gfx::Rect rc2(0, 0, 400, 300);
  gfx::RectF rcf2 = azer::WindowToRenderer(rc2, win);
  ASSERT_EQ(rcf2, gfx::RectF(-1.0f, 0.0f, 1.0f, 1.0f));

  gfx::Rect rc3(0, 300, 400, 300);
  gfx::RectF rcf3 = azer::WindowToRenderer(rc3, win);
  ASSERT_EQ(rcf3, gfx::RectF(-1.0f, -1.0f, 1.0f, 1.0f));

  gfx::Rect rc4(400, 300, 400, 300);
  gfx::RectF rcf4 = azer::WindowToRenderer(rc4, win);
  ASSERT_EQ(rcf4, gfx::RectF(0.0f, -1.0f, 1.0f, 1.0f));
}
