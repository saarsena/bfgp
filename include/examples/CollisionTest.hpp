#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <entt/entt.hpp>
#include <memory>
#include <vector>

#include "core/Components.h"
#include "core/Constants.h"
#include "core/DungeonGenerator.hpp"
#include "core/EntityFactory.hpp"
#include "core/GameManager.h"
#include "core/GraphicsContext.hpp"
#include "core/Mob.h"
#include "events/EventBus.h"
#include "events/GameEvent.h"
#include "input/InputHandler.hpp"
#include "rendering/Renderer.hpp"
#include "ui/EntityInspectorWindow.hpp"
#include "ui/PerformanceWindow.hpp"
#include "ui/TmxLoaderWindow.hpp"
#include "ui/WalkerDungeonWindow.hpp"

namespace Examples {

// Use namespace alias to make code more readable
using namespace Constants::Sprites;

class CollisionTest {
public:
  CollisionTest();
  ~CollisionTest();

  void run();
  void update();
  void setupTestScenario();
  void regenerateDungeon();

private:
  void setupEventHandlers();
  bool loadTilesets();
  bool loadTmxMap();
  void handleInput();
  void tickUpdate();
  void renderFrame();
  void updateViewportPosition();

  // Window and viewport constants
  static constexpr int WINDOW_WIDTH = Constants::WINDOW_WIDTH;
  static constexpr int WINDOW_HEIGHT = Constants::WINDOW_HEIGHT;
  static constexpr int VIEWPORT_WIDTH = Constants::Viewport::VIEWPORT_WIDTH;
  static constexpr int VIEWPORT_HEIGHT = Constants::Viewport::VIEWPORT_HEIGHT;

  // Core systems
  entt::registry m_registry;
  std::unique_ptr<Core::GraphicsContext> m_graphicsContext;
  GameManager m_gameManager;
  std::unique_ptr<Core::EntityFactory> m_entityFactory;
  std::unique_ptr<Core::WalkerDungeonGenerator> m_dungeonGenerator;

  // Viewport tracking
  SDL_Rect m_viewport;      // Current viewport rectangle
  int m_currentWindowWidth; // Track current window size for resizing
  int m_currentWindowHeight;

  // UI components
  std::unique_ptr<UI::PerformanceWindow> m_performanceWindow;
  std::unique_ptr<UI::EntityInspectorWindow> m_entityInspector;
  std::unique_ptr<UI::TmxLoaderWindow> m_tmxLoader;
  std::unique_ptr<UI::WalkerDungeonWindow> m_walkerDungeon;

  // Rendering and input
  std::unique_ptr<Rendering::Renderer> m_renderer;
  std::unique_ptr<Input::InputHandler> m_inputHandler;
  EventBus m_eventBus;

  // Game state
  std::vector<SDL_Rect> m_collisionObjects;
  bool m_running = true;
  Uint32 m_lastFrameTime = 0;
  Uint32 m_lastTickTime = 0;
  Uint32 m_accumulator = 0;
  int m_currentTick = 0;

  // UI visibility flags
  bool m_showPerformanceWindow;
  bool m_showEntityInspector;
  bool m_showTmxLoader;
  bool m_showWalkerDungeon;
  bool m_usingDungeonGenerator;

  // Dungeon generation parameters
  int m_totalFloorCount;
  int m_minHall;
  int m_maxHall;
  int m_roomDim;
  int m_tileWidth;
  int m_tileHeight;

  // Constants
  static constexpr float DEFAULT_ZOOM = 1.0f;
  static constexpr float MIN_ZOOM = 0.1f;
  static constexpr float MAX_ZOOM = 4.0f;
  static constexpr float ZOOM_SPEED = 0.1f;
  static constexpr Uint32 MS_PER_TICK = 16; // ~60 FPS
  static constexpr float TICKS_PER_SECOND = 60.0f;
};

} // namespace Examples