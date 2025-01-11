//
// Created by mkowa on 27.08.2024.
//


#include <cstring>
#include <raylib.h> // by RaySan, Open Source license (zlib)
#include <algorithm>
#include "text.h"

[[nodiscard]] Vector2 centerPos(int len,float x, float y, int fontSize)
{
    Vector2 pos;
    pos.x=x-fontSize/4.0*len;
    pos.y=y-fontSize/2.0;
    return pos;
}

void drawTextEx(Font font, const char *text, float x, float y, float fontSize, float spacing, Color tint,bool relative,bool centered, bool scaled)
{
    Vector2 pos;
    pos.x=x;
    pos.y=y;
    if(relative)
    {
        pos.x=GetScreenWidth()/1000.0*(x);
        pos.y=GetScreenHeight()/1000.0*(y);
    }

    if(scaled) fontSize=fontSize * std::min(GetScreenHeight()/1000.0,GetScreenWidth()/1000.0);
    if(centered) pos = centerPos(strlen(text),pos.x,pos.y,fontSize);
    DrawTextEx(font,text,pos,fontSize,spacing,tint);
}