#pragma once

#include "../../include/core/Mob.h"
#include "DebugWindow.hpp"
#include <entt/entt.hpp>
#include <string>

namespace UI {

class EntityInspectorWindow : public DebugWindow {
public:
  EntityInspectorWindow(bool *show = nullptr);
  virtual void render(entt::registry &registry) override;
  void setMob(Mob *mob) { m_mob = mob; }

private:
  void displayComponentData(entt::registry &registry, entt::entity entity);
  std::string getEntityDescription(entt::registry &registry,
                                   entt::entity entity);

  entt::entity m_selectedEntity{entt::null};
  Mob *m_mob{nullptr};
};

} // namespace UI