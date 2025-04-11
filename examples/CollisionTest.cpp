#include "examples/CollisionTest.hpp"

#include <iostream>
#include <memory>

#include "../deps/tmxparser/include/TmxTileLayer.h"
#include "../deps/tmxparser/include/TmxTileset.h"
#include "../externals/imgui/backends/imgui_impl_sdl2.h"
#include "../externals/imgui/backends/imgui_impl_sdlrenderer2.h"
#include "../externals/imgui/imgui.h"
#include "core/CollisionSystem.h"
#include "core/Components.h"
#include "core/Constants.h"
#include "core/DungeonGenerator.hpp"
#include "core/EntityFactory.hpp"
#include "core/GraphicsContext.hpp"
#include "core/WanderSystem.hpp"
#include "core/walkerdungeon.hpp"
#include "events/GameEvent.h"
#include "rendering/Renderer.hpp"

namespace Examples {
using namespace Constants::Sprites; // This lets us use IDs:: and Tilesets::
                                    // directly

CollisionTest::CollisionTest()
    : m_registry(),
      m_graphicsContext(std::make_unique<Core::GraphicsContext>(
          "Collision Test", Constants::WINDOW_WIDTH, Constants::WINDOW_HEIGHT,
          SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE)),
      m_gameManager(),
      m_entityFactory(std::make_unique<Core::EntityFactory>(m_registry)),
      m_dungeonGenerator(nullptr),
      m_viewport{0, 0, VIEWPORT_WIDTH, VIEWPORT_HEIGHT},
      m_currentWindowWidth(Constants::WINDOW_WIDTH),
      m_currentWindowHeight(Constants::WINDOW_HEIGHT),
      m_performanceWindow(std::make_unique<UI::PerformanceWindow>(
          "Performance", &m_showPerformanceWindow)),
      m_entityInspector(
          std::make_unique<UI::EntityInspectorWindow>(&m_showEntityInspector)),
      m_tmxLoader(std::make_unique<UI::TmxLoaderWindow>(&m_gameManager,
                                                        &m_showTmxLoader)),
      m_walkerDungeon(
          std::make_unique<UI::WalkerDungeonWindow>(&m_showWalkerDungeon)),
      m_renderer(std::make_unique<Rendering::Renderer>(&m_gameManager)),
      m_inputHandler(
          std::make_unique<Input::InputHandler>(&m_gameManager, m_eventBus)),
      m_eventBus(), m_running(true), m_lastFrameTime(0), m_lastTickTime(0),
      m_accumulator(0), m_currentTick(0), m_showPerformanceWindow(true),
      m_showEntityInspector(true), m_showTmxLoader(true),
      m_showWalkerDungeon(true), m_usingDungeonGenerator(false),
      m_totalFloorCount(100), m_minHall(3), m_maxHall(10), m_roomDim(5),
      m_tileWidth(8), m_tileHeight(8) {
  std::cout << "Starting CollisionTest constructor..." << std::endl;

  // Initialize SDL
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError()
              << std::endl;
    return;
  }
  std::cout << "SDL initialized successfully" << std::endl;

  // Initialize SDL_image
  if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
    std::cerr << "SDL_image could not initialize! SDL_image Error: "
              << IMG_GetError() << std::endl;
    return;
  }
  std::cout << "SDL_image initialized successfully" << std::endl;

  // Initialize SDL_ttf
  if (TTF_Init() == -1) {
    std::cerr << "SDL_ttf could not initialize! SDL_ttf Error: "
              << TTF_GetError() << std::endl;
    return;
  }
  std::cout << "SDL_ttf initialized successfully" << std::endl;

  // Initialize GameManager with the graphics context
  std::cout << "Initializing GameManager..." << std::endl;
  if (!m_gameManager.init(m_graphicsContext->getWindow(),
                          m_graphicsContext->getRenderer())) {
    std::cerr << "Failed to initialize GameManager!" << std::endl;
    return;
  }
  std::cout << "GameManager initialized successfully" << std::endl;

  // Load tilesets
  std::cout << "Loading tilesets..." << std::endl;
  if (!loadTilesets()) {
    std::cerr << "Failed to load tilesets!" << std::endl;
    return;
  }
  std::cout << "Tilesets loaded successfully" << std::endl;

  // Initialize dungeon generator with default tile size
  std::cout << "Initializing dungeon generator..." << std::endl;
  m_dungeonGenerator = std::make_unique<Core::WalkerDungeonGenerator>(
      m_totalFloorCount, m_minHall, m_maxHall, m_roomDim, m_tileWidth,
      m_tileHeight);
  std::cout << "Dungeon generator initialized successfully" << std::endl;

  // Setup ImGui
  std::cout << "Setting up ImGui..." << std::endl;
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

  // Setup ImGui backends
  ImGui_ImplSDL2_InitForSDLRenderer(m_graphicsContext->getWindow(),
                                    m_graphicsContext->getRenderer());
  ImGui_ImplSDLRenderer2_Init(m_graphicsContext->getRenderer());
  std::cout << "ImGui setup completed" << std::endl;

  // Setup event handlers
  std::cout << "Setting up event handlers..." << std::endl;
  setupEventHandlers();
  std::cout << "Event handlers setup completed" << std::endl;

  // Setup test scenario
  std::cout << "Setting up test scenario..." << std::endl;
  setupTestScenario();
  std::cout << "Test scenario setup completed" << std::endl;

  // Initialize viewport position
  updateViewportPosition();

  // Set up walker dungeon callback
  m_walkerDungeon->setGenerationCallback([this]() {
    // Update dungeon generator parameters from GUI
    m_totalFloorCount = m_walkerDungeon->getTotalFloorCount();
    m_minHall = m_walkerDungeon->getMinHall();
    m_maxHall = m_walkerDungeon->getMaxHall();
    m_roomDim = m_walkerDungeon->getRoomDim();
    m_tileWidth = m_walkerDungeon->getTileWidth();
    m_tileHeight = m_walkerDungeon->getTileHeight();

    // Update dungeon generator
    m_dungeonGenerator->setParameters(m_totalFloorCount, m_minHall, m_maxHall,
                                      m_roomDim, m_tileWidth, m_tileHeight);

    // Generate new dungeon
    m_usingDungeonGenerator = true;
    setupTestScenario();
  });

  // Set up TMX loader callback
  m_tmxLoader->setRegenerationCallback([this]() {
    m_usingDungeonGenerator = false;
    if (loadTmxMap()) {
      setupTestScenario();
    }
  });
}

bool CollisionTest::loadTmxMap() {
  std::cout << "Loading TMX file..." << std::endl;
  if (!m_gameManager.getMapLoader()->loadMap(Constants::Assets::MAP_TMX,
                                             m_registry)) {
    std::cerr << "Failed to load TMX file!" << std::endl;
    return false;
  }
  std::cout << "TMX file loaded successfully" << std::endl;
  return true;
}

bool CollisionTest::loadTilesets() {
  std::cout << "Loading sprite sheet from: " << Constants::Assets::SPRITE_SHEET
            << std::endl;

  // Load sprite sheet texture
  SDL_Surface *surface = IMG_Load(Constants::Assets::SPRITE_SHEET.c_str());
  if (!surface) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "Failed to load sprite sheet: %s", IMG_GetError());
    return false;
  }

  std::cout << "Sprite sheet loaded successfully, creating texture..."
            << std::endl;

  SDL_Texture *texture =
      SDL_CreateTextureFromSurface(m_graphicsContext->getRenderer(), surface);
  SDL_FreeSurface(surface);

  if (!texture) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to create texture: %s",
                 SDL_GetError());
    return false;
  }

  std::cout << "Texture created successfully, setting in map loader..."
            << std::endl;

  m_gameManager.getMapLoader()->setTilesetTexture(
      Constants::Sprites::Tilesets::MAIN_TILESET, texture);

  std::cout << "Tileset texture set successfully" << std::endl;
  return true;
}

CollisionTest::~CollisionTest() {
  // Cleanup ImGui
  ImGui_ImplSDLRenderer2_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();

  // Clean up GameManager (which will clean up SDL resources)
  m_gameManager.clean();
}

void CollisionTest::setupEventHandlers() {
  // Log player movement events
  m_eventBus.subscribe(GameEventType::PLAYER_MOVED, [](const GameEvent &event) {
    const auto &moveEvent = static_cast<const PlayerMovedEvent &>(event);
    std::cout << "Player moved to: (" << moveEvent.newX << ", "
              << moveEvent.newY << ")\n";
  });
}

void CollisionTest::setupTestScenario() {
  std::cout << "Starting setupTestScenario..." << std::endl;

  // Clear existing entities
  m_registry.clear();

  // If we're using the dungeon generator, generate a new dungeon
  if (m_usingDungeonGenerator && m_dungeonGenerator) {
    std::cout << "Generating dungeon..." << std::endl;
    m_dungeonGenerator->generate();
    std::cout << "Dungeon generated" << std::endl;

    const auto &floors = m_dungeonGenerator->getFloors();
    const auto &walls = m_dungeonGenerator->getWalls();

    if (floors.empty()) {
      std::cerr << "No floor tiles generated!" << std::endl;
      return;
    }

    // Create floor tiles
    std::cout << "Creating floor tiles..." << std::endl;
    for (const auto &floor : floors) {
      m_entityFactory->createFloor(floor.x, floor.y,
                                   Constants::Sprites::Tilesets::MAIN_TILESET);
    }
    std::cout << "Floor tiles created" << std::endl;

    // Create wall tiles with collision components
    std::cout << "Creating wall tiles..." << std::endl;
    for (const auto &wall : walls) {
      m_entityFactory->createWall(wall.x, wall.y,
                                  Constants::Sprites::Tilesets::MAIN_TILESET);
    }
    std::cout << "Wall tiles created" << std::endl;

    // Create player and mobs
    if (!floors.empty()) {
      m_entityFactory->createPlayer(floors[0].x, floors[0].y,
                                    Constants::Sprites::Tilesets::MAIN_TILESET);

      // Create 5 mobs at different positions, ensuring they have space to move
      int mobsCreated = 0;
      int attempts = 0;
      const int maxAttempts = floors.size();

      while (mobsCreated < 30 && attempts < maxAttempts) {
        // Pick a random floor tile
        int randomIndex = 1 + (rand() % (floors.size() - 1));
        const auto &floor = floors[randomIndex];

        // Count nearby floor tiles to ensure there's room to move
        int openSpaces = 0;
        for (const auto &checkFloor : floors) {
          int dx = abs(checkFloor.x - floor.x);
          int dy = abs(checkFloor.y - floor.y);
          if (dx <= 2 && dy <= 2) { // Check in a 5x5 area
            openSpaces++;
          }
        }

        // Only spawn if there are enough open spaces
        if (openSpaces >= 8) {
          // Create mob entity
          auto mobEntity = m_registry.create();

          // Add position component
          m_registry.emplace<Components::PositionComponent>(mobEntity, floor.x,
                                                            floor.y);

          // Add collision component
          m_registry.emplace<Components::CollisionComponent>(mobEntity, true);

          // Add sprite component
          m_registry.emplace<Components::SpriteComponent>(
              mobEntity, Constants::Sprites::Tilesets::MAIN_TILESET, IDs::MOB);

          // Add mob tag
          m_registry.emplace<Components::MobTag>(mobEntity);

          // Add name component
          m_registry.emplace<Components::Name>(
              mobEntity, "Enemy Mob " + std::to_string(mobsCreated + 1));

          // Add and initialize wander component with faster movement
          auto &wander =
              m_registry.emplace<Components::WanderComponent>(mobEntity);
          wander.moveCooldown = 300;
          wander.lastMoveTime = SDL_GetTicks();
          wander.direction = -1;

          mobsCreated++;
          std::cout << "Created mob " << mobsCreated << " at (" << floor.x
                    << ", " << floor.y << ") with " << openSpaces
                    << " open spaces nearby" << std::endl;
        }
        attempts++;
      }

      if (mobsCreated < 5) {
        std::cout << "Warning: Only able to create " << mobsCreated
                  << " mobs with sufficient space" << std::endl;
      }
    }
  } else {
    // For TMX maps, reload the map to ensure entities are properly created
    std::cout << "Reloading TMX map..." << std::endl;
    if (!m_gameManager.getMapLoader()->loadMap(Constants::Assets::MAP_TMX,
                                               m_registry)) {
      std::cerr << "Failed to reload TMX map!" << std::endl;
      return;
    }
    std::cout << "TMX map reloaded successfully" << std::endl;

    // For TMX maps, find suitable positions for the player and mobs
    auto floorView =
        m_registry.view<Components::PositionComponent, Components::FloorTag>();

    // Debug: Print all floor tiles in the registry
    std::cout << "Searching for floor tiles in registry..." << std::endl;

    size_t floorCount = 0;
    for (auto entity : floorView) {
      const auto &pos = floorView.get<Components::PositionComponent>(entity);
      std::cout << "Found floor tile at (" << pos.x << ", " << pos.y << ")"
                << std::endl;
      floorCount++;
    }
    std::cout << "Total floor tiles found: " << floorCount << std::endl;

    // Find the first floor tile for player
    auto it = floorView.begin();
    if (it != floorView.end()) {
      auto firstFloor = *it;
      const auto &pos =
          floorView.get<Components::PositionComponent>(firstFloor);
      std::cout << "Creating player at (" << pos.x << ", " << pos.y << ")"
                << std::endl;

      // Create player at the first floor tile position
      m_entityFactory->createPlayer(pos.x, pos.y,
                                    Constants::Sprites::Tilesets::MAIN_TILESET);

      // Create 5 mobs at different floor tiles
      int mobCount = 0;
      ++it; // Move to next floor tile
      while (it != floorView.end() && mobCount < 30) {
        auto floorTile = *it;
        const auto &mobPos =
            floorView.get<Components::PositionComponent>(floorTile);
        std::cout << "Creating mob " << (mobCount + 1) << " at (" << mobPos.x
                  << ", " << mobPos.y << ")" << std::endl;

        // Create mob entity
        auto mobEntity = m_registry.create();

        // Add position component
        m_registry.emplace<Components::PositionComponent>(mobEntity, mobPos.x,
                                                          mobPos.y);

        // Add collision component
        m_registry.emplace<Components::CollisionComponent>(mobEntity, true);

        // Add sprite component
        m_registry.emplace<Components::SpriteComponent>(
            mobEntity, Constants::Sprites::Tilesets::MAIN_TILESET, IDs::MOB);

        // Add mob tag
        m_registry.emplace<Components::MobTag>(mobEntity);

        // Add name component
        m_registry.emplace<Components::Name>(
            mobEntity, "Enemy Mob " + std::to_string(mobCount + 1));

        // Add and initialize wander component
        auto &wander =
            m_registry.emplace<Components::WanderComponent>(mobEntity);
        wander.moveCooldown = 500;
        wander.lastMoveTime = SDL_GetTicks();
        wander.direction = -1;

        ++it;
        ++mobCount;
      }
    } else {
      std::cerr << "No floor tiles found in TMX map!" << std::endl;
      return;
    }
  }

  // Update viewport and camera position
  updateViewportPosition();
  std::cout << "setupTestScenario completed" << std::endl;
}

void CollisionTest::updateViewportPosition() {
  // Center the viewport in the window
  m_viewport.w = Constants::Viewport::VIEWPORT_WIDTH;
  m_viewport.h = Constants::Viewport::VIEWPORT_HEIGHT;
  m_viewport.x = (m_currentWindowWidth - m_viewport.w) / 2;
  m_viewport.y = (m_currentWindowHeight - m_viewport.h) / 2;

  if (m_renderer) {
    m_renderer->setViewport(m_viewport);
  }
}

void CollisionTest::handleInput() {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    // Let ImGui handle its events
    ImGui_ImplSDL2_ProcessEvent(&event);

    // Handle window events
    if (event.type == SDL_QUIT) {
      m_running = false;
      return;
    }

    if (event.type == SDL_WINDOWEVENT) {
      if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
        m_currentWindowWidth = event.window.data1;
        m_currentWindowHeight = event.window.data2;
        updateViewportPosition();
      }
    }

    // Let the input handler process game-specific events
    m_inputHandler->handleInput(event, m_registry, nullptr);
  }
}

void CollisionTest::tickUpdate() {
  // Update tick count
  m_currentTick++;
  m_lastTickTime = SDL_GetTicks();

  // Update performance monitoring
  m_performanceWindow->updateTickRate(m_currentTick, m_lastTickTime);

  // Update mob wandering for all mobs using ECS collision system
  Systems::WanderSystem(m_registry, SDL_GetTicks());
}

void CollisionTest::renderFrame() {
  // Clear the entire window first
  SDL_RenderClear(m_graphicsContext->getRenderer());

  // Set the viewport for game rendering
  SDL_RenderSetViewport(m_graphicsContext->getRenderer(), &m_viewport);

  // Render game content within viewport
  m_renderer->render(m_registry);

  // Reset viewport for ImGui (full window)
  SDL_RenderSetViewport(m_graphicsContext->getRenderer(), nullptr);

  // Start ImGui frame
  ImGui_ImplSDLRenderer2_NewFrame();
  ImGui_ImplSDL2_NewFrame();
  ImGui::NewFrame();

  // Set default positions for ImGui windows around the viewport
  ImGui::SetNextWindowPos(
      ImVec2(10, 10), ImGuiCond_FirstUseEver); // Performance window top-left
  m_performanceWindow->render(m_registry);

  ImGui::SetNextWindowPos(ImVec2(m_currentWindowWidth - 300, 10),
                          ImGuiCond_FirstUseEver); // Entity inspector top-right
  m_entityInspector->render(m_registry);

  ImGui::SetNextWindowPos(ImVec2(10, m_currentWindowHeight - 300),
                          ImGuiCond_FirstUseEver); // TMX loader bottom-left
  m_tmxLoader->render(m_registry);

  ImGui::SetNextWindowPos(
      ImVec2(m_currentWindowWidth - 300, m_currentWindowHeight - 300),
      ImGuiCond_FirstUseEver); // Walker dungeon bottom-right
  m_walkerDungeon->render(m_registry);

  // Render ImGui
  ImGui::Render();
  ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(),
                                        m_graphicsContext->getRenderer());

  // Present the final frame
  SDL_RenderPresent(m_graphicsContext->getRenderer());
}

void CollisionTest::run() {
  m_lastFrameTime = SDL_GetTicks();
  m_lastTickTime = m_lastFrameTime;

  // Calculate frame time in milliseconds (60 FPS = 16.67ms per frame)
  const Uint32 targetFrameTime = 1000 / 60;

  while (m_running) {
    Uint32 frameStart = SDL_GetTicks();

    // Calculate delta time and update accumulator
    Uint32 currentTime = SDL_GetTicks();
    Uint32 deltaTime = currentTime - m_lastFrameTime;
    m_lastFrameTime = currentTime;
    m_accumulator += deltaTime;

    // Check for TMX map modifications if we're not using the dungeon generator
    if (!m_usingDungeonGenerator) {
      if (m_gameManager.getMapLoader()->isMapModified()) {
        std::cout << "TMX map file modified, reloading..." << std::endl;
        m_gameManager.getMapLoader()->reloadIfModified();
        setupTestScenario();
      }
    }

    // Handle input
    handleInput();

    // Update game logic at fixed time steps
    while (m_accumulator >= MS_PER_TICK) {
      tickUpdate();
      m_accumulator -= MS_PER_TICK;
    }

    // Render frame
    renderFrame();

    // Frame limiting
    Uint32 frameTime = SDL_GetTicks() - frameStart;
    if (frameTime < targetFrameTime) {
      SDL_Delay(targetFrameTime - frameTime);
    }
  }
}

void CollisionTest::update() {
  // Update game state
  // No need to update mobs directly as they are handled by the WanderSystem
}

void CollisionTest::regenerateDungeon() {
  std::cout << "Regenerating dungeon..." << std::endl;

  // Clear existing entities
  m_registry.clear();

  // Generate new dungeon layout using walker algorithm
  m_dungeonGenerator->generate();

  const auto &floors = m_dungeonGenerator->getFloors();
  const auto &walls = m_dungeonGenerator->getWalls();

  if (floors.empty()) {
    std::cerr << "No floor tiles generated!" << std::endl;
    return;
  }

  // Create floor tiles
  std::cout << "Creating floor tiles..." << std::endl;
  for (const auto &floor : floors) {
    m_entityFactory->createFloor(floor.x, floor.y,
                                 Constants::Sprites::Tilesets::MAIN_TILESET);
  }
  std::cout << "Floor tiles created" << std::endl;

  // Create wall tiles with collision components
  std::cout << "Creating wall tiles..." << std::endl;
  for (const auto &wall : walls) {
    m_entityFactory->createWall(wall.x, wall.y,
                                Constants::Sprites::Tilesets::MAIN_TILESET);
  }
  std::cout << "Wall tiles created" << std::endl;

  // Create player and mobs
  if (!floors.empty()) {
    m_entityFactory->createPlayer(floors[0].x, floors[0].y,
                                  Constants::Sprites::Tilesets::MAIN_TILESET);

    // Create 5 mobs at different positions, ensuring they have space to move
    int mobsCreated = 0;
    int attempts = 0;
    const int maxAttempts = floors.size();

    while (mobsCreated < 5 && attempts < maxAttempts) {
      // Pick a random floor tile
      int randomIndex = 1 + (rand() % (floors.size() - 1));
      const auto &floor = floors[randomIndex];

      // Count nearby floor tiles to ensure there's room to move
      int openSpaces = 0;
      for (const auto &checkFloor : floors) {
        int dx = abs(checkFloor.x - floor.x);
        int dy = abs(checkFloor.y - floor.y);
        if (dx <= 2 && dy <= 2) { // Check in a 5x5 area
          openSpaces++;
        }
      }

      // Only spawn if there are enough open spaces
      if (openSpaces >= 8) {
        // Create mob entity
        auto mobEntity = m_registry.create();

        // Add position component
        m_registry.emplace<Components::PositionComponent>(mobEntity, floor.x,
                                                          floor.y);

        // Add collision component
        m_registry.emplace<Components::CollisionComponent>(mobEntity, true);

        // Add sprite component
        m_registry.emplace<Components::SpriteComponent>(
            mobEntity, Constants::Sprites::Tilesets::MAIN_TILESET, IDs::MOB);

        // Add mob tag
        m_registry.emplace<Components::MobTag>(mobEntity);

        // Add name component
        m_registry.emplace<Components::Name>(
            mobEntity, "Enemy Mob " + std::to_string(mobsCreated + 1));

        // Add and initialize wander component with faster movement
        auto &wander =
            m_registry.emplace<Components::WanderComponent>(mobEntity);
        wander.moveCooldown = 300;
        wander.lastMoveTime = SDL_GetTicks();
        wander.direction = -1;

        mobsCreated++;
        std::cout << "Created mob " << mobsCreated << " at (" << floor.x << ", "
                  << floor.y << ") with " << openSpaces << " open spaces nearby"
                  << std::endl;
      }
      attempts++;
    }

    if (mobsCreated < 5) {
      std::cout << "Warning: Only able to create " << mobsCreated
                << " mobs with sufficient space" << std::endl;
    }
  }

  // Update viewport position
  updateViewportPosition();
  std::cout << "Dungeon regeneration completed" << std::endl;
}

} // namespace Examples
