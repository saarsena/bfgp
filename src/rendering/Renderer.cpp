#include "../../include/rendering/Renderer.hpp"
#include "../../deps/tmxparser/include/TmxTileset.h"
#include "../../externals/imgui/imgui.h"

namespace Rendering {

Renderer::Renderer(GameManager *gameManager) : m_gameManager(gameManager) {}

  void Renderer::render(entt::registry &registry) {
  SDL_Renderer *renderer = m_gameManager->getRenderer();

  // Clear the viewport area with a different color to make it visible
  SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255);
  SDL_Rect clearRect = {0, 0, m_viewport.w, m_viewport.h};
  SDL_RenderFillRect(renderer, &clearRect);

  // Calculate camera offset
  int offsetX = 0, offsetY = 0;
  calculateCameraOffset(registry, offsetX, offsetY);

  // Render game entities
  renderGameEntities(registry, offsetX, offsetY);

  // Draw viewport border
  SDL_SetRenderDrawColor(renderer, 64, 64, 64, 255);
  SDL_RenderDrawRect(renderer, &clearRect);
}

void Renderer::calculateCameraOffset(entt::registry &registry, int &offsetX,
                                     int &offsetY) {
  auto playerView =
      registry.view<Components::PositionComponent, Components::PlayerMarker>();
  for (auto entity : playerView) {
    const auto &pos = playerView.get<Components::PositionComponent>(entity);
    offsetX =
        (GAMEPLAY_WIDTH / 2) -
        (pos.x * m_gameManager->getMapLoader()->getTileWidth() * m_cameraScale);
    offsetY = (GAMEPLAY_HEIGHT / 2) -
              (pos.y * m_gameManager->getMapLoader()->getTileHeight() *
               m_cameraScale);
    break; // Only use the first player entity
  }
}

void Renderer::renderGameEntities(entt::registry &registry, int offsetX,
                                  int offsetY) {
  const int tileWidth = m_gameManager->getMapLoader()->getTileWidth();
  const int tileHeight = m_gameManager->getMapLoader()->getTileHeight();

  // First render floor tiles
  auto floorView =
      registry.view<Components::PositionComponent, Components::SpriteComponent,
                    Components::FloorTag>();
  for (auto entity : floorView) {
    const auto &pos = floorView.get<Components::PositionComponent>(entity);
    const auto &sprite = floorView.get<Components::SpriteComponent>(entity);

    SDL_Texture *texture =
        m_gameManager->getMapLoader()->getTilesetTexture(sprite.tilesetName);
    if (!texture)
      continue;

    // Calculate source rectangle from tile ID
    int tilesPerRow = 16; // Default for our tilesets

    // If we have a loaded TMX map, get the tileset columns
    if (m_gameManager->getMapLoader()->isLoaded()) {
      const Tmx::Tileset *tileset = nullptr;
      for (int i = 0; i < m_gameManager->getMapLoader()->getNumTilesets();
           ++i) {
        tileset = m_gameManager->getMapLoader()->getTileset(i);
        if (tileset && tileset->GetName() == sprite.tilesetName) {
          tilesPerRow = tileset->GetColumns();
          break;
        }
      }
    }

    // Use sprite ID directly (0-based indexing for the sprite sheet)
    int tileId = sprite.tileId;
    SDL_Rect srcRect = {(tileId % tilesPerRow) * tileWidth,
                        (tileId / tilesPerRow) * tileHeight, tileWidth,
                        tileHeight};

    SDL_Rect dstRect = {
        static_cast<int>(pos.x * tileWidth * m_cameraScale + offsetX),
        static_cast<int>(pos.y * tileHeight * m_cameraScale + offsetY),
        static_cast<int>(tileWidth * m_cameraScale),
        static_cast<int>(tileHeight * m_cameraScale)};

    SDL_RenderCopy(m_gameManager->getRenderer(), texture, &srcRect, &dstRect);
  }

  // Then render wall tiles
  auto wallView =
      registry.view<Components::PositionComponent, Components::SpriteComponent,
                    Components::TileColliderTag>();
  for (auto entity : wallView) {
    const auto &pos = wallView.get<Components::PositionComponent>(entity);
    const auto &sprite = wallView.get<Components::SpriteComponent>(entity);

    SDL_Texture *texture =
        m_gameManager->getMapLoader()->getTilesetTexture(sprite.tilesetName);
    if (!texture)
      continue;

    int tilesPerRow = 16;
    if (m_gameManager->getMapLoader()->isLoaded()) {
      const Tmx::Tileset *tileset = nullptr;
      for (int i = 0; i < m_gameManager->getMapLoader()->getNumTilesets();
           ++i) {
        tileset = m_gameManager->getMapLoader()->getTileset(i);
        if (tileset && tileset->GetName() == sprite.tilesetName) {
          tilesPerRow = tileset->GetColumns();
          break;
        }
      }
    }

    // Use sprite ID directly (0-based indexing for the sprite sheet)
    int tileId = sprite.tileId;
    SDL_Rect srcRect = {(tileId % tilesPerRow) * tileWidth,
                        (tileId / tilesPerRow) * tileHeight, tileWidth,
                        tileHeight};

    SDL_Rect dstRect = {
        static_cast<int>(pos.x * tileWidth * m_cameraScale + offsetX),
        static_cast<int>(pos.y * tileHeight * m_cameraScale + offsetY),
        static_cast<int>(tileWidth * m_cameraScale),
        static_cast<int>(tileHeight * m_cameraScale)};

    SDL_RenderCopy(m_gameManager->getRenderer(), texture, &srcRect, &dstRect);
  }

  // Finally render entities (player, mobs, etc.)
  auto entityView =
      registry.view<Components::PositionComponent, Components::SpriteComponent>(
          entt::exclude<Components::FloorTag, Components::TileColliderTag>);
  for (auto entity : entityView) {
    const auto &pos = entityView.get<Components::PositionComponent>(entity);
    const auto &sprite = entityView.get<Components::SpriteComponent>(entity);

    SDL_Texture *texture =
        m_gameManager->getMapLoader()->getTilesetTexture(sprite.tilesetName);
    if (!texture)
      continue;

    int tilesPerRow = 16;
    if (m_gameManager->getMapLoader()->isLoaded()) {
      const Tmx::Tileset *tileset = nullptr;
      for (int i = 0; i < m_gameManager->getMapLoader()->getNumTilesets();
           ++i) {
        tileset = m_gameManager->getMapLoader()->getTileset(i);
        if (tileset && tileset->GetName() == sprite.tilesetName) {
          tilesPerRow = tileset->GetColumns();
          break;
        }
      }
    }

    // Use sprite ID directly (0-based indexing for the sprite sheet)
    int tileId = sprite.tileId;
    SDL_Rect srcRect = {(tileId % tilesPerRow) * tileWidth,
                        (tileId / tilesPerRow) * tileHeight, tileWidth,
                        tileHeight};

    SDL_Rect dstRect = {
        static_cast<int>(pos.x * tileWidth * m_cameraScale + offsetX),
        static_cast<int>(pos.y * tileHeight * m_cameraScale + offsetY),
        static_cast<int>(tileWidth * m_cameraScale),
        static_cast<int>(tileHeight * m_cameraScale)};

    SDL_RenderCopy(m_gameManager->getRenderer(), texture, &srcRect, &dstRect);
  }
}

} // namespace Rendering