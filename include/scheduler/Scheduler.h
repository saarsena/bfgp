#pragma once

#include <entt/entt.hpp>
#include <functional>
#include <queue>
#include <unordered_set>

using ActionID = uint32_t;

struct ScheduledAction {
  ActionID id;
  int tick;
  entt::entity entity;
  std::function<void(entt::entity, entt::registry &)> action;

  bool operator<(const ScheduledAction &other) const {
    return tick > other.tick; // Min-heap behavior for priority queue
  }
};

class Scheduler {
public:
  Scheduler() : nextActionId(1) {}

  // Schedule an action and get its ID for potential cancellation
  ActionID schedule(const ScheduledAction &action) {
    ActionID actionId = nextActionId++;
    ScheduledAction actionWithId = action;
    actionWithId.id = actionId;
    queue.push(actionWithId);
    activeActions.insert(actionId);
    return actionId;
  }

  // Convenience method to create and schedule an action
  ActionID
  schedule(int tick, entt::entity entity,
           std::function<void(entt::entity, entt::registry &)> action) {
    ScheduledAction scheduledAction{0, tick, entity, std::move(action)};
    return schedule(scheduledAction);
  }

  // Cancel a scheduled action
  bool cancel(ActionID id) {
    if (activeActions.find(id) != activeActions.end()) {
      // We can't remove from the priority queue directly, so we mark it as
      // inactive
      activeActions.erase(id);
      return true;
    }
    return false;
  }

  // Process actions that are due at the current tick
  void update(int current_tick, entt::registry &registry) {
    while (!queue.empty() && queue.top().tick <= current_tick) {
      ScheduledAction action = queue.top();
      queue.pop();

      // Skip if the action was cancelled
      if (activeActions.find(action.id) == activeActions.end()) {
        continue;
      }

      // Execute the action if the entity is still valid
      if (registry.valid(action.entity)) {
        action.action(action.entity, registry);
      }

      // Remove from active actions
      activeActions.erase(action.id);
    }
  }

  // Clear all pending actions
  void clear() {
    // Clear the queue - we have to rebuild it
    std::priority_queue<ScheduledAction> empty;
    std::swap(queue, empty);
    activeActions.clear();
  }

private:
  std::priority_queue<ScheduledAction> queue;
  std::unordered_set<ActionID> activeActions;
  ActionID nextActionId;
};