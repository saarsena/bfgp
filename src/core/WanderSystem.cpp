#include "core/WanderSystem.hpp"
#include "core/CollisionSystem.h"
#include <cstdlib>
#include <iostream>

namespace Systems {

void WanderSystem(entt::registry &registry, Uint32 currentTime) {
  auto view =
      registry
          .view<Components::PositionComponent, Components::WanderComponent>();

  for (auto entity : view) {
    auto &pos = view.get<Components::PositionComponent>(entity);
    auto &wander = view.get<Components::WanderComponent>(entity);

    // Check if enough time has passed since last move
    if (currentTime - wander.lastMoveTime < wander.moveCooldown) {
      continue;
    }

    // Pick a random direction if we don't have one
    if (wander.direction == -1) {
      wander.direction = rand() % 4;
      std::cout << "Mob at (" << pos.x << "," << pos.y
                << ") choosing new direction: " << wander.direction
                << std::endl;
    }

    // Calculate movement based on direction
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

    // Calculate new position
    Components::PositionComponent newPos{pos.x + dx, pos.y + dy};

    // Check for collisions using the CollisionSystem
    bool blocked = CollisionSystem::CheckCollision(registry, newPos, entity);
    if (blocked) {
      wander.direction = -1; // Reset direction if blocked
      std::cout << "Mob at (" << pos.x << "," << pos.y
                << ") blocked by collision" << std::endl;
    } else {
      // Update position if not blocked
      std::cout << "Mob moving from (" << pos.x << "," << pos.y << ") to ("
                << newPos.x << "," << newPos.y << ")" << std::endl;
      pos = newPos;

      // Occasionally change direction even when not blocked
      if (rand() % 4 == 0) {
        wander.direction = -1;
        std::cout << "Mob randomly changing direction" << std::endl;
      }
    }

    // Update last move time
    wander.lastMoveTime = currentTime;
  }
}

} // namespace Systems