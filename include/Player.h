#pragma once
#include "EventBus.h"
#include <SDL.h>

class Player {
public:
  Player(int tileX, int tileY, SDL_Texture *texture, int tileSize = 16);
  void handleInput(const SDL_Event &e, EventBus &bus);
  void update();
  void render(SDL_Renderer *renderer);

  // New movement method that publishes events
  bool move(int dx, int dy, EventBus &bus);

  // Getters
  int getTileX() const { return tileX; }
  int getTileY() const { return tileY; }

private:
  int tileX, tileY; // Position in tile coordinates
  int tileSize;     // Size of each tile in pixels
  SDL_Texture *texture;
};
