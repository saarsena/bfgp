#pragma once

#include "Constants.h"
#include <SDL.h>
#include <string>

// Core game components
namespace Components {

// Name component
struct Name {
  std::string value;
};

// Position component
struct Position {
  int x = 0;
  int y = 0;
};

// Health component
struct Health {
  int current = 100;
  int max = 100;
};

// Attack component
struct Attack {
  int damage = 5;
  int speed = 4; // Speed in ticks
};

// Movement component
struct Movement {
  int speed = 2;
  int lastMoveTick = 0;
};

// Player tag component
struct PlayerTag {};

// Status Effect: Poison
struct Poisoned {
  int ticksRemaining = 3;
  int damagePerTick = 2;
  int tickRate = 2;
};

// Trigger: Call for Help
struct CallForHelpOnHit {
  bool triggered = false;
};

// AI Trigger: Join fight if called
struct AddSpawnTrigger {
  bool ready = true;
};

struct PositionComponent {
  int x;
  int y;
};

struct CollisionComponent {
  bool isBlocking; // true for walls, false for passable entities
  int width = Constants::Sprites::DEFAULT_TILE_WIDTH;
  int height = Constants::Sprites::DEFAULT_TILE_HEIGHT;

  CollisionComponent(bool blocking) : isBlocking(blocking) {}
};

// Marker component to identify our test entity
struct TestComponent {
  // Empty marker component
};

// Marker component to identify the player entity
struct PlayerMarker {
  // Empty marker component
};

// Marker component to identify tile colliders
struct TileColliderTag {
  // Empty marker component
};

// Marker component to identify floor tiles
struct FloorTag {
  // Empty marker component
};

// Marker component to identify mob entities
struct MobTag {
  // Empty marker component
};

// Component for wandering behavior
struct WanderComponent {
  int direction = -1; // 0 = up, 1 = right, 2 = down, 3 = left
  Uint32 lastMoveTime = 0;
  Uint32 moveCooldown = 1000; // ms between steps
};

// Sprite component for rendering entities using tileset sprites or custom
// textures
struct SpriteComponent {
  std::string tilesetName;
  int tileId;
  SDL_Texture *customTexture;

  // Constructor for tileset-based sprites
  SpriteComponent(const std::string &tileset, int id)
      : tilesetName(tileset), tileId(id), customTexture(nullptr) {}

  // Constructor for custom textures
  SpriteComponent(SDL_Texture *texture)
      : tilesetName(""), tileId(0), customTexture(texture) {}
};

} // namespace Components