#include "../include/core/WanderSystem.hpp"
#include <cstdlib>

namespace Systems {

void WanderSystem(entt::registry &registry, Uint32 currentTime,
                  const std::vector<SDL_Rect> &collisionRects, int tileWidth,
                  int tileHeight) {
  auto view =
      registry
          .view<Components::PositionComponent, Components::WanderComponent>();

  for (auto entity : view) {
    auto &pos = view.get<Components::PositionComponent>(entity);
    auto &wander = view.get<Components::WanderComponent>(entity);

    if (currentTime - wander.lastMoveTime < wander.moveCooldown)
      continue;

    // Pick a random direction if we don't have one or if we're blocked
    if (wander.direction == -1) {
      wander.direction = rand() % 4;
    }

    int dx = 0, dy = 0;
    switch (wander.direction) {
    case 0:
      dy = -1;
      break; // up
    case 1:
      dx = 1;
      break; // right
    case 2:
      dy = 1;
      break; // down
    case 3:
      dx = -1;
      break; // left
    }

    int newX = pos.x + dx;
    int newY = pos.y + dy;

    // Create a rectangle for collision checking
    SDL_Rect proposed = {newX * tileWidth, newY * tileHeight, tileWidth,
                         tileHeight};

    bool blocked = false;
    for (const auto &rect : collisionRects) {
      if (SDL_HasIntersection(&proposed, &rect)) {
        blocked = true;
        wander.direction = -1; // Reset direction if blocked
        break;
      }
    }

    // Also check boundaries (assuming map is 30x20 tiles)
    if (newX < 0 || newX >= 30 || newY < 0 || newY >= 20) {
      blocked = true;
      wander.direction = -1;
    }

    if (!blocked) {
      pos.x = newX;
      pos.y = newY;
      // Occasionally change direction even when not blocked
      if (rand() % 4 == 0) {
        wander.direction = -1;
      }
    }

    wander.lastMoveTime = currentTime;
  }
}
} // namespace Systems