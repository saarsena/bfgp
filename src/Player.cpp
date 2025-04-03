#include "Player.h"
#include "GameEvent.h"
#include <SDL.h>

Player::Player(int tileX, int tileY, SDL_Texture *texture, int tileSize)
    : tileX(tileX), tileY(tileY), texture(texture), tileSize(tileSize) {}

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

bool Player::move(int dx, int dy, EventBus &bus) {
  tileX += dx;
  tileY += dy;
  bus.publish(PlayerMovedEvent(tileX, tileY));
  return true;
}

void Player::update() {
  // Add future logic here
}

void Player::render(SDL_Renderer *renderer) {
  // Calculate pixel coordinates based on tile position
  int pixelX = tileX * tileSize;
  int pixelY = tileY * tileSize;

  if (texture) {
    SDL_Rect dest = {pixelX, pixelY, tileSize, tileSize};
    SDL_RenderCopy(renderer, texture, nullptr, &dest);
  } else {
    // If no texture, draw a simple colored rectangle
    SDL_Rect playerRect = {pixelX, pixelY, tileSize, tileSize};
    SDL_SetRenderDrawColor(renderer, 255, 50, 50, 255); // Red
    SDL_RenderFillRect(renderer, &playerRect);
  }
}
