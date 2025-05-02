#pragma once

#include <SDL2/SDL.h>
#include <memory>
#include <string>

namespace Core {

class GraphicsContext {
public:
  GraphicsContext(const std::string &title, int width, int height,
                  Uint32 flags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
  ~GraphicsContext();

  // Delete copy constructor and assignment operator
  GraphicsContext(const GraphicsContext &) = delete;
  GraphicsContext &operator=(const GraphicsContext &) = delete;

  // Allow move operations
  GraphicsContext(GraphicsContext &&other) noexcept;
  GraphicsContext &operator=(GraphicsContext &&other) noexcept;

  SDL_Window *getWindow() const { return window; }
  SDL_Renderer *getRenderer() const { return renderer; }

  void setWindowTitle(const std::string &title);
  void getWindowSize(int &width, int &height) const;
  void setWindowSize(int width, int height);

private:
  SDL_Window *window;
  SDL_Renderer *renderer;
};

} // namespace Core