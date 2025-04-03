#pragma once

#include "TmxMap.h"
#include <filesystem>
#include <string>
#include <vector>

// Forward declarations for Tmx types
namespace Tmx {
class Layer;
class TileLayer;
class Tileset;
class ObjectGroup;
class Object;
} // namespace Tmx

class TmxMapLoader {
private:
  Tmx::Map map;
  bool loaded;
  std::string currentMapPath;
  std::filesystem::file_time_type lastModifiedTime;

public:
  TmxMapLoader();
  ~TmxMapLoader();

  bool loadMap(const std::string &filepath);
  bool isLoaded() const;
  std::string getCurrentMapPath() const;
  bool isMapModified() const;
  bool reloadIfModified();

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
};