// main.cpp (ECS combat + tick loop + status effects + trigger system)
#include "GameManager.h"
#include "TmxMapLoader.h"
#include "TmxObject.h"
#include "TmxObjectGroup.h"
#include <entt/entt.hpp>
#include <functional>
#include <iostream>
#include <queue>
#include <string>
#include <thread>

// === Components ===
struct Name {
  std::string value;
};
struct Position {
  int x = 0, y = 0;
};
struct Health {
  int current = 100;
};
struct Attack {
  int damage = 5;
  int speed = 4;
}; // speed in ticks
struct Movement {
  int speed = 2;
  int lastMoveTick = 0;
};
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

// === Direction enum (optional movement logic) ===
enum class Direction { NONE, UP, DOWN, LEFT, RIGHT };

// === Scheduled Action ===
struct ScheduledAction {
  int tick;
  entt::entity entity;
  std::function<void(entt::entity, entt::registry &)> action;

  bool operator<(const ScheduledAction &other) const {
    return tick > other.tick; // min-heap
  }
};

// === Scheduler ===
class Scheduler {
public:
  void schedule(const ScheduledAction &action) { queue.push(action); }

  void update(int current_tick, entt::registry &registry) {
    while (!queue.empty() && queue.top().tick <= current_tick) {
      auto action = queue.top();
      queue.pop();
      action.action(action.entity, registry);
    }
  }

private:
  std::priority_queue<ScheduledAction> queue;
};

// === Main ===
int main() {
  GameManager game;

  if (!game.init()) {
    std::cerr << "Failed to initialize game!" << std::endl;
    return 1;
  }

  game.run();

  return 0;
}
