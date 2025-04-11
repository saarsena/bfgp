#include "../include/core/TmxMapLoader.h"
#include "../include/core/Components.h"

#include <algorithm>
#include <iostream>

#include "../deps/tmxparser/include/TmxLayer.h"
#include "../deps/tmxparser/include/TmxObject.h"
#include "../deps/tmxparser/include/TmxObjectGroup.h"
#include "../deps/tmxparser/include/TmxTileLayer.h"
#include "../deps/tmxparser/include/TmxTileset.h"

TmxMapLoader::TmxMapLoader(SDL_Renderer *r) : loaded(false), renderer(r) {}

TmxMapLoader::~TmxMapLoader() {
  // Clean up textures
  for (auto &[index, texture] : tilesetTextures) {
    if (texture) {
      SDL_DestroyTexture(texture);
    }
  }
  tilesetTextures.clear();
}

bool TmxMapLoader::loadMap(const std::string &filepath, entt::registry &reg) {
  std::cout << "TmxMapLoader: Loading map from " << filepath << std::endl;

  // Store registry pointer
  registry = &reg;

  // Clean up existing textures
  for (auto &[index, texture] : tilesetTextures) {
    if (texture) {
      SDL_DestroyTexture(texture);
    }
  }
  tilesetTextures.clear();

  if (!std::filesystem::exists(filepath)) {
    std::cerr << "TmxMapLoader: File not found: " << filepath << std::endl;
    loaded = false;
    return false;
  }

  // Reset map to clear previous state
  map = Tmx::Map();

  // Get the directory containing the map file
  std::filesystem::path mapPath(filepath);
  std::string basePath = mapPath.parent_path().string();
  if (!basePath.empty() && basePath.back() != '/') {
    basePath += '/';
  }

  std::cout << "TmxMapLoader: Base path: " << basePath << std::endl;

  map.ParseFile(filepath);

  if (map.HasError()) {
    std::cerr << "TmxMapLoader: Failed to load TMX map: " << map.GetErrorText()
              << std::endl;
    loaded = false;
    return false;
  }

  currentMapPath = filepath;
  lastModifiedTime = std::filesystem::last_write_time(filepath);
  loaded = true;

  // Load all tileset textures
  for (int i = 0; i < map.GetNumTilesets(); ++i) {
    const Tmx::Tileset *tileset = map.GetTileset(i);
    if (!tileset)
      continue;

    // Get the image source path
    std::string imageSource = tileset->GetImage()->GetSource();
    std::string imagePath = basePath + imageSource;

    std::cout << "TmxMapLoader: Loading tileset " << i << ": "
              << tileset->GetName() << std::endl;
    std::cout << "  Image path: " << imagePath << std::endl;

    SDL_Texture *texture = IMG_LoadTexture(renderer, imagePath.c_str());
    if (!texture) {
      std::cerr << "TmxMapLoader: Failed to load tileset texture: "
                << SDL_GetError() << std::endl;
      continue;
    }
    tilesetTextures[i] = texture;
  }

  // Create collider entities for each layer
  for (int i = 0; i < map.GetNumLayers(); ++i) {
    createColliderEntities(*registry, i, map.GetTileWidth(),
                           map.GetTileHeight());
  }

  std::cout << "TmxMapLoader: Map loaded successfully" << std::endl;
  return true;
}

bool TmxMapLoader::isMapModified() const {
  if (!loaded || currentMapPath.empty()) {
    return false;
  }

  try {
    auto currentTime = std::filesystem::last_write_time(currentMapPath);
    bool modified = currentTime != lastModifiedTime;
    if (modified) {
      std::cout << "TmxMapLoader: Map file has been modified" << std::endl;
    }
    return modified;
  } catch (const std::filesystem::filesystem_error &e) {
    std::cerr << "TmxMapLoader: Error checking file modification time: "
              << e.what() << std::endl;
    return false;
  }
}

bool TmxMapLoader::reloadIfModified() {
  if (!loaded || !registry) {
    std::cerr
        << "TmxMapLoader: Cannot reload - no map loaded or no registry set"
        << std::endl;
    return false;
  }

  if (!isMapModified()) {
    return false;
  }

  std::cout << "TmxMapLoader: Reloading modified map..." << std::endl;

  // Store the current map path
  std::string mapPath = currentMapPath;

  // Reset loaded state and clear registry
  loaded = false;
  registry->clear();

  // Attempt to load the map again
  return loadMap(mapPath, *registry);
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

/**
 * @brief Checks if a tile is a floor tile based on its metadata properties
 *
 * CURRENT IMPLEMENTATION:
 * Currently checks for a "Floor" property on individual tiles. Map creators
 * need to set this property to true for each floor tile in their tileset.
 *
 * TODO: Future Implementation
 * This implementation will be replaced with a layer-based approach where floor
 * tiles will be identified by their layer (e.g., "Floor", "Ground") rather than
 * individual tile properties. This will:
 * - Improve performance by checking layer type instead of individual tile
 * properties
 * - Simplify map creation by using logical layer organization
 * - Follow common TMX mapping conventions
 * - Enable easier management of multiple floor types
 *
 * @param gid The global tile ID to check
 * @return true if the tile is marked as a floor tile
 */
bool TmxMapLoader::isTileFloor(int gid) const {
  if (!loaded)
    return false;

  for (int i = 0; i < map.GetNumTilesets(); ++i) {
    const Tmx::Tileset *tileset = map.GetTileset(i);
    if (!tileset)
      continue;

    int firstGid = tileset->GetFirstGid();
    int tileCount = tileset->GetTileCount();

    if (gid >= firstGid && gid < firstGid + tileCount) {
      const Tmx::Tile *tile = tileset->GetTile(gid - firstGid);
      if (tile) {
        return tile->GetProperties().HasProperty("Floor") &&
               tile->GetProperties().GetBoolProperty("Floor");
      }
    }
  }

  return false;
}

bool TmxMapLoader::isTileCollidable(int gid) const {
  if (!loaded)
    return false;

  for (int i = 0; i < map.GetNumTilesets(); ++i) {
    const Tmx::Tileset *tileset = map.GetTileset(i);
    if (!tileset)
      continue;

    int firstGid = tileset->GetFirstGid();
    int tileCount = tileset->GetTileCount();

    if (gid >= firstGid && gid < firstGid + tileCount) {
      const Tmx::Tile *tile = tileset->GetTile(gid - firstGid);
      if (tile) {
        return tile->GetProperties().HasProperty("Collider") &&
               tile->GetProperties().GetBoolProperty("Collider");
      }
    }
  }

  return false;
}

void TmxMapLoader::createColliderEntities(entt::registry &registry,
                                          int layerIndex, int tileWidth,
                                          int tileHeight) const {
  if (!loaded)
    return;

  const Tmx::Layer *layer = getLayer(layerIndex);
  if (!layer)
    return;

  // Skip decoration and non-collidable layers
  std::string name = layer->GetName();
  if (name == "Decorations" || name.find("NoCollide") != std::string::npos)
    return;

  std::cout << "Processing layer: " << name << std::endl;

  const Tmx::TileLayer *tileLayer = dynamic_cast<const Tmx::TileLayer *>(layer);
  if (!tileLayer)
    return;

  int floorTilesFound = 0;
  for (int y = 0; y < tileLayer->GetHeight(); ++y) {
    for (int x = 0; x < tileLayer->GetWidth(); ++x) {
      int gid = tileLayer->GetTileGid(x, y);
      if (gid == 0)
        continue; // Skip empty tiles

      // Find which tileset this tile belongs to
      const Tmx::Tileset *tileset = nullptr;
      for (int i = map.GetNumTilesets() - 1; i >= 0; --i) {
        const Tmx::Tileset *ts = map.GetTileset(i);
        if (ts && gid >= ts->GetFirstGid()) {
          tileset = ts;
          break;
        }
      }

      if (!tileset)
        continue;

      // Create entity
      auto entity = registry.create();
      registry.emplace<Components::PositionComponent>(entity, x, y);

      // Calculate local tile ID
      int localTileId = gid - tileset->GetFirstGid();

      // Add sprite component with tileset name and local tile ID
      registry.emplace<Components::SpriteComponent>(entity, tileset->GetName(),
                                                    localTileId);

      // Check if this is a floor tile
      bool isFloorByName =
          name == "Floor" || name.find("Floor") != std::string::npos;
      bool isFloorByProperty = isTileFloor(gid);

      if (isFloorByName || isFloorByProperty) {
        registry.emplace<Components::FloorTag>(entity);
        floorTilesFound++;
        std::cout << "Floor tile found at (" << x << ", " << y
                  << ") - By Name: " << isFloorByName
                  << ", By Property: " << isFloorByProperty << std::endl;
      }

      if (isTileCollidable(gid)) {
        registry.emplace<Components::CollisionComponent>(entity, true);
        registry.emplace<Components::TileColliderTag>(entity);
      }
    }
  }

  std::cout << "Total floor tiles found in layer " << name << ": "
            << floorTilesFound << std::endl;
}

SDL_Rect TmxMapLoader::getTileRect(int tileIndex, int tilesetCols,
                                   int tilesetRows) const {
  // Already 0-based index, no need to adjust
  // Calculate row and column in the grid
  int row = tileIndex / tilesetCols;
  int col = tileIndex % tilesetCols;
  return {col * getTileWidth(), row * getTileHeight(), getTileWidth(),
          getTileHeight()};
}

SDL_Texture *
TmxMapLoader::getTilesetTexture(const std::string &tilesetName) const {
  auto it = namedTilesetTextures.find(tilesetName);
  if (it != namedTilesetTextures.end()) {
    return it->second;
  }
  return nullptr;
}

void TmxMapLoader::setTilesetTexture(const std::string &tilesetName,
                                     SDL_Texture *texture) {
  namedTilesetTextures[tilesetName] = texture;
}

void TmxMapLoader::render(SDL_Renderer *renderer, int offsetX, int offsetY) {
  if (!loaded || !renderer)
    return;

  // Render each layer
  for (int i = 0; i < map.GetNumLayers(); ++i) {
    const Tmx::Layer *layer = map.GetLayer(i);
    if (layer->GetLayerType() != Tmx::TMX_LAYERTYPE_TILE)
      continue;

    const Tmx::TileLayer *tileLayer =
        dynamic_cast<const Tmx::TileLayer *>(layer);
    if (!tileLayer)
      continue;

    // Render each tile
    for (int y = 0; y < tileLayer->GetHeight(); ++y) {
      for (int x = 0; x < tileLayer->GetWidth(); ++x) {
        int gid = tileLayer->GetTileGid(x, y);
        if (gid == 0)
          continue; // Skip empty tiles

        // Find the correct tileset for this tile
        const Tmx::Tileset *tileset = nullptr;
        for (int t = map.GetNumTilesets() - 1; t >= 0; --t) {
          const Tmx::Tileset *ts = map.GetTileset(t);
          if (ts && gid >= ts->GetFirstGid()) {
            tileset = ts;
            break;
          }
        }
        if (!tileset)
          continue;

        // Get the tileset texture by name
        SDL_Texture *tilesetTexture = getTilesetTexture(tileset->GetName());
        if (!tilesetTexture)
          continue;

        // Calculate source and destination rectangles
        SDL_Rect srcRect = getTileRect(
            gid - tileset->GetFirstGid(), tileset->GetColumns(),
            tileset->GetImage()->GetHeight() / tileset->GetTileHeight());
        SDL_Rect dstRect = {x * getTileWidth() + offsetX,
                            y * getTileHeight() + offsetY, getTileWidth(),
                            getTileHeight()};

        // Render the tile
        SDL_RenderCopy(renderer, tilesetTexture, &srcRect, &dstRect);
      }
    }
  }
}
