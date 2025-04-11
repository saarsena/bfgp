#pragma once
#include "../events/EventBus.h"
#include "Player.h"
#include "Tile.h"
#include "TmxMapLoader.h"
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <entt/entt.hpp>
#include <map>
#include <unordered_map>
#include <vector>

class GameManager {
public:
  GameManager();
  ~GameManager();

  bool init();
  bool init(SDL_Window *existingWindow, SDL_Renderer *existingRenderer);
  void setWindow(SDL_Window *window) { this->window = window; }
  void setRenderer(SDL_Renderer *renderer) {
    this->renderer = renderer;
    if (mapLoader) {
      mapLoader->setRenderer(renderer);
    }
  }
  void run();
  void clean();
  void handleEvents();
  void update();
  void render();
  bool isGameRunning() const { return isRunning; }
  SDL_Window *getWindow() const { return window; }
  SDL_Renderer *getRenderer() const { return renderer; }
  TmxMapLoader *getMapLoader() const { return mapLoader; }

private:
  void setupEventHandlers();
  bool loadTilesets();
  SDL_Rect getTileSourceRect(int gid, const Tmx::Tileset *tileset);
  void renderLayer(int layerIndex);
  void renderMap();

  SDL_Window *window;
  SDL_Renderer *renderer;
  bool isRunning;
  TmxMapLoader *mapLoader;
  Player *player;
  std::unordered_map<std::string, SDL_Texture *> tilesetTextures;
  EventBus eventBus;
};
