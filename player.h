#ifndef PLAYER__H
#define PLAYER__H


#include "twoWaySprite.h"

class Player : public TwoWaySprite {
public:
  Player(const std::string&);
  Player(const Player&);
  virtual void update(Uint32 ticks);

  void collided() { collision = true; }
  void missed() { collision = false; }
  Player& operator=(const Player&);

  void right();
  void left();
  void up();
  void down();
  void stop();
private:
  bool collision;
  Vector2f initialVelocity;
};
#endif
