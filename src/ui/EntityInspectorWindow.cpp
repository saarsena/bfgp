#include "../../include/ui/EntityInspectorWindow.hpp"
#include "../../externals/imgui/imgui.h"
#include <iostream>

namespace UI {

EntityInspectorWindow::EntityInspectorWindow(bool *show)
    : DebugWindow("Entity Inspector", show) {}

void EntityInspectorWindow::render(entt::registry &registry) {
  if (!isVisible())
    return;

  ImGui::Begin(getTitle().c_str(), getShowPtr());

  // Entity List
  ImGui::BeginChild("Entities", ImVec2(200, 0), true);

  // Display all entities grouped by type
  if (ImGui::CollapsingHeader("Players", ImGuiTreeNodeFlags_DefaultOpen)) {
    auto playerView = registry.view<Components::PlayerMarker>();
    for (auto entity : playerView) {
      std::string label =
          "Player##" + std::to_string(static_cast<uint32_t>(entity));
      if (ImGui::Selectable(label.c_str(), m_selectedEntity == entity)) {
        m_selectedEntity = entity;
      }
    }
  }

  if (ImGui::CollapsingHeader("Mobs", ImGuiTreeNodeFlags_DefaultOpen)) {
    auto mobView = registry.view<Components::MobTag>();
    int mobCount = 0;
    for (auto entity : mobView) {
      std::string label = "Mob " + std::to_string(mobCount++) + "##" +
                          std::to_string(static_cast<uint32_t>(entity));
      if (ImGui::Selectable(label.c_str(), m_selectedEntity == entity)) {
        m_selectedEntity = entity;
      }
    }
    if (mobCount == 0) {
      ImGui::TextDisabled("No mobs found");
    }
  }

  if (ImGui::CollapsingHeader("Walls", ImGuiTreeNodeFlags_DefaultOpen)) {
    auto wallView = registry.view<Components::TileColliderTag>();
    int wallCount = 0;
    for (auto entity : wallView) {
      std::string label = "Wall " + std::to_string(wallCount++) + "##" +
                          std::to_string(static_cast<uint32_t>(entity));
      if (ImGui::Selectable(label.c_str(), m_selectedEntity == entity)) {
        m_selectedEntity = entity;
      }
    }
    if (wallCount == 0) {
      ImGui::TextDisabled("No walls found");
    }
  }

  if (ImGui::CollapsingHeader("Floor Tiles", ImGuiTreeNodeFlags_DefaultOpen)) {
    auto floorView = registry.view<Components::FloorTag>();
    int floorCount = 0;
    for (auto entity : floorView) {
      std::string label = "Floor " + std::to_string(floorCount++) + "##" +
                          std::to_string(static_cast<uint32_t>(entity));
      if (ImGui::Selectable(label.c_str(), m_selectedEntity == entity)) {
        m_selectedEntity = entity;
      }
    }
    if (floorCount == 0) {
      ImGui::TextDisabled("No floor tiles found");
    }
  }

  ImGui::EndChild();
  ImGui::SameLine();

  // Component Inspector
  ImGui::BeginChild("Components", ImVec2(0, 0), true);
  if (m_selectedEntity != entt::null && registry.valid(m_selectedEntity)) {
    ImGui::Text("Entity ID: %u", static_cast<uint32_t>(m_selectedEntity));
    ImGui::Separator();
    displayComponentData(registry, m_selectedEntity);
  } else {
    ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "No entity selected");
  }
  ImGui::EndChild();

  ImGui::End();
}

void EntityInspectorWindow::displayComponentData(entt::registry &registry,
                                                 entt::entity entity) {
  // Position Component
  if (auto *pos = registry.try_get<Components::PositionComponent>(entity)) {
    if (ImGui::CollapsingHeader("Position", ImGuiTreeNodeFlags_DefaultOpen)) {
      ImGui::DragInt("X", &pos->x, 0.1f);
      ImGui::DragInt("Y", &pos->y, 0.1f);
    }
  }

  // Sprite Component
  if (auto *sprite = registry.try_get<Components::SpriteComponent>(entity)) {
    if (ImGui::CollapsingHeader("Sprite", ImGuiTreeNodeFlags_DefaultOpen)) {
      ImGui::Text("Tileset: %s", sprite->tilesetName.c_str());
      ImGui::DragInt("Tile ID", &sprite->tileId, 0.1f);
    }
  }

  // Collision Component
  if (auto *collision =
          registry.try_get<Components::CollisionComponent>(entity)) {
    if (ImGui::CollapsingHeader("Collision", ImGuiTreeNodeFlags_DefaultOpen)) {
      ImGui::Checkbox("Is Blocking", &collision->isBlocking);
    }
  }

  // Player Marker
  if (registry.all_of<Components::PlayerMarker>(entity)) {
    if (ImGui::CollapsingHeader("Player", ImGuiTreeNodeFlags_DefaultOpen)) {
      ImGui::Text("This entity is the player");
    }
  }

  // Floor Tag
  if (registry.all_of<Components::FloorTag>(entity)) {
    if (ImGui::CollapsingHeader("Floor", ImGuiTreeNodeFlags_DefaultOpen)) {
      ImGui::Text("This entity is a floor tile");
    }
  }

  // Tile Collider Tag
  if (registry.all_of<Components::TileColliderTag>(entity)) {
    if (ImGui::CollapsingHeader("Wall", ImGuiTreeNodeFlags_DefaultOpen)) {
      ImGui::Text("This entity is a wall tile");
    }
  }

  // Add Delete Button
  ImGui::Separator();
  if (ImGui::Button("Delete Entity", ImVec2(-1, 0))) {
    if (registry.valid(entity)) {
      // Don't delete the player
      if (!registry.all_of<Components::PlayerMarker>(entity)) {
        registry.destroy(entity);
        m_selectedEntity = entt::null;
      } else {
        std::cout << "Cannot delete player entity!" << std::endl;
      }
    }
  }
}

std::string
EntityInspectorWindow::getEntityDescription(entt::registry &registry,
                                            entt::entity entity) {
  if (!registry.valid(entity))
    return "Invalid Entity";

  if (registry.all_of<Components::PlayerMarker>(entity))
    return "Player";
  if (m_mob && m_mob->getEntity() == entity)
    return "Mob";
  if (registry.all_of<Components::TileColliderTag>(entity))
    return "Wall";
  if (registry.all_of<Components::FloorTag>(entity))
    return "Floor";

  return "Unknown";
}

} // namespace UI