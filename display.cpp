//
// Created by mkowa on 02.04.2024.
//
#include "display.h"
#include <raylib.h>
void initScreen();
#ifdef _WIN32
display::os display::Screen::operating_system = display::os::Windows;
#elifdef __linux__
display::os display::Screen::operating_system = display::os::Unix;
#else
display::os display::Screen::operating_system = display::os::Other;

#endif
[[deprecated]] void display::initScreen() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(100,100,"Projekt PK2");
    MaximizeWindow();
    SetTargetFPS(60);

}
int display::Screen::getdx() const {
    return m_dx;
}

int display::Screen::getdy() const {
    return m_dy;
}

display::os display::Screen::getos() {
    return operating_system;
}

bool display::Screen::isActive() const {
    return m_isactive;
}

void display::Screen::stop() {
    m_isactive = false;

}

void display::Screen::start() {
    m_isactive = true;

}
Color display::colorGradient(float fade, Color color1, Color color2, Color color3) {

    // Do we have 3 colors for the gradient? Need to adjust the params.
    if (color3.a!=0) {
        fade = fade * 2;

        // Find which interval to use and adjust the fade percentage
        if (fade >= 1) {
            fade -= 1;
            color1 = color2;
            color2 = color3;
        }
    }
    int diffRed = color2.r - color1.r;
    int diffGreen = color2.g - color1.g;
    int diffBlue = color2.b - color1.b;

    Color result;
            result.r = (int)(color1.r + (diffRed * fade));
            result.g=(int)(color1.g + (diffGreen * fade));
            result.b= (int)(color1.b + (diffBlue * fade));
            result.a=255;

    return result;
}
display::Screen::Screen(int x, int y) : empty(BLACK) {

    m_dx = x, m_dy = y;
   buffer = new Color *[m_dx];
    for (int i = 0; i < m_dx; i++) {
        buffer[i] = new Color[m_dy];
        for (int j = 0; j < m_dy; j++) {
            buffer[i][j] = Color(0,0,0,0);
        }
    }
}

bool display::Screen::writeToBuffer(Color what, int x, int y ) {
    if (x < m_dx && x >= 0 && y >= 0 && y < m_dy) {
        buffer[x][y] = what;
        return 1;
    }
    return 0;
}

bool display::Screen::removeFromBuffer(int x, int y) {
    if (x < m_dx && x >= 0 && y >= 0 && y < m_dy) {
        buffer[x][y] = Color(0,0,0,0);
        return 1;
    }
    return 0;
}

void display::Screen::renderFrame(int sep)  {
    BeginDrawing();
    ClearBackground(Color(0,0,0,255));
    int gridHeight, gridWidth;
    gridWidth=(GetScreenWidth()-m_dx*sep)/m_dx;
    gridHeight=(GetScreenHeight()-m_dy*sep)/m_dy;
    for (int i = 0; i < m_dy; i++) {
        for (int j = 0; j < m_dx; j++) {
           // if(buffer[j][i].a==0){ buffer[j][i]=empty; buffer[j][i].a= GetRandomValue(buffer[j][i].a-60,buffer[j][i].a-40);}
                DrawRectangle(0+j*(gridWidth+sep), 0+i*(gridHeight+sep),gridWidth, gridHeight, buffer[j][i]);
        }
    }
    EndDrawing();
}
void display::Screen::renderFrame(RenderTexture2D &rendertexture, int sep)  {
    int gridHeight, gridWidth;
    gridWidth=(rendertexture.texture.width-m_dx*sep)/m_dx;
    gridHeight=(rendertexture.texture.height-m_dy*sep)/m_dy;
    for (int i = 0; i < m_dy; i++) {
        for (int j = 0; j < m_dx; j++) {
            if(buffer[j][i].a==0){ buffer[j][i]=empty; buffer[j][i].a= GetRandomValue(buffer[j][i].a-60,buffer[j][i].a-40);}
            DrawRectangle(0+j*(gridWidth+sep), 0+i*(gridHeight+sep),gridWidth, gridHeight, buffer[j][i]);
        }
    }
}

[[maybe_unused]] void display::Screen::clearBuffer() {
    for (int i = 0; i < m_dy; i++) {
        for (int j = 0; j < m_dx; j++) {
            buffer[i][j] = Color(0,0,0,0);
        }
    }
}
display::Screen::~Screen() {
    for (int i = 0; i < m_dy; i++) {
        delete[] buffer[i];
    }
    delete[] buffer;
    buffer = nullptr;
}