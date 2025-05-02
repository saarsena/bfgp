// main.cpp (ECS combat + tick loop + status effects + trigger system)
#include "../include/core/Components.h"
#include "../include/core/GameManager.h"
#include "../include/core/TmxMapLoader.h"
#include "../include/scheduler/Scheduler.h"
#include <SDL.h>
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

  // Initialize SDL
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
    return;
  }

  // Create a temporary window and renderer
  SDL_Window *window =
      SDL_CreateWindow("TMX Debug", SDL_WINDOWPOS_CENTERED,
                       SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_HIDDEN);
  if (!window) {
    std::cerr << "Window creation failed: " << SDL_GetError() << std::endl;
    SDL_Quit();
    return;
  }

  SDL_Renderer *renderer =
      SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (!renderer) {
    std::cerr << "Renderer creation failed: " << SDL_GetError() << std::endl;
    SDL_DestroyWindow(window);
    SDL_Quit();
    return;
  }

  entt::registry registry;
  TmxMapLoader mapLoader(renderer);

  if (!mapLoader.loadMap("assets/tg_tiled_example.tmx", registry)) {
    std::cerr << "Failed to load map for debugging!" << std::endl;
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return;
  }

  // Print various map information
  mapLoader.printMapInfo();
  mapLoader.printLayerInfo();
  mapLoader.printTilesetInfo();
  mapLoader.printObjectGroups();

  std::cout << "\nMap loaded successfully!\n";

  // Clean up
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
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
