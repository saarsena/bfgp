#pragma once

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

} // namespace Components