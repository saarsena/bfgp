#include "../include/core/Mob.h"
#include "../include/core/CollisionSystem.h"
#include "../include/events/GameEvent.h"
#include <SDL.h>

Mob::Mob(int tileX, int tileY, SDL_Texture *texture, entt::registry &registry,
         int tileSize)
    : registry(registry), tileSize(tileSize), texture(texture) {

  // Create the mob entity
  entity = registry.create();

  // Set up position component
  position.x = tileX;
  position.y = tileY;
  registry.emplace<Components::PositionComponent>(entity, position);

  // Set up collision component (player is not blocking)
  registry.emplace<Components::CollisionComponent>(entity, true);
}

void Mob::handleInput(const SDL_Event &e, EventBus &bus) {
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

void Mob::update() {
  // Update position from the registry
  position = registry.get<Components::PositionComponent>(entity);
}

void Mob::render(SDL_Renderer *renderer) {
  // Set the color for the mob (green)
  SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // RGBA: Green

  // Create the rectangle at the mob's absolute position
  SDL_Rect dstRect = {position.x * tileSize, position.y * tileSize, tileSize,
                      tileSize};

  // Draw the filled rectangle
  SDL_RenderFillRect(renderer, &dstRect);

  // Draw a border around the rectangle
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // RGBA: Black
  SDL_RenderDrawRect(renderer, &dstRect);
}

void Mob::render(SDL_Renderer *renderer, int offsetX, int offsetY) {
  // Set the color for the mob (green)
  SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // RGBA: Green

  // Create the rectangle at the mob's position with camera offset
  SDL_Rect dstRect = {position.x * tileSize + offsetX,
                      position.y * tileSize + offsetY, tileSize, tileSize};

  // Draw the filled rectangle
  SDL_RenderFillRect(renderer, &dstRect);

  // Draw a border around the rectangle
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // RGBA: Black
  SDL_RenderDrawRect(renderer, &dstRect);
}

bool Mob::move(int dx, int dy, EventBus &bus) {
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
    bus.publish(MobMovedEvent(position.x, position.y));
    return true;
  }

  return false;
}
