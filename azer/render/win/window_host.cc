#include "azer/render/win/window_host.h"

#include "base/logging.h"

namespace azer {
WindowHost::WindowHost(const Options& opt, Delegate* delegate)
    : handle_(0)
    , delegate_(delegate)
    , render_system_(NULL)
    , options_(opt) {
}

WindowHost::WindowHost(const Options& opt)
    : handle_(0)
    , delegate_(NULL)
    , render_system_(NULL)
    , options_(opt) {
}

WindowHost::~WindowHost() {
}

}  // namespace azer
