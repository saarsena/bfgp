#include "ui/PerformanceWindow.hpp"
#include "core/Components.h"
#include <chrono>
#include <imgui.h>
#include <string>
#include <sys/resource.h>
#include <sys/sysinfo.h>
#include <unistd.h>

namespace UI {

PerformanceWindow::PerformanceWindow(const std::string &title, bool *show)
    : DebugWindow(title, show) {
  // Initialize last CPU values
  struct rusage usage;
  if (getrusage(RUSAGE_SELF, &usage) == 0) {
    lastUserTime = usage.ru_utime.tv_sec + usage.ru_utime.tv_usec / 1000000.0f;
    lastSystemTime =
        usage.ru_stime.tv_sec + usage.ru_stime.tv_usec / 1000000.0f;
  }
}

void PerformanceWindow::updateTickRate(Uint32 currentTick, Uint32 currentTime) {
  if (currentTime - lastTickTime >= TPS_UPDATE_INTERVAL) {
    Uint32 tickDelta = currentTick - lastTickCount;
    float timeElapsed = (currentTime - lastTickTime) / 1000.0f;
    currentTPS = tickDelta / timeElapsed;

    tpsHistory[tpsHistoryIndex] = currentTPS;
    tpsHistoryIndex = (tpsHistoryIndex + 1) % HISTORY_SIZE;

    lastTickCount = currentTick;
    lastTickTime = currentTime;
  }
}

void PerformanceWindow::updateFPS() {
  auto now = std::chrono::high_resolution_clock::now();
  auto deltaTime =
      std::chrono::duration_cast<std::chrono::milliseconds>(now - lastFrameTime)
          .count();
  lastFrameTime = now;

  if (deltaTime > 0) {
    currentFPS = 1000.0f / deltaTime;
    fpsHistory[fpsHistoryIndex] = currentFPS;
    fpsHistoryIndex = (fpsHistoryIndex + 1) % HISTORY_SIZE;
  }
}

void PerformanceWindow::updateMemoryUsage() {
  struct rusage usage;
  if (getrusage(RUSAGE_SELF, &usage) == 0) {
    currentMemoryUsage = usage.ru_maxrss; // in kilobytes
    memoryHistory[memoryHistoryIndex] = currentMemoryUsage;
    memoryHistoryIndex = (memoryHistoryIndex + 1) % HISTORY_SIZE;
  }
}

void PerformanceWindow::updateCPUUsage() {
  auto now = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
                      now - lastCPUCheckTime)
                      .count();

  if (duration >= 1000) { // Update every second
    struct rusage usage;
    if (getrusage(RUSAGE_SELF, &usage) == 0) {
      // Get current CPU times
      float currentUserTime =
          usage.ru_utime.tv_sec + usage.ru_utime.tv_usec / 1000000.0f;
      float currentSystemTime =
          usage.ru_stime.tv_sec + usage.ru_stime.tv_usec / 1000000.0f;

      // Calculate CPU time used in this interval
      float userTimeDelta = currentUserTime - lastUserTime;
      float systemTimeDelta = currentSystemTime - lastSystemTime;
      float totalTimeDelta = userTimeDelta + systemTimeDelta;

      // Calculate CPU usage as percentage of the interval
      float intervalInSeconds = duration / 1000.0f;
      currentCPUUsage = (totalTimeDelta / intervalInSeconds) * 100.0f;

      // Store current times for next update
      lastUserTime = currentUserTime;
      lastSystemTime = currentSystemTime;

      // Update history
      cpuHistory[cpuHistoryIndex] = currentCPUUsage;
      cpuHistoryIndex = (cpuHistoryIndex + 1) % HISTORY_SIZE;
    }
    lastCPUCheckTime = now;
  }
}

float PerformanceWindow::calculateAverage(
    const std::array<float, HISTORY_SIZE> &history) {
  float sum = 0.0f;
  for (float value : history) {
    sum += value;
  }
  return sum / HISTORY_SIZE;
}

float PerformanceWindow::calculateAverage(
    const std::array<size_t, HISTORY_SIZE> &history) {
  size_t sum = 0;
  for (size_t value : history) {
    sum += value;
  }
  return static_cast<float>(sum) / HISTORY_SIZE;
}

void PerformanceWindow::plotHistory(
    const char *label, const std::array<float, HISTORY_SIZE> &history,
    float minScale, float maxScale) {
  ImGui::PlotLines(label, history.data(), HISTORY_SIZE, 0, nullptr, minScale,
                   maxScale, ImVec2(0, 50));
}

void PerformanceWindow::render(entt::registry &registry) {
  if (!isVisible())
    return;

  updateFPS();
  updateMemoryUsage();
  updateCPUUsage();

  if (ImGui::Begin(getTitle().c_str(), getShowPtr())) {
    // FPS Section
    if (ImGui::CollapsingHeader("FPS", ImGuiTreeNodeFlags_DefaultOpen)) {
      ImGui::Text("Current FPS: %.1f", currentFPS);
      ImGui::Text("Average FPS: %.1f", calculateAverage(fpsHistory));
      plotHistory("FPS History", fpsHistory);
    }

    // TPS Section
    if (ImGui::CollapsingHeader("TPS", ImGuiTreeNodeFlags_DefaultOpen)) {
      ImGui::Text("Current TPS: %.1f", currentTPS);
      ImGui::Text("Average TPS: %.1f", calculateAverage(tpsHistory));
      plotHistory("TPS History", tpsHistory);
    }

    // Memory Section
    if (ImGui::CollapsingHeader("Memory", ImGuiTreeNodeFlags_DefaultOpen)) {
      ImGui::Text("Current Memory Usage: %.2f MB",
                  currentMemoryUsage / 1024.0f);
      ImGui::Text("Average Memory Usage: %.2f MB",
                  calculateAverage(memoryHistory) / 1024.0f);
      plotHistory(
          "Memory History (MB)",
          std::array<float, HISTORY_SIZE>{}, // Convert KB to MB for display
          0.0f, currentMemoryUsage * 1.2f / 1024.0f);
    }

    // CPU Section
    if (ImGui::CollapsingHeader("CPU", ImGuiTreeNodeFlags_DefaultOpen)) {
      ImGui::Text("Current CPU Usage: %.1f%%", currentCPUUsage);
      ImGui::Text("Average CPU Usage: %.1f%%", calculateAverage(cpuHistory));
      plotHistory("CPU History", cpuHistory, 0.0f, 100.0f);
    }

    // Entity Stats Section
    if (ImGui::CollapsingHeader("Entity Stats",
                                ImGuiTreeNodeFlags_DefaultOpen)) {
      ImGui::Text("Total Entities: %zu",
                  registry.storage<entt::entity>().size());
      ImGui::Text("Position Components: %zu",
                  registry.view<Components::PositionComponent>().size());
      ImGui::Text("Sprite Components: %zu",
                  registry.view<Components::SpriteComponent>().size());
      ImGui::Text("Collision Components: %zu",
                  registry.view<Components::CollisionComponent>().size());
      ImGui::Text("Player Components: %zu",
                  registry.view<Components::PlayerMarker>().size());
      ImGui::Text("Mob Components: %zu",
                  registry.view<Components::MobTag>().size());
      ImGui::Text("Wander Components: %zu",
                  registry.view<Components::WanderComponent>().size());
    }
  }
  ImGui::End();
}

} // namespace UI