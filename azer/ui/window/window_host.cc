#include "azer/ui/window/window_host.h"

#include "base/logging.h"

namespace azer {
WindowHost::WindowHost(const Options& opt, Delegate* delegate)
    : delegate_(delegate)
    , render_system_(NULL)
    , options_(opt) {
}

WindowHost::WindowHost(const Options& opt)
    : delegate_(NULL)
    , render_system_(NULL)
    , options_(opt) {
}

WindowHost::~WindowHost() {
}

}  // namespace azer
