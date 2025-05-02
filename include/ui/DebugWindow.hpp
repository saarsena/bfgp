#pragma once

#include <entt/entt.hpp>
#include <memory>
#include <string>

namespace UI {

class DebugWindow {
public:
  explicit DebugWindow(const std::string &title, bool *show = nullptr)
      : title(title), isOpen(true), m_show(show) {}
  virtual ~DebugWindow() = default;

  virtual void render(entt::registry &registry) = 0;

  bool isWindowOpen() const { return isOpen; }
  void setWindowOpen(bool open) { isOpen = open; }

  const std::string &getTitle() const { return title; }
  bool *getShowPtr() const { return m_show; }
  bool isVisible() const { return m_show ? *m_show : isOpen; }

protected:
  std::string title;
  bool isOpen;
  bool *m_show;
};

} // namespace UI