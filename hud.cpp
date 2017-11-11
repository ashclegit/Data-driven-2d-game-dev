#include <string>
#include <sstream>
#include "SDL.h"
#include "hud.h"


Hud& Hud::getInstance() {
  static Hud instance;
  return instance;
}

Hud::Hud():gdata(Gamedata::getInstance()),io(IOmod::getInstance()) 
{
  
}

Hud::~Hud()
{

}

void Hud::drawHud(SDL_Renderer * const renderer )
{
    SDL_Rect hudLayout;
    hudLayout.x = gdata.getXmlInt("hudLayout/xpos");
    hudLayout.y = gdata.getXmlInt("hudLayout/ypos");
    hudLayout.w = gdata.getXmlInt("hudLayout/w");
    hudLayout.h = gdata.getXmlInt("hudLayout/h");

    SDL_Color bgColor;
    bgColor.r = gdata.getXmlInt("hudColor/red");
    bgColor.g = gdata.getXmlInt("hudColor/green");
    bgColor.b = gdata.getXmlInt("hudColor/blue");
    bgColor.a = gdata.getXmlInt("hudColor/alpha");


    SDL_SetRenderDrawColor(renderer,bgColor.r,bgColor.g,bgColor.b,bgColor.a);
    SDL_RenderFillRect(renderer,&hudLayout);

    SDL_Color textColor;
    textColor.r = gdata.getXmlInt("font/red");
    textColor.g = gdata.getXmlInt("font/green");
    textColor.b = gdata.getXmlInt("font/blue");
    textColor.a = gdata.getXmlInt("font/alpha");

    io.writeText(gdata.getXmlStr("hudLayout/text").c_str(),hudLayout.x+=15,hudLayout.y+=15,textColor);
    io.writeText(gdata.getXmlStr("hudLayout/right").c_str(),hudLayout.x,hudLayout.y+=30,textColor);
    io.writeText(gdata.getXmlStr("hudLayout/left").c_str(),hudLayout.x,hudLayout.y+=30,textColor);
    io.writeText(gdata.getXmlStr("hudLayout/up").c_str(),hudLayout.x,hudLayout.y+=30,textColor);
    io.writeText(gdata.getXmlStr("hudLayout/down").c_str(),hudLayout.x,hudLayout.y+=30,textColor);
}