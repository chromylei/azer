#pragma once

#include "azer/render/render_export.h"
#include "azer/ui/widget/window.h"
#include "base/basictypes.h"

namespace azer {
namespace ui {
/**
 * Canvas
 * �� ui ����£����е� control ʵ�������ս����ʾ��һ�� texture ��
 * �������ÿһ�� control ���ǵĻ�������ϵ�������Լ�������ϵΪ�ο���
 * ����Ĺ��ܾ����ṩһ���Ա���������ϵ�� canvas ����
 *
 */
class Canvas {
 public:
  Canvas();
 private:
  DISALLOW_COPY_AND_ASSIGN(Canvas);
};
}  // namespace ui
}  // namespace azer
