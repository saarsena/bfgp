# 🕹️ Entropic Engine

_A minimal, modular game engine built in C++ using SDL2 and EnTT._

This engine serves as a foundation for tile-based simulations and procedural storytelling. It uses an ECS (Entity Component System) pattern for clarity, scalability, and speed.

---

## 1. Introduction

This document outlines the architecture and implementation of a minimalist game engine designed with a focus on emergent behavior and modular simulation systems.

Built in **C++**, using **SDL2** for rendering/input and **EnTT** for ECS, the engine is ideal for prototyping or building full 2D games from scratch.

---

## 2. Architecture

### 2.1 Core Systems

#### 2.1.1 `GameManager`

The central orchestrator. Manages the SDL window, renderer, and access to other systems like input and rendering.

#### 2.1.2 `InputHandler`

Processes SDL events and translates them into game actions. Supports:
- Keyboard + mouse input
- Camera movement and zoom
- Modifier-based debug commands

#### 2.1.3 `CollisionSystem`

A lightweight grid-based collision detection system for entities tagged as collidable.

---

### 2.2 Entity Component System (EnTT)

#### 2.2.1 Components

- `PositionComponent`: Stores world coordinates
- `CollisionComponent`: Tags an entity as collidable
- `PlayerMarker`: Identifies the player
- `TileColliderTag`: Used for static tile collisions

#### 2.2.2 Systems

Each system operates over filtered entity sets. Example: the collision system processes all entities with `PositionComponent` and `CollisionComponent`.

---

### 2.3 Rendering

Rendering is SDL2-based, and debug tools use ImGui overlays.

The game renders:
- Entities
- Map tiles
- UI overlays for debugging, memory, and world inspection

---

## 3. Implementation Details

### 3.1 Game Loop

Uses a fixed timestep model:
1. Measure `deltaTime`
2. Handle input
3. Update logic
4. Render world + debug UI

### 3.2 Input Handling

Translates SDL events into entity modifications and camera actions.

### 3.3 Collision Detection

Spatial grid check between all `CollisionComponent`-tagged entities.

### 3.4 Entity Management

Entities are managed with EnTT's `registry`. All creation, deletion, and query operations are handled through it.

---

## 4. Usage

Create an instance of the `CollisionTest` class and call `run()`:

```cpp
int main() {
    CollisionTest game;
    game.run();
    return 0;
}
```

The engine handles:
- Input
- Update ticks
- Hot-reloading `.tmx` maps from Tiled
- Debug visualization

---

## 5. Future Enhancements

- AI behavior trees and state machines
- Audio (SDL_mixer)
- Resource and asset management
- Advanced tilemap support
- Story or dialogue systems
- Export simulation history
- Custom scripting or plugin support

---

## 6. Conclusion

This engine is lightweight, fast, and built for evolution.

It’s ideal for:
- Procedural simulations
- Tactical RPGs
- Rogue-inspired games
- Experimental emergent storytelling

> Designed for clarity, built for chaos.  
> _2025-04-10_
