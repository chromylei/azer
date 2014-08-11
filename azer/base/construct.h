#pragma once

/*
 * 此宏帮助类能够在堆当中 new
 * 此外，还能够通过 std::unique_ptr 和 std::shared_ptr 管理指针
 * 注意： 需要 include <memory> 并将类的析构函数定义为 protected 或 private
 */
#define DISALLOW_CREATE_ON_STACK_HELPER(CLASSNAME)                     \
  public:                                                              \
  void operator delete(void* const arg) {                              \
    ::delete static_cast<CLASSNAME*>(arg);                             \
  }                                                                    \
  friend struct std::default_delete<CLASSNAME>                        
  
  
