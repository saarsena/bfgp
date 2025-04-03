#pragma once

// Define event types
enum class GameEventType {
  PLAYER_MOVED,
  PLAYER_COLLISION,
  ENTITY_SPAWN,
  ENTITY_DESTROY,
  MAP_CHANGED
};

// Base event class
class GameEvent {
public:
  explicit GameEvent(GameEventType type) : type(type) {}
  virtual ~GameEvent() = default;

  GameEventType getType() const { return type; }

private:
  GameEventType type;
};

// Player movement event
class PlayerMovedEvent : public GameEvent {
public:
  PlayerMovedEvent(int newX, int newY)
      : GameEvent(GameEventType::PLAYER_MOVED), newX(newX), newY(newY) {}

  int newX;
  int newY;
};