#pragma once

#include "ui/DebugWindow.hpp"
#include <SDL2/SDL.h>
#include <array>
#include <chrono>
#include <deque>
#include <entt/entt.hpp>

namespace UI {

class PerformanceWindow : public DebugWindow {
public:
  PerformanceWindow(const std::string &title, bool *show);
  void render(entt::registry &registry) override;
  void updateTickRate(Uint32 currentTick, Uint32 currentTime);

private:
  static constexpr size_t HISTORY_SIZE = 60;
  static constexpr size_t TPS_UPDATE_INTERVAL = 1000; // Update TPS every second

  // FPS tracking
  std::array<float, HISTORY_SIZE> fpsHistory{};
  size_t fpsHistoryIndex = 0;
  float currentFPS = 0.0f;
  std::chrono::high_resolution_clock::time_point lastFrameTime =
      std::chrono::high_resolution_clock::now();

  // TPS (Ticks Per Second) tracking
  std::array<float, HISTORY_SIZE> tpsHistory{};
  size_t tpsHistoryIndex = 0;
  float currentTPS = 0.0f;
  Uint32 lastTickCount = 0;
  Uint32 lastTickTime = 0;

  // Memory tracking
  std::array<size_t, HISTORY_SIZE> memoryHistory{};
  size_t memoryHistoryIndex = 0;
  size_t currentMemoryUsage = 0;

  // CPU usage tracking
  std::array<float, HISTORY_SIZE> cpuHistory{};
  size_t cpuHistoryIndex = 0;
  float currentCPUUsage = 0.0f;
  std::chrono::high_resolution_clock::time_point lastCPUCheckTime =
      std::chrono::high_resolution_clock::now();
  float lastUserTime = 0.0f;   // Last recorded user CPU time
  float lastSystemTime = 0.0f; // Last recorded system CPU time

  // Helper methods
  void updateFPS();
  void updateMemoryUsage();
  void updateCPUUsage();
  float calculateAverage(const std::array<float, HISTORY_SIZE> &history);
  float calculateAverage(const std::array<size_t, HISTORY_SIZE> &history);
  void plotHistory(const char *label,
                   const std::array<float, HISTORY_SIZE> &history,
                   float minScale = 0.0f, float maxScale = 0.0f);
};

} // namespace UI