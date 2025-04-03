#include "TmxMapLoader.h"

#include <algorithm>
#include <iostream>

#include "TmxLayer.h"
#include "TmxObject.h"
#include "TmxObjectGroup.h"
#include "TmxTileLayer.h"
#include "TmxTileset.h"

TmxMapLoader::TmxMapLoader() : loaded(false) {}

TmxMapLoader::~TmxMapLoader() {
  // No need to delete map as it's not a pointer anymore
}

bool TmxMapLoader::loadMap(const std::string &filepath) {
  if (!std::filesystem::exists(filepath)) {
    std::cerr << "File not found: " << filepath << std::endl;
    loaded = false;
    return false;
  }

  // Reset map to clear previous state
  map = Tmx::Map();

  map.ParseFile(filepath);

  if (map.HasError()) {
    std::cerr << "Failed to load TMX map: " << map.GetErrorText() << std::endl;
    loaded = false;
    return false;
  }

  currentMapPath = filepath;
  lastModifiedTime = std::filesystem::last_write_time(filepath);
  loaded = true;
  return true;
}

bool TmxMapLoader::isLoaded() const { return loaded; }

std::string TmxMapLoader::getCurrentMapPath() const { return currentMapPath; }

bool TmxMapLoader::isMapModified() const {
  if (!loaded || currentMapPath.empty())
    return false;

  try {
    auto currentTime = std::filesystem::last_write_time(currentMapPath);
    return currentTime > lastModifiedTime;
  } catch (const std::exception &) {
    return false;
  }
}

bool TmxMapLoader::reloadIfModified() {
  if (isMapModified()) {
    return loadMap(currentMapPath);
  }
  return false;
}

void TmxMapLoader::printMapInfo() const {
  if (!loaded) {
    std::cerr << "No map loaded" << std::endl;
    return;
  }

  std::cout << "Map size: " << map.GetWidth() << " x " << map.GetHeight()
            << std::endl;
  std::cout << "Tile size: " << map.GetTileWidth() << " x "
            << map.GetTileHeight() << std::endl;
}

void TmxMapLoader::printLayerInfo() const {
  if (!loaded)
    return;

  for (int i = 0; i < map.GetNumLayers(); ++i) {
    const Tmx::Layer *layer = map.GetLayer(i);
    std::cout << "Layer " << i << ": " << layer->GetName() << " ("
              << layer->GetWidth() << " x " << layer->GetHeight() << ")"
              << std::endl;
  }
}

void TmxMapLoader::printTilesetInfo() const {
  if (!loaded)
    return;

  for (int i = 0; i < map.GetNumTilesets(); ++i) {
    const Tmx::Tileset *tileset = map.GetTileset(i);
    std::cout << "Tileset " << i << ": " << tileset->GetName()
              << " (Tile size: " << tileset->GetTileWidth() << " x "
              << tileset->GetTileHeight() << ")" << std::endl;
  }
}

void TmxMapLoader::printObjectGroups() const {
  if (!loaded)
    return;

  std::cout << "\nObject Groups:\n";
  for (int i = 0; i < map.GetNumObjectGroups(); ++i) {
    const auto *group = map.GetObjectGroup(i);
    std::cout << "  - " << group->GetName() << " (" << group->GetNumObjects()
              << " objects)\n";
  }
}

int TmxMapLoader::getWidth() const { return map.GetWidth(); }

int TmxMapLoader::getHeight() const { return map.GetHeight(); }

int TmxMapLoader::getTileWidth() const { return map.GetTileWidth(); }

int TmxMapLoader::getTileHeight() const { return map.GetTileHeight(); }

int TmxMapLoader::getNumLayers() const { return map.GetNumLayers(); }

int TmxMapLoader::getNumTilesets() const { return map.GetNumTilesets(); }

const Tmx::Layer *TmxMapLoader::getLayer(int index) const {
  return map.GetLayer(index);
}

const Tmx::Tileset *TmxMapLoader::getTileset(int index) const {
  return map.GetTileset(index);
}

const Tmx::Map &TmxMapLoader::getMap() const { return map; }

std::string TmxMapLoader::getLayerName(int index) const {
  return getLayer(index)->GetName();
}

std::vector<int> TmxMapLoader::getTileGIDsFromLayer(int index) const {
  std::vector<int> gids;
  if (!loaded || index < 0 || index >= map.GetNumLayers()) {
    return gids;
  }

  const Tmx::Layer *layer = getLayer(index);
  // Check if it's a tile layer
  if (layer->GetLayerType() == Tmx::TMX_LAYERTYPE_TILE) {
    // Cast to tile layer
    const Tmx::TileLayer *tileLayer =
        dynamic_cast<const Tmx::TileLayer *>(layer);
    if (tileLayer) {
      for (int y = 0; y < layer->GetHeight(); ++y) {
        for (int x = 0; x < layer->GetWidth(); ++x) {
          gids.push_back(tileLayer->GetTileGid(x, y));
        }
      }
    }
  }

  return gids;
}

const Tmx::ObjectGroup *
TmxMapLoader::getObjectGroup(const std::string &name) const {
  if (!loaded)
    return nullptr;

  for (int i = 0; i < map.GetNumObjectGroups(); ++i) {
    const Tmx::ObjectGroup *group = map.GetObjectGroup(i);
    if (group && group->GetName() == name) {
      return group;
    }
  }

  return nullptr;
}

const Tmx::ObjectGroup *TmxMapLoader::getObjectGroup(int index) const {
  if (!loaded || index < 0 || index >= map.GetNumObjectGroups())
    return nullptr;
  return map.GetObjectGroup(index);
}

std::vector<const Tmx::Object *>
TmxMapLoader::getObjectsByType(const std::string &type) const {
  std::vector<const Tmx::Object *> results;
  if (!loaded)
    return results;

  for (int i = 0; i < map.GetNumObjectGroups(); ++i) {
    const Tmx::ObjectGroup *group = map.GetObjectGroup(i);
    if (!group)
      continue;

    for (int j = 0; j < group->GetNumObjects(); ++j) {
      const Tmx::Object *obj = group->GetObject(j);
      if (obj && obj->GetType() == type) {
        results.push_back(obj);
      }
    }
  }

  return results;
}