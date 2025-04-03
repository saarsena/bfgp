#include "SchedulerExample.h"

void SchedulerExample::runSchedulerExample() {
  std::cout << "=== Scheduler System Example ===" << std::endl;

  // Create registry and scheduler
  entt::registry registry;
  Scheduler scheduler;

  // Create a player entity with health
  auto player = registry.create();
  registry.emplace<Health>(player, 100, 100);

  // Create an enemy entity with health
  auto enemy = registry.create();
  registry.emplace<Health>(enemy, 50, 50);

  // Print entity health
  auto printHealth = [&](const char *name, entt::entity entity) {
    const auto &health = registry.get<Health>(entity);
    std::cout << name << " health: " << health.current << "/" << health.max
              << std::endl;
  };

  // Print initial health
  printHealth("Player", player);
  printHealth("Enemy", enemy);

  // Schedule a basic attack from enemy to player at tick 5
  scheduler.schedule(
      5, enemy, [player](entt::entity attacker, entt::registry &reg) {
        if (reg.valid(attacker) && reg.valid(player)) {
          auto &playerHealth = reg.get<Health>(player);
          playerHealth.current -= 10;
          std::cout << "Enemy attacks player for 10 damage!" << std::endl;
        }
      });

  // Use utility to schedule player attacks on enemy at ticks 3 and 7
  SchedulerUtils::scheduleAttack(scheduler, player, enemy, 15, 3,
                                 [](entt::entity, entt::entity, int damage) {
                                   std::cout << "Player attacks enemy for "
                                             << damage << " damage!"
                                             << std::endl;
                                 });

  SchedulerUtils::scheduleAttack(scheduler, player, enemy, 20, 7,
                                 [](entt::entity, entt::entity, int damage) {
                                   std::cout << "Player attacks enemy for "
                                             << damage << " damage!"
                                             << std::endl;
                                 });

  // Schedule poison damage over time on the player
  SchedulerUtils::scheduleDamageOverTime(
      scheduler, player, 5, 3, 2, 4, [](entt::entity, int damage) {
        std::cout << "Player takes " << damage << " poison damage!"
                  << std::endl;
      });

  // Run the simulation for 10 ticks
  for (int tick = 1; tick <= 10; ++tick) {
    std::cout << "\n-- Tick " << tick << " --" << std::endl;
    scheduler.update(tick, registry);

    // Print health after each tick
    if (registry.valid(player)) {
      printHealth("Player", player);
    }

    if (registry.valid(enemy)) {
      printHealth("Enemy", enemy);
    }
  }
}

void SchedulerExample::runTimedEventExample() {
  std::cout << "\n=== Timed Event System Example ===" << std::endl;

  TimedEventScheduler eventScheduler;

  // Schedule some simple function events
  eventScheduler.scheduleFunction(
      3, []() { std::cout << "Function event at tick 3" << std::endl; });

  eventScheduler.scheduleFunction(
      5, []() { std::cout << "Function event at tick 5" << std::endl; });

  // Create a custom event
  class GameStartEvent : public TimedEvent {
  public:
    GameStartEvent(int tick) : TimedEvent(tick, "GameStart") {}

    void execute() override {
      std::cout << "Game starts! Initializing systems..." << std::endl;
    }
  };

  // Schedule the custom event
  eventScheduler.scheduleEvent<GameStartEvent>(1);

  // Create a cancellable event
  auto eventId = eventScheduler.scheduleFunction(
      4, []() { std::cout << "This event will be cancelled" << std::endl; });

  // Cancel the event
  eventScheduler.cancelEvent(eventId);

  // Run the simulation for 6 ticks
  for (int tick = 1; tick <= 6; ++tick) {
    std::cout << "\n-- Tick " << tick << " --" << std::endl;
    eventScheduler.update(tick);
  }
}

void SchedulerExample::run() {
  runSchedulerExample();
  runTimedEventExample();
}