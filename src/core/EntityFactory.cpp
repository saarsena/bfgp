#include "core/EntityFactory.hpp"

namespace Core {

entt::entity EntityFactory::createPlayer(int x, int y,
                                         const std::string &tilesetName) {
  return createEntity(x, y, Components::TestComponent{},
                      Components::CollisionComponent{false},
                      Components::PlayerMarker{},
                      Components::SpriteComponent{
                          tilesetName, Constants::Sprites::IDs::PLAYER});
}

entt::entity EntityFactory::createMob(int x, int y,
                                      const std::string &tilesetName) {
  return createEntity(
      x, y, Components::CollisionComponent{false},
      Components::SpriteComponent{tilesetName, Constants::Sprites::IDs::MOB});
}

entt::entity EntityFactory::createWall(int x, int y,
                                       const std::string &tilesetName) {
  return createEntity(
      x, y, Components::CollisionComponent{true}, Components::TileColliderTag{},
      Components::SpriteComponent{tilesetName, Constants::Sprites::IDs::WALL});
}

entt::entity EntityFactory::createFloor(int x, int y,
                                        const std::string &tilesetName) {
  return createEntity(
      x, y, Components::CollisionComponent{false}, Components::FloorTag{},
      Components::SpriteComponent{tilesetName, Constants::Sprites::IDs::FLOOR});
}

} // namespace Core