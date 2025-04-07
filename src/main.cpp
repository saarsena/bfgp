// main.cpp (ECS combat + tick loop + status effects + trigger system)
#include "../include/core/Components.h"
#include "../include/core/GameManager.h"
#include "../include/core/TmxMapLoader.h"
#include "../include/scheduler/Scheduler.h"
#include <entt/entt.hpp>
#include <functional>
#include <iostream>
#include <string>
#include <thread>

// === Direction enum (optional movement logic) ===
enum class Direction { NONE, UP, DOWN, LEFT, RIGHT };

// Forward declarations of example classes
class SchedulerExample;
class EnttEventExample;

// Implementation of example runner functions
#include "../include/core/SchedulerExample.h"
void runSchedulerExamples() {
  SchedulerExample example;
  example.run();
}

#include "../include/core/EnttEventExample.h"
void runEnttEventExamples() {
  EnttEventExample example;
  example.run();
}

// Function to run the game
void runGame() {
  GameManager game;

  if (!game.init()) {
    std::cerr << "Failed to initialize game!" << std::endl;
    return;
  }

  game.run();
}

// Add this before runGame() or main()
void debugMapLoader() {
  std::cout << "\n=== TMX Map Debug Info ===\n";

  TmxMapLoader mapLoader;
  if (!mapLoader.loadMap("assets/tg_tiled_example.tmx")) {
    std::cerr << "Failed to load map for debugging!" << std::endl;
    return;
  }

  // Print various map information
  mapLoader.printMapInfo();
  mapLoader.printLayerInfo();
  mapLoader.printTilesetInfo();
  mapLoader.printObjectGroups();

  std::cout << "\nMap loaded successfully!\n";
}

int main(int argc, char *argv[]) {
  // Parse command line arguments
  if (argc > 1) {
    std::string arg = argv[1];

    if (arg == "--scheduler") {
      runSchedulerExamples();
      return 0;
    } else if (arg == "--events") {
      runEnttEventExamples();
      return 0;
    } else if (arg == "--help") {
      std::cout << "Usage: " << argv[0] << " [option]" << std::endl;
      std::cout << "Options:" << std::endl;
      std::cout << "  --scheduler    Run scheduler system examples"
                << std::endl;
      std::cout << "  --events       Run event system examples" << std::endl;
      std::cout << "  --help         Show this help message" << std::endl;
      std::cout << "  (no option)    Run the main game" << std::endl;
      return 0;
    } else if (arg == "--debug-map") {
      debugMapLoader();
      return 0;
    }
  }

  // Default: run the game
  runGame();

  return 0;
}
