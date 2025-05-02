#include "../../include/ui/WalkerDungeonWindow.hpp"
#include "../../externals/imgui/imgui.h"
#include "../../include/core/Components.h"
#include <iostream>

namespace UI {

WalkerDungeonWindow::WalkerDungeonWindow(bool *show)
    : DebugWindow("Walker Dungeon Generator", show) {}

void WalkerDungeonWindow::render(entt::registry &registry) {
  if (!isVisible())
    return;

  ImGui::Begin(getTitle().c_str(), getShowPtr());

  // Dungeon parameters
  ImGui::Text("Dungeon Parameters:");
  ImGui::DragInt("Floor Count", &m_totalFloorCount, 1, 10, 1000, "%d");
  ImGui::DragInt("Min Hall Length", &m_minHall, 1, 1, m_maxHall, "%d");
  ImGui::DragInt("Max Hall Length", &m_maxHall, 1, m_minHall, 50, "%d");
  ImGui::DragInt("Room Dimension", &m_roomDim, 1, 3, 20, "%d");

  ImGui::Separator();
  ImGui::Text("Tile Settings:");
  ImGui::DragInt("Tile Width", &m_tileWidth, 1, 8, 64, "%d");
  ImGui::DragInt("Tile Height", &m_tileHeight, 1, 8, 64, "%d");

  if (ImGui::Button("Generate New Dungeon")) {
    std::cout << "Generating new dungeon with parameters:" << std::endl
              << "Floor Count: " << m_totalFloorCount << std::endl
              << "Min Hall: " << m_minHall << std::endl
              << "Max Hall: " << m_maxHall << std::endl
              << "Room Dim: " << m_roomDim << std::endl;

    if (m_onGenerateCallback) {
      m_onGenerateCallback();
    }
  }

  // Display current entity counts
  ImGui::Separator();
  ImGui::Text("Current Entities:");
  auto wallView = registry.view<Components::TileColliderTag>();
  auto floorView = registry.view<Components::FloorTag>();
  ImGui::Text("Walls: %zu", wallView.size());
  ImGui::Text("Floors: %zu", floorView.size());

  ImGui::End();
}

} // namespace UI