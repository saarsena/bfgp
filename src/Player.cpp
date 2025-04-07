#include "../include/core/Player.h"
#include "../include/events/GameEvent.h"
#include <SDL.h>

Player::Player(int tileX, int tileY, SDL_Texture *texture,
               entt::registry &registry, int tileSize)
    : registry(registry), tileSize(tileSize), texture(texture) {
  // Create the player entity
  entity = registry.create();

  // Set up position component
  position.x = tileX;
  position.y = tileY;
  registry.emplace<Components::PositionComponent>(entity, position);

  // Set up collision component (player is not blocking)
  registry.emplace<Components::CollisionComponent>(entity, true);
}

void Player::handleInput(const SDL_Event &e, EventBus &bus) {
  if (e.type == SDL_KEYDOWN) {
    switch (e.key.keysym.sym) {
    case SDLK_w:
      move(0, -1, bus);
      break;
    case SDLK_s:
      move(0, 1, bus);
      break;
    case SDLK_a:
      move(-1, 0, bus);
      break;
    case SDLK_d:
      move(1, 0, bus);
      break;
    }
  }
}

void Player::update() {
  // Update position from the registry
  position = registry.get<Components::PositionComponent>(entity);
}

void Player::render(SDL_Renderer *renderer) {
  SDL_Rect dstRect = {position.x * tileSize, position.y * tileSize, tileSize,
                      tileSize};
  SDL_RenderCopy(renderer, texture, nullptr, &dstRect);
}

bool Player::move(int dx, int dy, EventBus &bus) {
  // Calculate new position
  Components::PositionComponent newPos = position;
  newPos.x += dx;
  newPos.y += dy;

  // Check for collisions
  if (!CollisionSystem::CheckCollision(registry, newPos, entity)) {
    // Update position in registry
    registry.patch<Components::PositionComponent>(
        entity, [&](auto &pos) { pos = newPos; });

    // Update local position
    position = newPos;

    // Publish movement event
    bus.publish(PlayerMovedEvent(position.x, position.y));
    return true;
  }

  return false;
}
