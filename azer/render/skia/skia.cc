#include "azer/render/skia/skia.h"
#include "azer/render/skia/context.h"


namespace azer {
namespace skia {

Context* Init() {
  std::unique_ptr<Context> ctx(new Context());
  if (ctx->Init()) {
    return ctx.release();;
  } else {
    return NULL;
  }
}
}  // namespace skia
}  // namespace azer
