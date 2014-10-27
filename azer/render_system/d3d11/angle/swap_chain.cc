#include "azer/render_system/d3d11/angle/swap_chain.h"

AzerSwapChain::AzerSwapChain(rx::Renderer11 *renderer, HWND window,
                             HANDLE shareHandle,
                             GLenum backBufferFormat,
                             GLenum depthBufferFormat)
    : rx::SwapChain(renderer, window, shareHandle,
                    backBufferFormat, depthBufferFormat) {
}

AzerSwapChain::~AzerSwapChain() {
}
