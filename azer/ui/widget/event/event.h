#pragma once

#include "base/basictypes.h"
#include "azer/base/render_export.h"
#include "azer/ui/widget/widget.h"

namespace azer {
namespace ui {
class AZER_EXPORT Event {
 public:
 protected:
  DISALLOW_COPY_AND_ASSIGN(Event);
};

typedef std::shared_ptr<Event> EventPtr;

/**
 * ��Ϣ���ݵĴ���
 * ��Ϣ�� rootwindow ��ȡ�����ղ�ι�ϵ�·���ָ�����մ��ڴ�����Ϣλ��
 */
class AZER_EXPORT EventTarget {
 public:
  EventTarget();

  /**
   * Ԥ������Ϣ�������Ϣ���������̼����·����򷵻� false
   */
  virtual bool PreProcessEvent(Event* event) = 0;
 private:
  DISALLOW_COPY_AND_ASSIGN(EventTarget);
};
}  // namespace ui
}  // namespace azer
