#pragma once

#include "GameEvent.h"
#include <functional>
#include <unordered_map>
#include <vector>

class EventBus {
public:
  using Handler = std::function<void(const GameEvent &)>;

  void subscribe(GameEventType type, Handler handler) {
    handlers[type].push_back(handler);
  }

  void publish(const GameEvent &event) const {
    auto it = handlers.find(event.getType());
    if (it != handlers.end()) {
      for (const auto &handler : it->second) {
        handler(event);
      }
    }
  }

private:
  std::unordered_map<GameEventType, std::vector<Handler>> handlers;
};