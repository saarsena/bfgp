// WalkerDungeon.h
#ifndef WALKER_DUNGEON_H
#define WALKER_DUNGEON_H

#include <SDL.h>

#include <random>
#include <vector>

class WalkerDungeon {
public:
  WalkerDungeon(int totalFloorCount, int minHall, int maxHall, int roomDim,
                int tileWidth = 16, int tileHeight = 16);

  // Dungeon generation functions
  void GenerateRandomWalker();
  void GenerateRoomWalker();

  // Returns the generated floor positions
  const std::vector<SDL_Point> &GetFloorList() const;

  // Computes and returns a list of wall positions
  // Walls are any positions adjacent (8 directions) to a floor tile that are
  // not floors.
  std::vector<SDL_Point> GetWallList() const;

  // Getters for tile dimensions
  int GetTileWidth() const { return tileWidth; }
  int GetTileHeight() const { return tileHeight; }

private:
  // Helper functions
  void RandomRoom(const SDL_Point &pos);
  SDL_Point RandomDirection();
  bool InFloorList(const SDL_Point &pos) const; // Make this method const

  // Dungeon parameters
  int totalFloorCount = 1000;
  int minHall = 10;
  int maxHall = 20;
  int roomDim = 10;
  int tileWidth = 16;
  int tileHeight = 16;

  // Container for floor positions
  std::vector<SDL_Point> floorList;

  // Random number generator
  std::mt19937 rng;
};

#endif // WALKER_DUNGEON_H
