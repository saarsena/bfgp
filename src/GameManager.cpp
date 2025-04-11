#include "../include/core/GameManager.h"
#include "../deps/tmxparser/include/TmxImage.h"
#include "../deps/tmxparser/include/TmxLayer.h"
#include "../deps/tmxparser/include/TmxObject.h"
#include "../deps/tmxparser/include/TmxObjectGroup.h"
#include "../deps/tmxparser/include/TmxTileLayer.h"
#include "../deps/tmxparser/include/TmxTileset.h"
#include "../include/events/GameEvent.h"
#include <iostream>

GameManager::GameManager()
    : window(nullptr), renderer(nullptr), isRunning(false), mapLoader(nullptr),
      player(nullptr) {
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
    std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
    return;
  }

  // Initialize SDL_image
  if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
    std::cerr << "SDL_image initialization failed: " << IMG_GetError()
              << std::endl;
    return;
  }

  // Initialize SDL_ttf
  if (TTF_Init() < 0) {
    std::cerr << "SDL_ttf initialization failed: " << TTF_GetError()
              << std::endl;
    return;
  }

  // Initialize SDL_mixer
  if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
    std::cerr << "SDL_mixer initialization failed: " << Mix_GetError()
              << std::endl;
    return;
  }

  setupEventHandlers();
  isRunning = true;
}

GameManager::~GameManager() { clean(); }

void GameManager::setupEventHandlers() {
  // Log player movement events
  eventBus.subscribe(GameEventType::PLAYER_MOVED, [](const GameEvent &event) {
    const auto &e = static_cast<const PlayerMovedEvent &>(event);
    std::cout << "🟢 Player moved to: (" << e.newX << ", " << e.newY << ")\n";
  });

  // You can add more event handlers here
}

bool GameManager::init(SDL_Window *existingWindow,
                       SDL_Renderer *existingRenderer) {
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
    return false;
  if (!IMG_Init(IMG_INIT_PNG))
    return false;
  if (TTF_Init() == -1)
    return false;
  if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    return false;

  window = existingWindow;
  renderer = existingRenderer;

  // Initialize TmxMapLoader with the renderer
  mapLoader = new TmxMapLoader(renderer);

  // Set up event handlers
  setupEventHandlers();

  isRunning = true;
  return true;
}

bool GameManager::loadTilesets() {
  for (int i = 0; i < mapLoader->getNumTilesets(); ++i) {
    const Tmx::Tileset *tileset = mapLoader->getTileset(i);
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
  const Tmx::Layer *layer = mapLoader->getLayer(layerIndex);

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
      for (int i = mapLoader->getNumTilesets() - 1; i >= 0; --i) {
        const Tmx::Tileset *ts = mapLoader->getTileset(i);
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
  for (int i = 0; i < mapLoader->getNumLayers(); ++i) {
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
  if (mapLoader->isMapModified() && mapLoader->reloadIfModified()) {
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
  // Clean up textures
  for (auto &[name, texture] : tilesetTextures) {
    if (texture) {
      SDL_DestroyTexture(texture);
    }
  }
  tilesetTextures.clear();

  delete player;
  delete mapLoader;

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);

  Mix_Quit();
  TTF_Quit();
  IMG_Quit();
  SDL_Quit();
}
