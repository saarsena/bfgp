#pragma once

#include "../events/EventBus.h"
#include "../events/GameEvent.h"
#include "Components.h"
#include <SDL2/SDL.h>
#include <entt/entity/registry.hpp>
#include <vector>

class CollisionSystem {
public:
  // Check if a position would result in a collision
  static bool CheckCollision(const entt::registry &registry,
                             const Components::PositionComponent &pos,
                             entt::entity entityToIgnore = entt::null) {
    bool collisionDetected = false;

    registry
        .view<Components::PositionComponent, Components::CollisionComponent>()
        .each([&](auto entity, const auto &otherPos, const auto &collision) {
          if (entity != entityToIgnore && otherPos.x == pos.x &&
              otherPos.y == pos.y) {
            // Check if the entity is either blocking or has a PlayerMarker
            if (collision.isBlocking ||
                registry.all_of<Components::PlayerMarker>(entity)) {
              collisionDetected = true;
            }
          }
        });

    return collisionDetected;
  }

  // Check if a move would result in a collision and handle the movement
  static bool TryMove(entt::registry &registry, entt::entity entity, int dx,
                      int dy, EventBus &eventBus) {
    if (!registry.valid(entity)) {
      return false;
    }

    auto *pos = registry.try_get<Components::PositionComponent>(entity);
    if (!pos) {
      return false;
    }

    // Create a temporary position to test
    Components::PositionComponent newPos{pos->x + dx, pos->y + dy};

    // Check for collisions before moving
    bool canMove = true;
    auto collisions = registry.view<Components::CollisionComponent,
                                    Components::PositionComponent>();
    for (auto [collider, collision, colliderPos] : collisions.each()) {
      if (colliderPos.x == newPos.x && colliderPos.y == newPos.y) {
        if (collision.isBlocking) {
          canMove = false;
          break;
        }
      }
    }

    if (canMove) {
      // Update position
      pos->x = newPos.x;
      pos->y = newPos.y;

      // Emit movement event if it's a player
      if (registry.all_of<Components::PlayerMarker>(entity)) {
        PlayerMovedEvent event(pos->x, pos->y);
        eventBus.publish(event);
      }
    }

    return canMove;
  }

  // Check if two specific entities are colliding
  static bool AreEntitiesColliding(const entt::registry &registry,
                                   entt::entity entity1, entt::entity entity2) {
    if (!registry.valid(entity1) || !registry.valid(entity2)) {
      return false;
    }

    // Get positions of both entities
    const auto *pos1 = registry.try_get<Components::PositionComponent>(entity1);
    const auto *pos2 = registry.try_get<Components::PositionComponent>(entity2);

    if (!pos1 || !pos2) {
      return false;
    }

    // Check if they're at the same position
    return (pos1->x == pos2->x && pos1->y == pos2->y);
  }

  // Get all entities that are colliding with a specific entity
  static std::vector<entt::entity>
  GetCollidingEntities(const entt::registry &registry, entt::entity entity) {
    std::vector<entt::entity> collidingEntities;

    if (!registry.valid(entity)) {
      return collidingEntities;
    }

    const auto *pos = registry.try_get<Components::PositionComponent>(entity);
    if (!pos) {
      return collidingEntities;
    }

    registry.view<Components::PositionComponent>().each(
        [&](auto otherEntity, const auto &otherPos) {
          if (otherEntity != entity && pos->x == otherPos.x &&
              pos->y == otherPos.y) {
            collidingEntities.push_back(otherEntity);
          }
        });

    return collidingEntities;
  }

  // Create a wall with collision components
  static void CreateWall(entt::registry &registry, int tileX, int tileY,
                         int wallWidth, int wallHeight, int tileWidth,
                         int tileHeight,
                         std::vector<SDL_Rect> &collisionObjects) {
    // Create collision components for every tile in the wall
    for (int x = tileX; x < tileX + wallWidth; x++) {
      for (int y = tileY; y < tileY + wallHeight; y++) {
        auto entity = registry.create();
        registry.emplace<Components::PositionComponent>(entity, x, y);
        registry.emplace<Components::CollisionComponent>(entity, true);
        registry.emplace<Components::SpriteComponent>(
            entity, Constants::Sprites::Tilesets::MAIN_TILESET,
            Constants::Sprites::IDs::WALL);
      }
    }

    // Convert tile coordinates to pixel coordinates for rendering
    collisionObjects.push_back({tileX * tileWidth, tileY * tileHeight,
                                wallWidth * tileWidth,
                                wallHeight * tileHeight});
  }

  // Update positions while checking for collisions
  static void UpdatePositions(entt::registry &registry) {
    auto view = registry.view<Components::PositionComponent,
                              Components::CollisionComponent>();

    for (auto entity : view) {
      auto &pos = view.get<Components::PositionComponent>(entity);

      // Store original position
      Components::PositionComponent originalPos = pos;

      // Check for collisions at current position
      if (CheckCollision(registry, pos, entity)) {
        // If there's a collision, revert to original position
        pos = originalPos;
      }
    }
  }
};