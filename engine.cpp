#include <iostream>
#include <algorithm>
#include <sstream>
#include <string>
#include <random>
#include <iomanip>
#include <vector>
#include <SDL_ttf.h>
#include <SDL.h>
#include "sprite.h"
#include "player.h"
#include "multisprite.h"
#include "twoWaySprite.h"
#include "gamedata.h"
#include "engine.h"
#include "frameGenerator.h"
#include "hud.h"

Engine::~Engine() { 

  delete player;

  for ( SmartSprite* sprite : spriteVec ) {
    delete sprite;
  }

  for ( Drawable* simplesprite : sprites ) {
    delete simplesprite;
  }


  for ( CollisionStrategy* strategy : strategies ) {
    delete strategy;
  }
  std::cout << "Terminating program" << std::endl;

}

Engine::Engine() :
  rc( RenderContext::getInstance() ),
  io( IOmod::getInstance() ),
  clock( Clock::getInstance() ),
  renderer( rc->getRenderer() ),
  trees("trees", Gamedata::getInstance().getXmlInt("trees/factor") ),
  snowhill("snowhill", Gamedata::getInstance().getXmlInt("snowhill/factor") ),
  viewport( Viewport::getInstance() ),
  player(new SubjectSprite("Fighter")),
  spriteVec(),
  sprites(),
  strategies(),
  currentStrategy(0),
  hu(Hud::getInstance()),
  collision(false),
  displayHud(false),
  makeVideo( true )
{
  spriteVec.reserve(4);
  sprites.reserve(2);
  Vector2f pos = player->getPosition();
  int w = player->getScaledWidth();
  int h = player->getScaledHeight();
  int n = Gamedata::getInstance().getXmlInt("FlyingBird/enemies");

  for ( int i=0 ; i < 2 ; i++)
  {
    sprites.push_back( new Sprite("Ball"));
  }

  //sprites.push_back( new Sprite("Ball"));
  for ( int i=0 ; i < n ; i++)
  {
    spriteVec.push_back( new SmartSprite("FlyingBird", pos, w, h) );
    player->attach( spriteVec[i] );
  }
  strategies.push_back( new RectangularCollisionStrategy );
  strategies.push_back( new PerPixelCollisionStrategy );
  strategies.push_back( new MidPointCollisionStrategy );

  Viewport::getInstance().setObjectToTrack(player);
  std::cout << "Loading complete" << std::endl;
}

void Engine::draw() const {
  std::stringstream fpstr;

 
  snowhill.draw();
  trees.draw();

  if (displayHud || clock.getSeconds() < 3)
  {
  	hu.drawHud(renderer);
  }
  for(auto spritePtr : sprites)
  {
    spritePtr->draw();
  }


  IOmod::getInstance().writeText("Press m to change strategy", 30, 25);
  for ( const Drawable* sprite : spriteVec ) {
    sprite->draw();
  }
  std::stringstream strm;
  strm << spriteVec.size() << " Smart Sprites Remaining";
  IOmod::getInstance().writeText(strm.str(), 30, 125);

  strategies[currentStrategy]->draw();
  if ( collision ) {
    IOmod::getInstance().writeText("Oops: Collision", 30, 50);
  }

  player->draw();
  float fps = clock.getFps();

  fpstr << "FrameRate: " << fps;

  io.writeText(fpstr.str(),30,100);

  std::string uname = "Ashwin Kumar Vajantri";

  SDL_Color nameColor = {255,255,0,0};

  io.writeText(uname,30,450,nameColor);

  //viewport.draw();
  SDL_RenderPresent(renderer);
}

void Engine::checkForCollisions() {
  collision = false;
  for ( const SmartSprite* d : spriteVec ) {
    if ( strategies[currentStrategy]->execute(*player, *d) ) {
      collision = true;
    }
  }
  for ( const Drawable* d : sprites ) {
    if ( strategies[currentStrategy]->execute(*player, *d) ) {
      collision = true;
    }
  }

  if ( collision ) {
    player->collided();
  }
  else {
    player->missed();
    collision = false;
  }
  auto it = spriteVec.begin();
  while ( it != spriteVec.end() ) {
    if ( strategies[currentStrategy]->execute(*player, **it) ) {
      SmartSprite* doa = *it;
      player->detach(doa);
      delete doa;
      it = spriteVec.erase(it);
    }
    else ++it;
  }
}

void Engine::update(Uint32 ticks) {

  checkForCollisions();
  for ( SmartSprite* sprite : spriteVec ) {
    sprite->update( ticks );
  }
  player->update(ticks);
  for ( Drawable* simplesprite : sprites ) {
    simplesprite->update( ticks );
  }

  trees.update();
  snowhill.update(); 
  viewport.update(); // always update viewport last
}

void Engine::play() {
  SDL_Event event;
  const Uint8* keystate;
  bool done = false;
  Uint32 ticks = clock.getElapsedTicks();
  FrameGenerator frameGen;

  while ( !done ) {
    // The next loop polls for events, guarding against key bounce:
    while ( SDL_PollEvent(&event) ) {
      keystate = SDL_GetKeyboardState(NULL);
      if (event.type ==  SDL_QUIT) { done = true; break; }
      if(event.type == SDL_KEYDOWN) {
        if (keystate[SDL_SCANCODE_ESCAPE] || keystate[SDL_SCANCODE_Q]) {
          done = true;
          break;
        }
        if ( keystate[SDL_SCANCODE_P] ) {
          if ( clock.isPaused() ) clock.unpause();
          else clock.pause();
        }
        if ( keystate[SDL_SCANCODE_M] ) {
          currentStrategy = (1 + currentStrategy) % strategies.size();
        }
        if (keystate[SDL_SCANCODE_F1] && !displayHud)
        {
        	displayHud = true;
        }
        else if(keystate[SDL_SCANCODE_F1] && displayHud) {
        	displayHud = false;
        }

        if (keystate[SDL_SCANCODE_F4] && !makeVideo) {
          std::cout << "Initiating frame capture" << std::endl;
          makeVideo = true;
        }
        else if (keystate[SDL_SCANCODE_F4] && makeVideo) {
          std::cout << "Terminating frame capture" << std::endl;
          makeVideo = false;
        }
      }
    }

    // In this section of the event loop we allow key bounce:

    ticks = clock.getElapsedTicks();
    if ( ticks > 0 ) {
      clock.incrFrame();
      if (keystate[SDL_SCANCODE_A]) {
        player->left();
      }
      if (keystate[SDL_SCANCODE_D]) {
        player->right();
      }
      if (keystate[SDL_SCANCODE_W]) {
        player->up();
      }
      if (keystate[SDL_SCANCODE_S]) {
        player->down();
      }
      draw();
      update(ticks);
      if ( makeVideo ) {
        frameGen.makeFrame();
      }
    }
  }
}
