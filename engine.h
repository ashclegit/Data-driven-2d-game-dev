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

class Engine {
public:
  Engine ();
  ~Engine ();
  void play();

private:
  const RenderContext* rc;
  const IOmod& io;
  Clock& clock;


  SDL_Renderer * const renderer;
  World trees;
  World snowhill;
  Viewport& viewport;
  
  SubjectSprite* player;
  std::vector<SmartSprite*> spriteVec;
  std::vector<Drawable*> sprites;
  std::vector<CollisionStrategy*> strategies;


  int currentStrategy;

  Hud& hu;
  bool collision;
  bool displayHud;
  bool makeVideo;

  void draw() const;
  void update(Uint32);

  Engine(const Engine&) = delete;
  Engine& operator=(const Engine&) = delete;
  void printScales() const;
  void checkForCollisions();
};
