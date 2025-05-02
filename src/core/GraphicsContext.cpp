#include "core/GraphicsContext.hpp"
#include <stdexcept>

namespace Core {

GraphicsContext::GraphicsContext(const std::string &title, int width,
                                 int height, Uint32 flags) {
  window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED,
                            SDL_WINDOWPOS_CENTERED, width, height, flags);

  if (!window) {
    throw std::runtime_error("Failed to create window: " +
                             std::string(SDL_GetError()));
  }

  renderer = SDL_CreateRenderer(
      window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

  if (!renderer) {
    SDL_DestroyWindow(window);
    throw std::runtime_error("Failed to create renderer: " +
                             std::string(SDL_GetError()));
  }
}

GraphicsContext::~GraphicsContext() {
  if (renderer) {
    SDL_DestroyRenderer(renderer);
  }
  if (window) {
    SDL_DestroyWindow(window);
  }
}

GraphicsContext::GraphicsContext(GraphicsContext &&other) noexcept
    : window(other.window), renderer(other.renderer) {
  other.window = nullptr;
  other.renderer = nullptr;
}

GraphicsContext &GraphicsContext::operator=(GraphicsContext &&other) noexcept {
  if (this != &other) {
    if (renderer) {
      SDL_DestroyRenderer(renderer);
    }
    if (window) {
      SDL_DestroyWindow(window);
    }
    window = other.window;
    renderer = other.renderer;
    other.window = nullptr;
    other.renderer = nullptr;
  }
  return *this;
}

void GraphicsContext::setWindowTitle(const std::string &title) {
  SDL_SetWindowTitle(window, title.c_str());
}

void GraphicsContext::getWindowSize(int &width, int &height) const {
  SDL_GetWindowSize(window, &width, &height);
}

void GraphicsContext::setWindowSize(int width, int height) {
  SDL_SetWindowSize(window, width, height);
}

} // namespace Core