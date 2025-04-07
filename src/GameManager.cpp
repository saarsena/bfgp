#include "../include/core/GameManager.h"
#include "../include/events/GameEvent.h"
#include "TmxImage.h"
#include "TmxLayer.h"
#include "TmxObject.h"
#include "TmxObjectGroup.h"
#include "TmxTileLayer.h"
#include "TmxTileset.h"
#include <iostream>

GameManager::GameManager()
    : window(nullptr), renderer(nullptr), isRunning(false), player(nullptr) {}

GameManager::~GameManager() { clean(); }

void GameManager::setupEventHandlers() {
  // Log player movement events
  eventBus.subscribe(GameEventType::PLAYER_MOVED, [](const GameEvent &event) {
    const auto &e = static_cast<const PlayerMovedEvent &>(event);
    std::cout << "🟢 Player moved to: (" << e.newX << ", " << e.newY << ")\n";
  });

  // You can add more event handlers here
}

bool GameManager::init() {
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
    return false;
  if (!IMG_Init(IMG_INIT_PNG))
    return false;
  if (TTF_Init() == -1)
    return false;
  if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    return false;

  // Load the map first to determine window size
  if (!mapLoader.loadMap("assets/tg_tiled_example.tmx")) {
    std::cerr << "Failed to load map!" << std::endl;
    return false;
  }

  // Calculate window size based on map dimensions
  int mapWidth = mapLoader.getWidth() * mapLoader.getTileWidth();
  int mapHeight = mapLoader.getHeight() * mapLoader.getTileHeight();

  window = SDL_CreateWindow("Tiny Galaxy", SDL_WINDOWPOS_CENTERED,
                            SDL_WINDOWPOS_CENTERED, mapWidth, mapHeight, 0);
  if (!window)
    return false;

  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (!renderer)
    return false;

  // Load tileset textures
  if (!loadTilesets()) {
    std::cerr << "Failed to load tilesets!" << std::endl;
    return false;
  }

  // Find player spawn location from the map
  float spawnX = 5; // Default position if spawn point not found
  float spawnY = 5;

  // Search for object with name "player spawn" or type "player_spawn"
  bool foundSpawn = false;
  for (int i = 0; i < mapLoader.getMap().GetNumObjectGroups(); ++i) {
    const Tmx::ObjectGroup *group = mapLoader.getMap().GetObjectGroup(i);
    for (int j = 0; j < group->GetNumObjects(); ++j) {
      const Tmx::Object *obj = group->GetObject(j);
      if (obj->GetName() == "player spawn" ||
          obj->GetType() == "player_spawn" || obj->GetId() == 1) {
        spawnX = obj->GetX() / mapLoader.getTileWidth();
        spawnY = obj->GetY() / mapLoader.getTileHeight();
        foundSpawn = true;
        std::cout << "Found player spawn at tile position: " << spawnX << ","
                  << spawnY << std::endl;
        break;
      }
    }
    if (foundSpawn)
      break;
  }

  // Create player texture from tile ID 12 in the monsters tileset
  SDL_Texture *playerTexture = nullptr;

  // Find the monsters tileset specifically
  const Tmx::Tileset *monstersTileset = nullptr;
  for (int i = 0; i < mapLoader.getNumTilesets(); ++i) {
    const Tmx::Tileset *tileset = mapLoader.getTileset(i);
    if (tileset->GetName() == "tiny_galaxy_monsters") {
      monstersTileset = tileset;
      break;
    }
  }

  if (monstersTileset) {
    // Get the texture for the monsters tileset
    SDL_Texture *tilesetTexture = tilesetTextures["tiny_galaxy_monsters"];
    if (tilesetTexture) {
      // We want local ID 12 from the monsters tileset
      int localId = 12;
      int tilesetWidth = monstersTileset->GetImage()->GetWidth() /
                         monstersTileset->GetTileWidth();

      // Calculate source rectangle for the tile
      SDL_Rect src;
      src.x = (localId % tilesetWidth) * monstersTileset->GetTileWidth();
      src.y = (localId / tilesetWidth) * monstersTileset->GetTileHeight();
      src.w = monstersTileset->GetTileWidth();
      src.h = monstersTileset->GetTileHeight();

      // Set up a render target for the player texture
      SDL_Texture *tempTexture =
          SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
                            SDL_TEXTUREACCESS_TARGET, src.w, src.h);

      // Enable alpha blending on the texture
      SDL_SetTextureBlendMode(tempTexture, SDL_BLENDMODE_BLEND);

      // Copy the tile to the player texture
      SDL_SetRenderTarget(renderer, tempTexture);
      SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
      SDL_RenderClear(renderer);
      SDL_RenderCopy(renderer, tilesetTexture, &src, nullptr);
      SDL_SetRenderTarget(renderer, nullptr);

      playerTexture = tempTexture;
      std::cout << "Created player texture from monsters tileset, local ID 12"
                << std::endl;
    }
  } else {
    std::cout << "Could not find monsters tileset!" << std::endl;
  }

  // Position player at the spawn location
  player = new Player(static_cast<int>(spawnX), static_cast<int>(spawnY),
                      playerTexture, this->registry, mapLoader.getTileWidth());

  // Set up event handlers
  setupEventHandlers();

  isRunning = true;
  return true;
}

bool GameManager::loadTilesets() {
  for (int i = 0; i < mapLoader.getNumTilesets(); ++i) {
    const Tmx::Tileset *tileset = mapLoader.getTileset(i);
    std::string imagePath = "assets/" + tileset->GetImage()->GetSource();

    SDL_Surface *tempSurface = IMG_Load(imagePath.c_str());
    if (!tempSurface) {
      std::cerr << "Failed to load tileset image: " << imagePath << std::endl;
      return false;
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, tempSurface);
    SDL_FreeSurface(tempSurface);

    if (!texture) {
      std::cerr << "Failed to create texture from tileset: " << imagePath
                << std::endl;
      return false;
    }

    tilesetTextures[tileset->GetName()] = texture;
  }

  return true;
}

SDL_Rect GameManager::getTileSourceRect(int gid, const Tmx::Tileset *tileset) {
  SDL_Rect src;

  // Adjust gid to be local to this tileset
  int localId = gid - tileset->GetFirstGid();

  // Calculate tile position in the tileset image
  int tilesetWidth = tileset->GetImage()->GetWidth() / tileset->GetTileWidth();

  src.x = (localId % tilesetWidth) * tileset->GetTileWidth();
  src.y = (localId / tilesetWidth) * tileset->GetTileHeight();
  src.w = tileset->GetTileWidth();
  src.h = tileset->GetTileHeight();

  return src;
}

void GameManager::renderLayer(int layerIndex) {
  const Tmx::Layer *layer = mapLoader.getLayer(layerIndex);

  // Skip non-tile layers or invisible layers
  if (layer->GetLayerType() != Tmx::TMX_LAYERTYPE_TILE || !layer->IsVisible()) {
    return;
  }

  const Tmx::TileLayer *tileLayer = static_cast<const Tmx::TileLayer *>(layer);
  int width = layer->GetWidth();
  int height = layer->GetHeight();

  // Don't set global opacity, we'll handle it per texture when needed
  float opacity = layer->GetOpacity();

  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      int gid = tileLayer->GetTileGid(x, y);

      // Skip empty tiles
      if (gid == 0)
        continue;

      // Find which tileset this tile belongs to
      const Tmx::Tileset *tileset = nullptr;
      for (int i = mapLoader.getNumTilesets() - 1; i >= 0; --i) {
        const Tmx::Tileset *ts = mapLoader.getTileset(i);
        if (gid >= ts->GetFirstGid()) {
          tileset = ts;
          break;
        }
      }

      if (!tileset)
        continue;

      // Get the texture for this tileset
      SDL_Texture *texture = tilesetTextures[tileset->GetName()];
      if (!texture)
        continue;

      // Apply layer opacity to texture
      SDL_SetTextureAlphaMod(texture, static_cast<Uint8>(opacity * 255));

      // Set the source rectangle for this tile
      SDL_Rect src = getTileSourceRect(gid, tileset);

      // Set the destination rectangle
      SDL_Rect dest = {x * tileset->GetTileWidth(),
                       y * tileset->GetTileHeight(), tileset->GetTileWidth(),
                       tileset->GetTileHeight()};

      // Draw the tile
      SDL_RenderCopy(renderer, texture, &src, &dest);
    }
  }

  // Reset opacity for all textures
  for (auto &[name, texture] : tilesetTextures) {
    SDL_SetTextureAlphaMod(texture, 255);
  }
}

void GameManager::renderMap() {
  // Draw all layers in order
  for (int i = 0; i < mapLoader.getNumLayers(); ++i) {
    renderLayer(i);
  }
}

void GameManager::run() {
  while (isRunning) {
    handleEvents();
    update();
    render();
    SDL_Delay(16);
  }
}

void GameManager::handleEvents() {
  SDL_Event e;
  while (SDL_PollEvent(&e)) {
    if (e.type == SDL_QUIT)
      isRunning = false;
    player->handleInput(e, eventBus);
  }
}

void GameManager::update() {
  player->update();

  // Check for map hot reloading
  if (mapLoader.isMapModified() && mapLoader.reloadIfModified()) {
    std::cout << "Map was hot-reloaded!" << std::endl;
    // Reload tilesets if needed
    loadTilesets();
  }
}

void GameManager::render() {
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);

  // Render the map
  renderMap();

  // Render the player on top
  player->render(renderer);

  SDL_RenderPresent(renderer);
}

void GameManager::clean() {
  // Free tileset textures
  for (auto &[name, texture] : tilesetTextures) {
    SDL_DestroyTexture(texture);
  }
  tilesetTextures.clear();

  delete player;
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  Mix_Quit();
  TTF_Quit();
  IMG_Quit();
  SDL_Quit();
}
