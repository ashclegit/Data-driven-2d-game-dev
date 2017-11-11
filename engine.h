#include <vector>
#include <SDL.h>
#include "ioMod.h"
#include "renderContext.h"
#include "clock.h"
#include "world.h"
#include "viewport.h"
#include "player.h"
#include "hud.h"
#include "smartSprite.h"
#include "subjectSprite.h"
#include "collisionStrategy.h"

class CollisionStrategy;
class SmartSprite;
class SubjectSprite;

class Engine {
public:
  Engine ();
  ~Engine ();
  void play();
  //void switchSprite();

private:
  const RenderContext* rc;
  const IOmod& io;
  Clock& clock;


  SDL_Renderer * const renderer;
  World trees;
  World snowhill;
  Viewport& viewport;
  
  Player* player;
  std::vector<Drawable*> spriteVec;
  std::vector<CollisionStrategy*> strategies;


  int currentStrategy;

  Hud& hu;
  bool collision;
  bool showHud;
  bool makeVideo;

  void draw() const;
  void update(Uint32);

  Engine(const Engine&) = delete;
  Engine& operator=(const Engine&) = delete;
  void printScales() const;
  void checkForCollisions();
};
