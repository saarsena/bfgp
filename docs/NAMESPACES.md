# Namespace Hierarchy Documentation

This document outlines the namespace structure of the entire codebase, showing the organization of different components and their relationships.

## Core Namespace Hierarchy

```
Core                           - Core game systems and components
├── Services                  - Core services
│   └── RendererService      - SDL renderer management
├── GraphicsContext          - Window and renderer management
├── EntityFactory           - Entity creation and management
├── DungeonGenerator        - Base dungeon generation
└── WalkerDungeonGenerator  - Walker algorithm implementation
```

## Constants Namespace Hierarchy

```
Constants                    - Root namespace for all game constants
├── Assets                  - Asset file paths and resources
│   ├── SPRITE_SHEET       - Path to sprite sheet PNG
│   ├── SPRITE_TSX         - Path to sprite sheet TSX
│   └── MAP_TMX            - Path to map TMX
│
├── Sprites                - Sprite-related constants
│   ├── DEFAULT_TILE_WIDTH - Base tile dimensions
│   ├── DEFAULT_TILE_HEIGHT
│   ├── SHEET_COLUMNS     - Sprite sheet layout
│   ├── SHEET_ROWS
│   │
│   ├── IDs               - Sprite identifiers (0-based)
│   │   ├── PLAYER        - Yellow '@' character (64)
│   │   ├── MOB          - Enemy character 'm' (109)
│   │   ├── WALL         - Wall '#' symbol (35)
│   │   └── FLOOR        - Floor tile (96)
│   │
│   └── Tilesets         - Tileset names
│       └── MAIN_TILESET - Primary tileset "spritesheet"
│
├── Game                  - Game settings
│   ├── FPS              - Frame rate settings
│   ├── FRAME_DELAY
│   └── DEFAULT_*        - Dungeon generation defaults
│
└── Viewport             - Viewport/rendering settings
    ├── TILE_SCALE      - Rendering scale factors
    ├── SCALED_TILE_SIZE
    ├── MAP_TILES_WIDTH  - Map dimensions
    ├── MAP_TILES_HEIGHT
    ├── VIEWPORT_WIDTH   - Viewport dimensions
    └── VIEWPORT_HEIGHT
```

## Components Namespace Hierarchy

```
Components                    - Game entity components
├── Name                    - Entity naming
├── Position               - 2D positioning
├── Health                 - Health management
├── Attack                 - Combat attributes
├── Movement              - Movement capabilities
├── PlayerTag             - Player identification
├── Poisoned              - Status effects
├── CallForHelpOnHit      - AI triggers
├── AddSpawnTrigger       - AI behaviors
├── PositionComponent     - Entity positioning
├── CollisionComponent    - Collision detection
├── TestComponent         - Testing markers
├── PlayerMarker          - Player identification
├── TileColliderTag       - Collision markers
└── FloorTag             - Floor tile markers
```

## UI Namespace Hierarchy

```
UI                           - User interface components
├── PerformanceWindow       - Performance metrics display
├── EntityInspectorWindow   - Entity debugging
├── TmxLoaderWindow        - TMX map loading interface
└── WalkerDungeonWindow    - Dungeon generation controls
```

## Input Namespace Hierarchy

```
Input                        - Input handling systems
└── InputHandler           - Keyboard/mouse input processing
```

## Rendering Namespace Hierarchy

```
Rendering                    - Rendering systems
└── Renderer               - Main rendering system
```

## Examples Namespace Hierarchy

```
Examples                     - Example implementations
└── CollisionTest          - Collision system demo
```

## Usage Guidelines

1. When using constants, prefer the full namespace path:
   ```cpp
   Constants::Sprites::IDs::PLAYER
   Constants::Sprites::Tilesets::MAIN_TILESET
   ```

2. For cleaner code in implementation files, you can use namespace aliases:
   ```cpp
   namespace Examples {
   using namespace Constants::Sprites;  // Allows direct use of IDs:: and Tilesets::
   }
   ```

3. Component access should use the Components namespace:
   ```cpp
   registry.emplace<Components::PositionComponent>(entity, x, y);
   ```

4. Core systems should be accessed through the Core namespace:
   ```cpp
   std::unique_ptr<Core::GraphicsContext> graphics;
   std::unique_ptr<Core::EntityFactory> factory;
   ```

## Best Practices

1. Keep namespaces organized by functionality
2. Use nested namespaces for related functionality
3. Use clear, descriptive names for namespaces
4. Document namespace hierarchies in header files
5. Consider using namespace aliases to reduce verbosity
6. Avoid `using namespace` directives in header files
7. Prefer full namespace paths in header files
8. Use namespace aliases only in implementation files 