#pragma once

#include <SDL2/SDL.h>
#include <memory>

namespace Core {
namespace Services {

class RendererService {
public:
  explicit RendererService(SDL_Renderer *renderer) : renderer(renderer) {}

  SDL_Renderer *getRenderer() const { return renderer; }
  void setRenderer(SDL_Renderer *newRenderer) { renderer = newRenderer; }

private:
  SDL_Renderer *renderer;
};

} // namespace Services
} // namespace Core