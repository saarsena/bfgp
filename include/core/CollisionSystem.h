#pragma once

#include "Components.h"
#include <entt/entt.hpp>

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