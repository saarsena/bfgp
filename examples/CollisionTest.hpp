#pragma once

#include <SDL2/SDL.h>
#include <array>
#include <chrono>
#include <deque>
#include <entt/entity/registry.hpp>
#include <memory>
#include <vector>

#include "core/EntityFactory.hpp"
#include "core/GameManager.hpp"
#include "core/GraphicsContext.hpp"
#include "core/WalkerDungeonGenerator.hpp"
#include "events/EventBus.h"
#include "input/InputHandler.hpp"
#include "rendering/Renderer.hpp"
#include "ui/DebugWindow.hpp"
#include "ui/EntityInspectorWindow.hpp"
#include "ui/PerformanceWindow.hpp"
#include "ui/TmxLoaderWindow.hpp"
#include "ui/WalkerDungeonWindow.hpp"

namespace Examples {

class CollisionTest {
public:
  static constexpr int TARGET_FPS = 60;
  static constexpr Uint32 FRAME_TIME =
      1000 / TARGET_FPS; // Time per frame in milliseconds

  CollisionTest();
  ~CollisionTest();

  void run();
  void update();
  void regenerateDungeon();

private:
  bool loadTmxMap();
  bool loadTilesets();
  void setupEventHandlers();
  void setupTestScenario();
  void updateViewportPosition();
  void handleInput();
  void tickUpdate();
  void renderFrame();

  entt::registry m_registry;
  std::unique_ptr<Core::GraphicsContext> m_graphicsContext;
  Core::GameManager m_gameManager;
  std::unique_ptr<Core::EntityFactory> m_entityFactory;
  std::unique_ptr<Core::WalkerDungeonGenerator> m_dungeonGenerator;
  SDL_Rect m_viewport;
  int m_currentWindowWidth;
  int m_currentWindowHeight;
  std::unique_ptr<UI::PerformanceWindow> m_performanceWindow;
  std::unique_ptr<UI::EntityInspectorWindow> m_entityInspector;
  std::unique_ptr<UI::TmxLoaderWindow> m_tmxLoader;
  std::unique_ptr<UI::WalkerDungeonWindow> m_walkerDungeon;
  std::unique_ptr<Rendering::Renderer> m_renderer;
  std::unique_ptr<Input::InputHandler> m_inputHandler;
  EventBus m_eventBus;
  bool m_running;
  Uint32 m_lastFrameTime;
  Uint32 m_lastTickTime;
  Uint32 m_accumulator;
  Uint32 m_currentTick;
  bool m_showPerformanceWindow;
  bool m_showEntityInspector;
  bool m_showTmxLoader;
  bool m_showWalkerDungeon;
  bool m_usingDungeonGenerator;
  int m_totalFloorCount;
  int m_minHall;
  int m_maxHall;
  int m_roomDim;
  int m_tileWidth;
  int m_tileHeight;
};

} // namespace Examples
