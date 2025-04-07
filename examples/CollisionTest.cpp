#include "CollisionTest.hpp"
#include "core/Components.h"
#include "core/walkerdungeon.hpp"
#include "events/GameEvent.h"
#include <iostream>

CollisionTest::CollisionTest()
    : window(nullptr), renderer(nullptr), isRunning(false) {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
    return;
  }

  window = SDL_CreateWindow("Collision Test", SDL_WINDOWPOS_CENTERED,
                            SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT,
                            SDL_WINDOW_SHOWN);
  if (!window) {
    std::cerr << "Window creation failed: " << SDL_GetError() << std::endl;
    return;
  }

  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (!renderer) {
    std::cerr << "Renderer creation failed: " << SDL_GetError() << std::endl;
    return;
  }

  setupEventHandlers();
  setupTestScenario();
  isRunning = true;
}

CollisionTest::~CollisionTest() {
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}

void CollisionTest::setupEventHandlers() {

  eventBus.subscribe(
      GameEventType::PLAYER_MOVED, [this](const GameEvent &event) {
        const auto &moveEvent = static_cast<const PlayerMovedEvent &>(event);
        std::cout << "Player moved to: (" << moveEvent.newX << ", "
                  << moveEvent.newY << ")" << std::endl;

        // Check for collisions at new position
        auto view = registry.view<Components::PositionComponent,
                                  Components::CollisionComponent>();
        for (auto entity : view) {
          const auto &pos = view.get<Components::PositionComponent>(entity);
          const auto &col = view.get<Components::CollisionComponent>(entity);

          if (col.isBlocking && pos.x == moveEvent.newX &&
              pos.y == moveEvent.newY) {
            std::cout << "Collision detected at position (" << pos.x << ", "
                      << pos.y << ")" << std::endl;
            break;
          }
        }
      });
}

void CollisionTest::setupTestScenario() {
  const int TILE_SIZE = 32;

  // Generate dungeon using WalkerDungeon
  WalkerDungeon walker(100, 3, 6,
                       3); // 100 floors, halls 3-6 long, rooms up to 3x3
  walker.GenerateRoomWalker();

  // Get floor and wall tiles
  const auto &floors = walker.GetFloorList();
  const auto &walls = walker.GetWallList();

  // Find a suitable spawn point (first floor tile)
  SDL_Point spawnPoint = floors[0];
  std::cout << "Player spawning at tile position (" << spawnPoint.x << ", "
            << spawnPoint.y << ")" << std::endl;

  // Create player entity (white square) at spawn point
  auto playerEntity = registry.create();
  registry.emplace<Components::PositionComponent>(playerEntity, spawnPoint.x,
                                                  spawnPoint.y);
  registry.emplace<Components::TestComponent>(playerEntity);
  registry.emplace<Components::CollisionComponent>(playerEntity, false);
  registry.emplace<Components::PlayerMarker>(
      playerEntity); // Add marker to distinguish player

  // Create mob entity at a different floor tile
  SDL_Point mobPoint = floors[1];
  mob = new Mob(mobPoint.x, mobPoint.y, nullptr, registry, TILE_SIZE);

  // Create collision components and visual blocks for walls
  for (const auto &wall : walls) {
    auto entity = registry.create();
    registry.emplace<Components::PositionComponent>(entity, wall.x, wall.y);
    registry.emplace<Components::CollisionComponent>(entity, true);

    // Add visual representation
    collisionObjects.push_back(
        {wall.x * TILE_SIZE, wall.y * TILE_SIZE, TILE_SIZE, TILE_SIZE});
  }
}

void CollisionTest::createWall(int tileX, int tileY, int tileWidth,
                               int tileHeight) {
  const int TILE_SIZE = 32;

  // Create collision components for every tile in the wall
  for (int x = tileX; x < tileX + tileWidth; x++) {
    for (int y = tileY; y < tileY + tileHeight; y++) {
      auto entity = registry.create();
      registry.emplace<Components::PositionComponent>(entity, x, y);
      registry.emplace<Components::CollisionComponent>(entity, true);
    }
  }

  // Convert tile coordinates to pixel coordinates for rendering
  collisionObjects.push_back({tileX * TILE_SIZE, tileY * TILE_SIZE,
                              tileWidth * TILE_SIZE, tileHeight * TILE_SIZE});
}

void CollisionTest::run() {
  while (isRunning) {
    handleInput();
    update();
    render();
    SDL_Delay(16); // ~60 FPS
  }
}

void CollisionTest::handleInput() {
  SDL_Event e;
  while (SDL_PollEvent(&e)) {
    if (e.type == SDL_QUIT) {
      isRunning = false;
    } else if (e.type == SDL_KEYDOWN) {
      // Handle WASD for player movement
      auto playerView =
          registry
              .view<Components::TestComponent, Components::PositionComponent,
                    Components::PlayerMarker>();
      for (auto [entity, pos] : playerView.each()) {
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
        default:
          break;
        }

        if (dx != 0 || dy != 0) {
          // Check for collisions before moving
          bool canMove = true;
          auto collisions = registry.view<Components::CollisionComponent,
                                          Components::PositionComponent>();
          for (auto [collider, collision, colliderPos] : collisions.each()) {
            if (colliderPos.x == pos.x + dx && colliderPos.y == pos.y + dy) {
              if (collision.isBlocking) {
                canMove = false;
                break;
              }
            }
          }

          if (canMove) {
            pos.x += dx;
            pos.y += dy;
            // Emit movement event
            PlayerMovedEvent event(pos.x, pos.y);
            eventBus.publish(event);
            std::cout << "Player moved to: (" << pos.x << ", " << pos.y
                      << ")\n";
          }
        }
      }

      // Handle arrow keys for mob movement
      if (mob) {
        switch (e.key.keysym.sym) {
        case SDLK_LEFT:
          mob->move(-1, 0, eventBus);
          break;
        case SDLK_RIGHT:
          mob->move(1, 0, eventBus);
          break;
        case SDLK_UP:
          mob->move(0, -1, eventBus);
          break;
        case SDLK_DOWN:
          mob->move(0, 1, eventBus);
          break;
        }
      }
    }
  }
}

void CollisionTest::update() {
  // Update player logic here
}

void CollisionTest::render() {
  SDL_SetRenderDrawColor(renderer, 0, 43, 54, 255);
  SDL_RenderClear(renderer);

  // Get the player's position (entity with PlayerMarker)
  auto playerView =
      registry.view<Components::PositionComponent, Components::PlayerMarker>();
  auto [playerEntity, playerPos] = *playerView.each().begin();

  // Calculate camera offset to center on player
  int offsetX = (WINDOW_WIDTH / 2) - (playerPos.x * TILE_SIZE);
  int offsetY = (WINDOW_HEIGHT / 2) - (playerPos.y * TILE_SIZE);

  // Render red walls with offset
  SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
  for (const auto &obj : collisionObjects) {
    SDL_Rect adjustedRect = {obj.x + offsetX, obj.y + offsetY, obj.w, obj.h};
    SDL_RenderFillRect(renderer, &adjustedRect);
  }

  // Render mob with camera offset
  if (mob) {
    mob->render(renderer, offsetX, offsetY);
  }

  // Render player (white square) with offset
  SDL_Rect playerRect = {playerPos.x * TILE_SIZE + offsetX,
                         playerPos.y * TILE_SIZE + offsetY, TILE_SIZE,
                         TILE_SIZE};
  SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
  SDL_RenderFillRect(renderer, &playerRect);

  // Debug: show collision boundaries
  auto collisionView = registry.view<Components::PositionComponent>();
  for (auto entity : collisionView) {
    const auto &pos = collisionView.get<Components::PositionComponent>(entity);
    SDL_Rect debugRect = {pos.x * TILE_SIZE + offsetX,
                          pos.y * TILE_SIZE + offsetY, TILE_SIZE, TILE_SIZE};

    if (registry.all_of<Components::PlayerMarker>(entity)) {
      SDL_SetRenderDrawColor(renderer, 255, 255, 255, 64); // White for player
    } else if (registry.all_of<Components::TestComponent>(entity) &&
               !registry.all_of<Components::PlayerMarker>(entity)) {
      SDL_SetRenderDrawColor(renderer, 255, 255, 255,
                             64); // White for test entity
    } else {
      SDL_SetRenderDrawColor(renderer, 255, 255, 255, 64); // White for walls
    }
    SDL_RenderDrawRect(renderer, &debugRect);
  }

  SDL_RenderPresent(renderer);
}

void CollisionTest::renderDebug() {
  // Add debug visualization here
  // For example: collision boundaries, player position, etc.
}