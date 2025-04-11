#pragma once

#include "../../include/core/Constants.h"
#include "DebugWindow.hpp"
#include <functional>

namespace UI {

class WalkerDungeonWindow : public DebugWindow {
public:
  WalkerDungeonWindow(bool *show = nullptr);
  virtual void render(entt::registry &registry) override;

  void setGenerationCallback(std::function<void()> callback) {
    m_onGenerateCallback = callback;
  }

  // Getters for dungeon parameters
  int getTotalFloorCount() const { return m_totalFloorCount; }
  int getMinHall() const { return m_minHall; }
  int getMaxHall() const { return m_maxHall; }
  int getRoomDim() const { return m_roomDim; }
  int getTileWidth() const { return m_tileWidth; }
  int getTileHeight() const { return m_tileHeight; }

private:
  int m_totalFloorCount{Constants::Game::DEFAULT_FLOOR_COUNT};
  int m_minHall{Constants::Game::DEFAULT_MIN_HALL};
  int m_maxHall{Constants::Game::DEFAULT_MAX_HALL};
  int m_roomDim{Constants::Game::DEFAULT_ROOM_DIM};
  int m_tileWidth{Constants::Sprites::DEFAULT_TILE_WIDTH};
  int m_tileHeight{Constants::Sprites::DEFAULT_TILE_HEIGHT};

  std::function<void()> m_onGenerateCallback;
};

} // namespace UI