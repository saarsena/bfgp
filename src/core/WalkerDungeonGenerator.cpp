#include "../include/core/walkerdungeon.hpp"
#include "core/DungeonGenerator.hpp"
#include <algorithm>
#include <limits>

namespace Core {

WalkerDungeonGenerator::WalkerDungeonGenerator(int totalFloorCount, int minHall,
                                               int maxHall, int roomDim,
                                               int tileWidth, int tileHeight)
    : totalFloorCount(totalFloorCount), minHall(minHall), maxHall(maxHall),
      roomDim(roomDim), tileWidth(tileWidth), tileHeight(tileHeight) {}

void WalkerDungeonGenerator::generate() {
  WalkerDungeon walker(totalFloorCount, minHall, maxHall, roomDim, tileWidth,
                       tileHeight);
  walker.GenerateRoomWalker();

  floors = walker.GetFloorList();
  walls = walker.GetWallList();

  normalizeCoordinates();
}

std::vector<SDL_Point> WalkerDungeonGenerator::getFloors() const {
  return floors;
}

std::vector<SDL_Point> WalkerDungeonGenerator::getWalls() const {
  return walls;
}

void WalkerDungeonGenerator::setParameters(int totalFloorCount, int minHall,
                                           int maxHall, int roomDim,
                                           int tileWidth, int tileHeight) {
  this->totalFloorCount = totalFloorCount;
  this->minHall = minHall;
  this->maxHall = maxHall;
  this->roomDim = roomDim;
  this->tileWidth = tileWidth;
  this->tileHeight = tileHeight;
}

void WalkerDungeonGenerator::normalizeCoordinates() {
  // Find the minimum x and y coordinates
  int minX = std::numeric_limits<int>::max();
  int minY = std::numeric_limits<int>::max();

  for (const auto &floor : floors) {
    minX = std::min(minX, floor.x);
    minY = std::min(minY, floor.y);
  }

  for (const auto &wall : walls) {
    minX = std::min(minX, wall.x);
    minY = std::min(minY, wall.y);
  }

  // Add offset to keep dungeon away from screen edges
  const int OFFSET = 2;

  // Normalize floor coordinates
  for (auto &floor : floors) {
    floor.x = floor.x - minX + OFFSET;
    floor.y = floor.y - minY + OFFSET;
  }

  // Normalize wall coordinates
  for (auto &wall : walls) {
    wall.x = wall.x - minX + OFFSET;
    wall.y = wall.y - minY + OFFSET;
  }
}

} // namespace Core