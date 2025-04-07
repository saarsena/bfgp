#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <iostream>
#include <string>
#include <vector>

#include "core/walkerdungeon.hpp"

const int WINDOW_WIDTH = 1920;
const int WINDOW_HEIGHT = 1080;
const int TILE_SIZE = 16;   // Size of each tile in pixels
const int TILESET_COLS = 4; // Number of columns in the tileset
const int TILESET_ROWS = 4; // Number of rows in the tileset

// Camera settings
const float MIN_ZOOM = 0.5f;
const float MAX_ZOOM = 4.0f;
const float ZOOM_SPEED = 0.1f;
const int SCROLL_SPEED = 10;

// UI settings
const int FONT_SIZE = 24;
const int UI_PADDING = 10;
const int PARAM_STEP = 100; // Step size for parameter adjustments

// Helper function to convert 1-based tile index to source rectangle
SDL_Rect GetTileRect(int tileIndex) {
  // Convert 1-based index to 0-based
  tileIndex--;
  // Calculate row and column in the 4x4 grid
  int row = tileIndex / TILESET_COLS;
  int col = tileIndex % TILESET_COLS;
  return {col * TILE_SIZE, row * TILE_SIZE, TILE_SIZE, TILE_SIZE};
}

// Helper function to render text
void RenderText(SDL_Renderer *renderer, TTF_Font *font, const std::string &text,
                int x, int y, SDL_Color color) {
  SDL_Surface *surface = TTF_RenderText_Solid(font, text.c_str(), color);
  if (!surface) {
    std::cerr << "Failed to create text surface: " << TTF_GetError()
              << std::endl;
    return;
  }

  SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
  if (!texture) {
    std::cerr << "Failed to create text texture: " << SDL_GetError()
              << std::endl;
    SDL_FreeSurface(surface);
    return;
  }

  SDL_Rect dstRect = {x, y, surface->w, surface->h};
  SDL_RenderCopy(renderer, texture, nullptr, &dstRect);

  SDL_DestroyTexture(texture);
  SDL_FreeSurface(surface);
}

int main(int argc, char *argv[]) {
  // Initialize SDL
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError()
              << std::endl;
    return 1;
  }

  // Initialize SDL_ttf
  if (TTF_Init() < 0) {
    std::cerr << "SDL_ttf could not initialize! SDL_Error: " << TTF_GetError()
              << std::endl;
    SDL_Quit();
    return 1;
  }

  // Create window and renderer
  SDL_Window *window = SDL_CreateWindow(
      "Dungeon Generator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
      WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
  if (!window) {
    std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError()
              << std::endl;
    TTF_Quit();
    SDL_Quit();
    return 1;
  }

  SDL_Renderer *renderer =
      SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (!renderer) {
    std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError()
              << std::endl;
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
    return 1;
  }

  // Load font
  TTF_Font *font =
      TTF_OpenFont("../assets/IosevkaNerdFontMono-Regular.ttf", FONT_SIZE);
  if (!font) {
    std::cerr << "Failed to load font! SDL_ttf Error: " << TTF_GetError()
              << std::endl;
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
    return 1;
  }

  // Load textures
  SDL_Surface *tileSurface = IMG_Load("../assets/output16.png");
  if (!tileSurface) {
    std::cerr << "Failed to load tile texture! SDL_Error: " << SDL_GetError()
              << std::endl;
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
    return 1;
  }

  SDL_Texture *tileTexture =
      SDL_CreateTextureFromSurface(renderer, tileSurface);
  SDL_FreeSurface(tileSurface);

  // Dungeon parameters
  int totalFloorCount = 1000;
  int minHall = 10;
  int maxHall = 20;
  int roomDim = 10;

  // Camera state
  float zoom = 1.0f;
  int cameraX = 0;
  int cameraY = 0;

  // Create initial dungeon
  WalkerDungeon dungeon(totalFloorCount, minHall, maxHall, roomDim);
  dungeon.GenerateRandomWalker();

  // Get floor and wall positions
  const std::vector<SDL_Point> &floors = dungeon.GetFloorList();
  std::vector<SDL_Point> walls = dungeon.GetWallList();

  // Main game loop
  bool running = true;
  SDL_Event event;
  SDL_Color textColor = {255, 255, 255, 255}; // White text

  while (running) {
    // Handle events
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_QUIT:
        running = false;
        break;

      case SDL_KEYDOWN:
        switch (event.key.keysym.sym) {
        case SDLK_ESCAPE:
        case SDLK_q:
          running = false;
          break;

        case SDLK_BACKSPACE:
          // Generate new dungeon with current parameters
          dungeon = WalkerDungeon(totalFloorCount, minHall, maxHall, roomDim);
          dungeon.GenerateRandomWalker();
          walls = dungeon.GetWallList();
          break;

        case SDLK_LEFT:
          cameraX += SCROLL_SPEED; // Move camera right (map moves left)
          break;

        case SDLK_RIGHT:
          cameraX -= SCROLL_SPEED; // Move camera left (map moves right)
          break;

        case SDLK_UP:
          cameraY += SCROLL_SPEED; // Move camera down (map moves up)
          break;

        case SDLK_DOWN:
          cameraY -= SCROLL_SPEED; // Move camera up (map moves down)
          break;

        case SDLK_KP_PLUS:
          zoom = std::min(zoom + ZOOM_SPEED, MAX_ZOOM);
          break;

        case SDLK_KP_MINUS:
          zoom = std::max(zoom - ZOOM_SPEED, MIN_ZOOM);
          break;

        // Parameter controls
        case SDLK_1:
          totalFloorCount = std::max(100, totalFloorCount - PARAM_STEP);
          break;
        case SDLK_2:
          totalFloorCount += PARAM_STEP;
          break;
        case SDLK_3:
          minHall = std::max(1, minHall - 1);
          break;
        case SDLK_4:
          minHall += 1;
          break;
        case SDLK_5:
          maxHall = std::max(minHall + 1, maxHall - 1);
          break;
        case SDLK_6:
          maxHall += 1;
          break;
        case SDLK_7:
          roomDim = std::max(1, roomDim - 1);
          break;
        case SDLK_8:
          roomDim += 1;
          break;
        }
        break;
      }
    }

    // Clear screen
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Calculate dungeon bounds
    int minX = 0, maxX = 0, minY = 0, maxY = 0;
    for (const auto &point : floors) {
      minX = std::min(minX, point.x);
      maxX = std::max(maxX, point.x);
      minY = std::min(minY, point.y);
      maxY = std::max(maxY, point.y);
    }

    // Calculate center offset with camera position and zoom
    int dungeonWidth = (maxX - minX + 1) * TILE_SIZE;
    int dungeonHeight = (maxY - minY + 1) * TILE_SIZE;

    int offsetX = (WINDOW_WIDTH - dungeonWidth * zoom) / 2 + cameraX;
    int offsetY = (WINDOW_HEIGHT - dungeonHeight * zoom) / 2 + cameraY;

    // Draw floors
    SDL_Rect srcRect = GetTileRect(5);
    for (const auto &point : floors) {
      SDL_Rect dstRect = {
          static_cast<int>(offsetX + (point.x - minX) * TILE_SIZE * zoom),
          static_cast<int>(offsetY + (point.y - minY) * TILE_SIZE * zoom),
          static_cast<int>(TILE_SIZE * zoom),
          static_cast<int>(TILE_SIZE * zoom)};
      if (SDL_RenderCopy(renderer, tileTexture, &srcRect, &dstRect) < 0) {
        std::cerr << "Failed to render floor tile! SDL_Error: "
                  << SDL_GetError() << std::endl;
      }
    }

    // Draw walls
    srcRect = GetTileRect(1);
    for (const auto &point : walls) {
      SDL_Rect dstRect = {
          static_cast<int>(offsetX + (point.x - minX) * TILE_SIZE * zoom),
          static_cast<int>(offsetY + (point.y - minY) * TILE_SIZE * zoom),
          static_cast<int>(TILE_SIZE * zoom),
          static_cast<int>(TILE_SIZE * zoom)};
      if (SDL_RenderCopy(renderer, tileTexture, &srcRect, &dstRect) < 0) {
        std::cerr << "Failed to render wall tile! SDL_Error: " << SDL_GetError()
                  << std::endl;
      }
    }

    // Render UI text on top of the map
    int yPos = UI_PADDING;
    RenderText(renderer, font, "Controls:", UI_PADDING, yPos, textColor);
    yPos += FONT_SIZE + UI_PADDING;
    RenderText(renderer, font,
               "1/2: Floor Count: " + std::to_string(totalFloorCount),
               UI_PADDING, yPos, textColor);
    yPos += FONT_SIZE + UI_PADDING;
    RenderText(renderer, font,
               "3/4: Min Hall Length: " + std::to_string(minHall), UI_PADDING,
               yPos, textColor);
    yPos += FONT_SIZE + UI_PADDING;
    RenderText(renderer, font,
               "5/6: Max Hall Length: " + std::to_string(maxHall), UI_PADDING,
               yPos, textColor);
    yPos += FONT_SIZE + UI_PADDING;
    RenderText(renderer, font,
               "7/8: Room Dimension: " + std::to_string(roomDim), UI_PADDING,
               yPos, textColor);
    yPos += FONT_SIZE + UI_PADDING;
    RenderText(renderer, font, "Backspace: Generate New Map", UI_PADDING, yPos,
               textColor);
    yPos += FONT_SIZE + UI_PADDING;
    RenderText(renderer, font, "Arrow Keys: Move Camera", UI_PADDING, yPos,
               textColor);
    yPos += FONT_SIZE + UI_PADDING;
    RenderText(renderer, font, "Keypad +/-: Zoom", UI_PADDING, yPos, textColor);
    yPos += FONT_SIZE + UI_PADDING;
    RenderText(renderer, font, "Q/Esc: Quit", UI_PADDING, yPos, textColor);

    // Update screen
    SDL_RenderPresent(renderer);
  }

  // Cleanup
  TTF_CloseFont(font);
  SDL_DestroyTexture(tileTexture);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  TTF_Quit();
  SDL_Quit();

  return 0;
}