#pragma once

#include <SDL2/SDL.h>
#include <memory>
#include <vector>

namespace Core {

class DungeonGenerator {
public:
  virtual ~DungeonGenerator() = default;

  virtual void generate() = 0;
  virtual std::vector<SDL_Point> getFloors() const = 0;
  virtual std::vector<SDL_Point> getWalls() const = 0;

  virtual void setParameters(int totalFloorCount, int minHall, int maxHall,
                             int roomDim, int tileWidth, int tileHeight) = 0;
};

class WalkerDungeonGenerator : public DungeonGenerator {
public:
  WalkerDungeonGenerator(int totalFloorCount, int minHall, int maxHall,
                         int roomDim, int tileWidth, int tileHeight);

  void generate() override;
  std::vector<SDL_Point> getFloors() const override;
  std::vector<SDL_Point> getWalls() const override;

  void setParameters(int totalFloorCount, int minHall, int maxHall, int roomDim,
                     int tileWidth, int tileHeight) override;

private:
  int totalFloorCount;
  int minHall;
  int maxHall;
  int roomDim;
  int tileWidth;
  int tileHeight;

  std::vector<SDL_Point> floors;
  std::vector<SDL_Point> walls;

  void normalizeCoordinates();
};

} // namespace Core