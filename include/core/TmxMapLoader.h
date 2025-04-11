#pragma once

#include "../deps/tmxparser/include/TmxMap.h"
#include "../deps/tmxparser/include/TmxTile.h"
#include "Tags.h"
#include <SDL.h>
#include <SDL_image.h>
#include <entt/entt.hpp>
#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>

#include "core/Components.h"

// Forward declarations for Tmx types
namespace Tmx {
class Layer;
class TileLayer;
class Tileset;
class ObjectGroup;
class Object;
class Tile;
} // namespace Tmx

class TmxMapLoader {
private:
  Tmx::Map map;
  bool loaded;
  std::string currentMapPath;
  std::filesystem::file_time_type lastModifiedTime;
  entt::registry *registry;
  SDL_Renderer *renderer;
  std::unordered_map<int, SDL_Texture *>
      tilesetTextures; // Map tileset index to texture
  std::unordered_map<std::string, SDL_Texture *>
      namedTilesetTextures; // Map tileset name to texture

public:
  TmxMapLoader(SDL_Renderer *renderer);
  ~TmxMapLoader();

  void setRenderer(SDL_Renderer *renderer) { this->renderer = renderer; }
  bool loadMap(const std::string &path, entt::registry &registry);
  bool reloadIfModified();
  bool isMapModified() const;
  bool isLoaded() const { return loaded; }

  // Print methods
  void printMapInfo() const;
  void printLayerInfo() const;
  void printTilesetInfo() const;
  void printObjectGroups() const;

  // Getters
  int getWidth() const;
  int getHeight() const;
  int getTileWidth() const;
  int getTileHeight() const;
  int getNumLayers() const;
  int getNumTilesets() const;

  const Tmx::Layer *getLayer(int index) const;
  const Tmx::Tileset *getTileset(int index) const;
  const Tmx::Map &getMap() const;

  std::string getLayerName(int index) const;
  std::vector<int> getTileGIDsFromLayer(int index) const;

  const Tmx::ObjectGroup *getObjectGroup(const std::string &name) const;
  const Tmx::ObjectGroup *getObjectGroup(int index) const;
  std::vector<const Tmx::Object *>
  getObjectsByType(const std::string &type) const;

  // Collision checking
  bool isTileCollidable(int gid) const;

  // Collision checking
  bool isTileFloor(int gid) const;

  // Create collider entities from a tile layer
  void createColliderEntities(entt::registry &registry, int layerIndex,
                              int tileWidth, int tileHeight) const;

  // Rendering
  void render(SDL_Renderer *renderer, int offsetX = 0, int offsetY = 0);
  SDL_Rect getTileRect(int tileIndex, int tilesetCols = 4,
                       int tilesetRows = 4) const;
  SDL_Texture *getTilesetTexture(const std::string &tilesetName) const;
  void setTilesetTexture(const std::string &tilesetName, SDL_Texture *texture);
};