#ifndef HUD__H
#define HUD__H
#include "gamedata.h"
#include "ioMod.h"



class Hud {
public:
  static Hud& getInstance();
  ~Hud();
  void drawHud(SDL_Renderer * const);
  const Gamedata& gdata;
  const IOmod& io;
private:
  
  Hud();
  Hud(const Hud&) = delete;
  Hud& operator=(const Hud&) = delete;

};
#endif
