#pragma once

#include <string>
#include <memory>

#include "base/basictypes.h"
#include "base/logging.h"

namespace azer {

class Blending {
 public:
  enum Oper {
    kAdd = 1,
    kSub,
    kRevsub,
    kMin,
    kMax,
  };
  enum Blend {
    kZero = 0,
    kOne,
    kSrcColor,
    kSrcInvColor,
    kSrcAlpha,
    kSrcInvAlpha,
    kSrcAlphaSat,
    kSrc1Color,
    kSrc1InvColor,
    kSrc1Alpha,
    kSrc1InvAlpha,
    
    kDestColor,
    kDestInvColor,
    kDestAlpha,
    kDestInvAlpha,

    kFactor,
    kInvFactor,
  };

  enum WriteMask {
    kWriteR = 0x0001L,
    kWriteG = 0x0002L,
    kWriteB = 0x0004L,
    kWriteA = 0x0008L,
    kWriteColor = (kWriteR | kWriteG | kWriteB),
    kWriteAll = (kWriteColor | kWriteA),
  };

  struct Desc {
    Blend src;
    Blend dest;
    Oper oper;
    Blend src_alpha;
    Blend dest_alpha;
    Oper alpha_oper;
    WriteMask mask;
    Desc()
        : src(kOne)
        , dest(kZero)
        , oper(kAdd)
        , src_alpha(kOne)
        , dest_alpha(kZero)
        , alpha_oper(kAdd)
        , mask(kWriteAll) {
    }
  };

 public:
  virtual ~Blending() {}
  const Desc& desc() const { return desc_;}
 protected:
  Blending(const Desc& desc) : desc_(desc) {}
  virtual bool Init() = 0;
  Desc desc_;
};

typedef std::shared_ptr<Blending> BlendingPtr;
}  // namespace azer
