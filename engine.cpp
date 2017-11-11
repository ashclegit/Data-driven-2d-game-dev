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

  for ( Drawable* sprite : spriteVec ) {
    delete sprite;
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
  player(new Player("Fighter")),
  spriteVec(),
  strategies(),
  currentStrategy(0),
  hu(Hud::getInstance()),
  collision(false),
  showHud(false),
  makeVideo( true )
{
  spriteVec.reserve(2);
  //spriteVec.emplace_back(new Sprite("Ball"));
  //spriteVec.emplace_back(new Player("Fighter"));
  //spriteVec.emplace_back(new TwoWaySprite("FlyingBird"));
  //sprites.reserve(2);
  /*Vector2f pos = player->getPosition();
  int w = player->getScaledWidth();
  int h = player->getScaledHeight();
  sprites.push_back( new SmartSprite("Ball", pos, w, h) );
  player->attach( sprites[0] );*/
  spriteVec.emplace_back(new TwoWaySprite("FlyingBird"));
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

  if (showHud || clock.getSeconds() < 3)
  {
  	hu.drawHud(renderer);
  }
  /*for(auto spritePtr : spriteVec)
  {
    spritePtr->draw();
  }*/


  IOmod::getInstance().writeText("Press m to change strategy", 500, 60);
  for ( const Drawable* sprite : spriteVec ) {
    sprite->draw();
  }

  strategies[currentStrategy]->draw();
  if ( collision ) {
    std::cout << "In collision" << std::endl;
    IOmod::getInstance().writeText("Oops: Collision", 500, 90);
  }

  player->draw();
  /*float fps = clock.getFps();

  fpstr << "FrameRate: " << fps;

  io.writeText(fpstr.str(),30,55);

  std::string uname = "Ashwin Kumar Vajantri";

  SDL_Color nameColor = {255,69,0,0};

  io.writeText(uname,30,450,nameColor);*/

  viewport.draw();
  SDL_RenderPresent(renderer);
}

/*void Engine::checkForCollisions() {
  auto it = spriteVec.begin();
  while ( it != spriteVec.end() ) {
    if ( strategies[currentStrategy]->execute(*player, **it) ) {
      SmartSprite* doa = *it;
      player->detach(doa);
      delete doa;
      it = sprites.erase(it);
    }
    else ++it;
  }
}*/

void Engine::update(Uint32 ticks) {

  //checkForCollisions();
  for ( Drawable* sprite : spriteVec ) {
    sprite->update( ticks );
  }
  player->update(ticks);
  
  trees.update();
  snowhill.update(); 
  viewport.update(); // always update viewport last
}



/*void Engine::switchSprite(){
  ++currentSprite;
  currentSprite = currentSprite % 2;
  if ( currentSprite ) {
    Viewport::getInstance().setObjectToTrack(spriteVec[currentSprite]);
  }
  else {
    Viewport::getInstance().setObjectToTrack(spriteVec[currentSprite]);
  }
}*/



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
        /*if ( keystate[SDL_SCANCODE_T] ) {
          //switchSprite();
        }*/
        if ( keystate[SDL_SCANCODE_M] ) {
          currentStrategy = (1 + currentStrategy) % strategies.size();
        }
        if (keystate[SDL_SCANCODE_F1] && !showHud)
        {
        	showHud = true;
        }
        else if(keystate[SDL_SCANCODE_F1] && showHud) {
        	showHud = false;
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
