#pragma once

#include "core/CollisionSystem.h"
#include "core/GameManager.h"
#include "core/Mob.h"
#include "core/Player.h"
#include "events/GameEvent.h"
#include <SDL2/SDL.h>
#include <entt/entt.hpp>

// Window dimensions
constexpr int WINDOW_WIDTH = 1024;
constexpr int WINDOW_HEIGHT = 768;
constexpr int TILE_SIZE = 32;

// Forward declarations
struct CollisionEvent {
  CollisionEvent(int x, int y) : x(x), y(y) {}
  int x, y;
};

class CollisionTest {
public:
  CollisionTest();
  ~CollisionTest();
  void run();

private:
  void setupEventHandlers();
  void setupTestScenario();
  void handleInput();
  void update();
  void render();
  void renderDebug();
  void createWall(int x, int y, int w, int h);

  SDL_Window *window;
  SDL_Renderer *renderer;
  bool isRunning;
  entt::registry registry;
  EventBus eventBus;
  Player *player;
  Mob *mob;
  std::vector<SDL_Rect> collisionObjects;
};