#pragma once

#include "../../include/core/GameManager.h"
#include "DebugWindow.hpp"
#include <functional>
#include <string>

namespace UI {

class TmxLoaderWindow : public DebugWindow {
public:
  TmxLoaderWindow(GameManager *gameManager, bool *show = nullptr);
  virtual void render(entt::registry &registry) override;

  void setHotloadInfo(bool success, const std::string &message, float time) {
    m_lastHotloadSuccess = success;
    m_lastHotloadMessage = message;
    m_lastHotloadTime = time;
  }

  void incrementHotloadStats(bool success) {
    m_hotloadAttempts++;
    if (success)
      m_successfulHotloads++;
  }

  void setRegenerationCallback(std::function<void()> callback) {
    m_onRegenerateCallback = callback;
  }

private:
  GameManager *m_gameManager;
  bool m_lastHotloadSuccess{false};
  std::string m_lastHotloadMessage;
  float m_lastHotloadTime{0.0f};
  int m_hotloadAttempts{0};
  int m_successfulHotloads{0};
  std::function<void()> m_onRegenerateCallback;
};

} // namespace UI