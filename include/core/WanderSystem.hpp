#pragma once

#include <SDL2/SDL.h>
#include <entt/entity/registry.hpp>
#include <vector>

namespace Systems {

void WanderSystem(entt::registry &registry, Uint32 currentTime);

} // namespace Systems