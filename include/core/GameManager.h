#pragma once
#include "../events/EventBus.h"
#include "Player.h"
#include "Tile.h"
#include "TmxMapLoader.h"
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <map>
#include <vector>

class GameManager {
public:
  GameManager();
  ~GameManager();
  bool init();
  void run();
  void clean();

private:
  SDL_Window *window;
  SDL_Renderer *renderer;
  bool isRunning;

  // Tilemap integration
  TmxMapLoader mapLoader;
  std::map<std::string, SDL_Texture *> tilesetTextures;

  // Event system
  EventBus eventBus;

  std::vector<Tile> tiles;
  Player *player;

  void handleEvents();
  void update();
  void render();

  // Tilemap rendering functions
  bool loadTilesets();
  void renderLayer(int layerIndex);
  void renderMap();
  SDL_Rect getTileSourceRect(int gid, const Tmx::Tileset *tileset);

  // Event handlers setup
  void setupEventHandlers();
};