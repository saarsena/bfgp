#pragma once

#include <SDL2/SDL.h>
#include <string>

/**
 * Global Constants Namespace Hierarchy
 *
 * Constants                    - Root namespace for all game constants
 * ├── Assets                  - Asset file paths and resources
 * │   ├── SPRITE_SHEET       - Path to sprite sheet PNG
 * │   ├── SPRITE_TSX         - Path to sprite sheet TSX
 * │   └── MAP_TMX            - Path to map TMX
 * │
 * ├── Sprites                - Sprite-related constants
 * │   ├── DEFAULT_TILE_WIDTH - Base tile dimensions
 * │   ├── DEFAULT_TILE_HEIGHT
 * │   ├── SHEET_COLUMNS     - Sprite sheet layout
 * │   ├── SHEET_ROWS
 * │   │
 * │   ├── IDs               - Sprite identifiers (0-based)
 * │   │   ├── PLAYER        - Yellow '@' character (64)
 * │   │   ├── MOB          - Enemy character 'm' (109)
 * │   │   ├── WALL         - Wall '#' symbol (35)
 * │   │   └── FLOOR        - Floor tile (96)
 * │   │
 * │   └── Tilesets         - Tileset names
 * │       └── MAIN_TILESET - Primary tileset "spritesheet"
 * │
 * ├── Game                  - Game settings
 * │   ├── FPS              - Frame rate settings
 * │   ├── FRAME_DELAY
 * │   └── DEFAULT_*        - Dungeon generation defaults
 * │
 * └── Viewport             - Viewport/rendering settings
 *     ├── TILE_SCALE      - Rendering scale factors
 *     ├── SCALED_TILE_SIZE
 *     ├── MAP_TILES_WIDTH  - Map dimensions
 *     ├── MAP_TILES_HEIGHT
 *     ├── VIEWPORT_WIDTH   - Viewport dimensions
 *     └── VIEWPORT_HEIGHT
 */

namespace Constants {
// Window settings
constexpr int WINDOW_WIDTH = 1920;
constexpr int WINDOW_HEIGHT = 1080;

// Asset paths
namespace Assets {
const std::string SPRITE_SHEET = "assets/spritesheet.png";
const std::string SPRITE_TSX = "assets/spritesheet.tsx";
const std::string MAP_TMX = "assets/asciimap.tmx";
} // namespace Assets

// Sprite configuration
namespace Sprites {
// Default tile dimensions
constexpr int DEFAULT_TILE_WIDTH = 8;
constexpr int DEFAULT_TILE_HEIGHT = 8;

// Sprite sheet configuration
constexpr int SHEET_COLUMNS = 16; // Number of columns in the sprite sheet
constexpr int SHEET_ROWS = 16;    // Number of rows in the sprite sheet

// Common sprite IDs from the sprite sheet
namespace IDs {
// These IDs are 0-based for direct use in the sprite sheet
constexpr int PLAYER = 64; // Yellow '@' character
constexpr int MOB = 109;   // Enemy character small red m
constexpr int WALL = 35;   // Wall character ('#' symbol)
constexpr int FLOOR = 96;  // Floor/walkable tile - blank tyle
} // namespace IDs

// Tileset names
namespace Tilesets {
const std::string MAIN_TILESET = "spritesheet"; // Name used in TMX/TSX files
} // namespace Tilesets
} // namespace Sprites

// Game settings
namespace Game {
constexpr int FPS = 60;
constexpr int FRAME_DELAY = 1000 / FPS;

// Dungeon generation parameters
constexpr int DEFAULT_FLOOR_COUNT = 2000;
constexpr int DEFAULT_MIN_HALL = 4;
constexpr int DEFAULT_MAX_HALL = 12;
constexpr int DEFAULT_ROOM_DIM = 3;
} // namespace Game

// Viewport and rendering settings
namespace Viewport {
constexpr float TILE_SCALE = 4.0f; // Scale factor for rendering tiles
constexpr int SCALED_TILE_SIZE =
    Sprites::DEFAULT_TILE_WIDTH * TILE_SCALE; // 32 pixels
constexpr int MAP_TILES_WIDTH = 30;           // Number of tiles across
constexpr int MAP_TILES_HEIGHT = 20;          // Number of tiles down
constexpr int VIEWPORT_WIDTH = MAP_TILES_WIDTH * SCALED_TILE_SIZE; // 960 pixels
constexpr int VIEWPORT_HEIGHT =
    MAP_TILES_HEIGHT * SCALED_TILE_SIZE; // 640 pixels
} // namespace Viewport
} // namespace Constants