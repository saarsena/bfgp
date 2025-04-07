#pragma once
#include "../events/EventBus.h"
#include "Components.h"
#include <SDL.h>
#include <entt/entt.hpp>

class Mob {
public:
  Mob(int tileX, int tileY, SDL_Texture *texture, entt::registry &registry,
      int tileSize = 16);

  void handleInput(const SDL_Event &e, EventBus &bus);
  void update();
  void render(SDL_Renderer *renderer);
  void render(SDL_Renderer *renderer, int offsetX, int offsetY);

  // Movement method that publishes events
  bool move(int dx, int dy, EventBus &bus);

  // Getters
  int getTileX() const { return position.x; }
  int getTileY() const { return position.y; }

private:
  entt::registry &registry;
  entt::entity entity;
  Components::PositionComponent position;
  int tileSize; // Size of each tile in pixels
  SDL_Texture *texture;
};