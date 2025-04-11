#include "../../include/ui/TmxLoaderWindow.hpp"
#include "../../externals/imgui/imgui.h"
#include <SDL2/SDL.h>
#include <iostream>

namespace UI {

TmxLoaderWindow::TmxLoaderWindow(GameManager *gameManager, bool *show)
    : DebugWindow("TMX Map Loader", show), m_gameManager(gameManager) {}

void TmxLoaderWindow::render(entt::registry &registry) {
  if (!isVisible())
    return;

  ImGui::Begin(getTitle().c_str(), getShowPtr());

  // Quick load buttons for specific maps
  if (ImGui::Button("Load ASCII Map")) {
    std::string mapPath = "assets/asciimap.tmx";
    if (m_gameManager->getMapLoader()->loadMap(mapPath, registry)) {
      std::cout << "Successfully loaded ASCII map" << std::endl;
      if (m_onRegenerateCallback) {
        m_onRegenerateCallback();
      }
    } else {
      std::cout << "Failed to load ASCII map: " << mapPath << std::endl;
    }
  }
  ImGui::SameLine();
  ImGui::TextDisabled("(?)");
  if (ImGui::IsItemHovered()) {
    ImGui::SetTooltip("Load the default ASCII-style map");
  }

  ImGui::Separator();

  // Display current map info if loaded
  if (m_gameManager->getMapLoader()->isLoaded()) {
    ImGui::Text("Current Map: asciimap.tmx");

    // Get map dimensions from the first layer
    if (m_gameManager->getMapLoader()->getNumLayers() > 0) {
      const Tmx::Layer *layer = m_gameManager->getMapLoader()->getLayer(0);
      if (layer) {
        ImGui::Text("Map Size: %dx%d", layer->GetWidth(), layer->GetHeight());
      }
    }

    ImGui::Text("Tile Size: %dx%d",
                m_gameManager->getMapLoader()->getTileWidth(),
                m_gameManager->getMapLoader()->getTileHeight());

    // Display hotload status
    if (m_lastHotloadTime > 0) {
      ImGui::Separator();
      ImGui::Text("Last Hotload: %.1f seconds ago",
                  SDL_GetTicks() / 1000.0f - m_lastHotloadTime);
      ImGui::Text("Hotload Status: %s",
                  m_lastHotloadSuccess ? "Success" : "Failed");
      if (!m_lastHotloadMessage.empty()) {
        ImGui::Text("Message: %s", m_lastHotloadMessage.c_str());
      }
      ImGui::Text("Total Successful Hotloads: %d", m_successfulHotloads);
    }

    if (ImGui::Button("Regenerate Map")) {
      if (m_onRegenerateCallback) {
        m_onRegenerateCallback();
      }
      ImGui::SameLine();
      ImGui::TextDisabled("(?)");
      if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip(
            "Regenerate the current map\nUseful for testing changes");
      }
    }
  } else {
    ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f),
                       "No map currently loaded");
  }

  ImGui::End();
}

} // namespace UI