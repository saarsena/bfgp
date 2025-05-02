#include "../../include/input/InputHandler.hpp"
#include "../../externals/imgui/backends/imgui_impl_sdl2.h"
#include "../../externals/imgui/imgui.h"
#include <iostream>

namespace Input {

InputHandler::InputHandler(GameManager *gameManager, EventBus &eventBus)
    : m_gameManager(gameManager), m_eventBus(eventBus) {}

bool InputHandler::handleInput(SDL_Event &e, entt::registry &registry,
                               Mob *mob) {
  // Let ImGui handle its events first
  ImGui_ImplSDL2_ProcessEvent(&e);

  // Get ImGui IO
  ImGuiIO &io = ImGui::GetIO();

  // Only block mouse-based events when ImGui wants to capture mouse
  if (e.type == SDL_MOUSEMOTION || e.type == SDL_MOUSEBUTTONDOWN ||
      e.type == SDL_MOUSEBUTTONUP || e.type == SDL_MOUSEWHEEL) {
    if (io.WantCaptureMouse) {
      return true;
    }
    return handleMouseInput(e);
  }

  // Always process game keyboard input unless typing in an ImGui input field
  if (e.type == SDL_KEYDOWN && !io.WantTextInput) {
    return handleKeyboardInput(e, registry, mob);
  }

  return false;
}

bool InputHandler::handleKeyboardInput(SDL_Event &e, entt::registry &registry,
                                       Mob *mob) {
  // Handle WASD for player movement
  auto playerView =
      registry.view<Components::TestComponent, Components::PositionComponent,
                    Components::PlayerMarker>();
  for (auto entity : playerView) {
    int dx = 0, dy = 0;
    switch (e.key.keysym.sym) {
    case SDLK_w:
      dy = -1;
      break;
    case SDLK_s:
      dy = 1;
      break;
    case SDLK_a:
      dx = -1;
      break;
    case SDLK_d:
      dx = 1;
      break;
    case SDLK_q:
    case SDLK_ESCAPE:
      return true; // Signal quit
    default:
      break;
    }

    if (dx != 0 || dy != 0) {
      handlePlayerMovement(registry, dx, dy);
      return true;
    }
  }

  // Handle arrow keys for mob movement
  if (mob) {
    int dx = 0, dy = 0;
    switch (e.key.keysym.sym) {
    case SDLK_LEFT:
      dx = -1;
      break;
    case SDLK_RIGHT:
      dx = 1;
      break;
    case SDLK_UP:
      dy = -1;
      break;
    case SDLK_DOWN:
      dy = 1;
      break;
    default:
      break;
    }

    if (dx != 0 || dy != 0) {
      handleMobMovement(mob, dx, dy);
      return true;
    }
  }

  return false;
}

bool InputHandler::handleMouseInput(SDL_Event &e) {
  if (e.type == SDL_MOUSEWHEEL) {
    float zoomDelta = e.wheel.y * m_zoomSpeed;
    float newZoom = m_currentZoom + zoomDelta;
    m_currentZoom = std::max(m_minZoom, std::min(m_maxZoom, newZoom));
    std::cout << "Camera zoom: " << m_currentZoom << "x" << std::endl;
    return true;
  }
  return false;
}

void InputHandler::handlePlayerMovement(entt::registry &registry, int dx,
                                        int dy) {
  auto playerView =
      registry.view<Components::TestComponent, Components::PositionComponent,
                    Components::PlayerMarker>();
  for (auto entity : playerView) {
    if (CollisionSystem::TryMove(registry, entity, dx, dy, m_eventBus)) {
      const auto &pos = registry.get<Components::PositionComponent>(entity);
      std::cout << "Player moved to: (" << pos.x << ", " << pos.y << ")\n";
    }
  }
}

void InputHandler::handleMobMovement(Mob *mob, int dx, int dy) {
  if (mob) {
    mob->move(dx, dy, m_eventBus);
  }
}

} // namespace Input