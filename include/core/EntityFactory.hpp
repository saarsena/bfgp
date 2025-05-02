#pragma once

#include "../include/core/Components.h"
#include "../include/core/Constants.h"
#include <SDL2/SDL.h>
#include <entt/entt.hpp>
#include <memory>
#include <string>

namespace Core {

class EntityFactory {
public:
  explicit EntityFactory(entt::registry &registry) : registry(registry) {}

  // Player creation
  entt::entity createPlayer(int x, int y,
                            const std::string &tilesetName =
                                Constants::Sprites::Tilesets::MAIN_TILESET);

  // Mob creation
  entt::entity createMob(int x, int y,
                         const std::string &tilesetName =
                             Constants::Sprites::Tilesets::MAIN_TILESET);

  // Tile creation
  entt::entity createWall(int x, int y,
                          const std::string &tilesetName =
                              Constants::Sprites::Tilesets::MAIN_TILESET);
  entt::entity createFloor(int x, int y,
                           const std::string &tilesetName =
                               Constants::Sprites::Tilesets::MAIN_TILESET);

  // Template method for creating entities with common components
  template <typename... ComponentTypes>
  entt::entity createEntity(int x, int y, ComponentTypes &&...components) {
    auto entity = registry.create();
    registry.emplace<Components::PositionComponent>(entity, x, y);
    (registry.emplace<ComponentTypes>(entity,
                                      std::forward<ComponentTypes>(components)),
     ...);
    return entity;
  }

private:
  entt::registry &registry;
};

} // namespace Core