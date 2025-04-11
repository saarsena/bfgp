#pragma once

#include "../core/CollisionSystem.h"
#include "../core/GameManager.h"
#include "../core/Mob.h"
#include "../events/GameEvent.h"
#include <SDL2/SDL.h>
#include <entt/entt.hpp>

namespace Input {

class InputHandler {
public:
  InputHandler(GameManager *gameManager, EventBus &eventBus);
  ~InputHandler() = default;

  bool handleInput(SDL_Event &e, entt::registry &registry, Mob *mob);
  void setZoomConstraints(float minZoom, float maxZoom) {
    m_minZoom = minZoom;
    m_maxZoom = maxZoom;
  }
  void setZoomSpeed(float speed) { m_zoomSpeed = speed; }
  float getCurrentZoom() const { return m_currentZoom; }

private:
  bool handleKeyboardInput(SDL_Event &e, entt::registry &registry, Mob *mob);
  bool handleMouseInput(SDL_Event &e);
  void handlePlayerMovement(entt::registry &registry, int dx, int dy);
  void handleMobMovement(Mob *mob, int dx, int dy);

  GameManager *m_gameManager;
  EventBus &m_eventBus;
  float m_currentZoom{4.0f};
  float m_minZoom{1.0f};
  float m_maxZoom{4.0f};
  float m_zoomSpeed{0.25f};
};

} // namespace Input