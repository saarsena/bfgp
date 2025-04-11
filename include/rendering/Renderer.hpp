#pragma once

#include "../core/Components.h"
#include "../core/GameManager.h"
#include <SDL2/SDL.h>
#include <entt/entt.hpp>

namespace Rendering {

class Renderer {
public:
  Renderer(GameManager *gameManager);
  ~Renderer() = default;

  void render(entt::registry &registry);
  void setViewport(const SDL_Rect &viewport) { m_viewport = viewport; }
  void setCameraScale(float scale) { m_cameraScale = scale; }
  float getCameraScale() const { return m_cameraScale; }

private:
  void renderGameEntities(entt::registry &registry, int offsetX, int offsetY);
  void calculateCameraOffset(entt::registry &registry, int &offsetX,
                             int &offsetY);

  GameManager *m_gameManager;
  SDL_Rect m_viewport{0, 0, 1024, 768};
  float m_cameraScale{4.0f}; // Scale up 8x8 tiles by 4x to make them 32x32

  // Gameplay area dimensions (30x20 tiles at 32x32 pixels each)
  static constexpr int GAMEPLAY_WIDTH = 30 * 32;  // 960 pixels
  static constexpr int GAMEPLAY_HEIGHT = 20 * 32; // 640 pixels
};

} // namespace Rendering