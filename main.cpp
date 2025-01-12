#include <iostream>
#include <raylib.h> // by RaySan, Open Source license (zlib)
#include <string>
#include "display.h"
#include <charconv>
#include <queue>
#include "perlin.h"
#include "perlin.cpp"
#include <fstream>
#define RAYGUI_IMPLEMENTATION // Required for GUI controls
#include "raygui.h" // GUI controls by RaySan, Open Source license (zlib)
#include "tinyfiledialogs.h" // Native file dialogs by Guillaume Vareille http://ysengrin.com, Open Source license (zlib)
#include "style.rgs.h"
#include "text.h"
#include "ecs.h"
#include "bilist.h"
/**
 * @file Plik główny projektu. Zawiera pętle główne każdego ekranu programu.
 * @file ecs.h Służy obsłudze stanu gry. Zawiera wszystkie elementy ECS.
 * @file display.h Służy obsłudze wyświetlania stanu gry. Korzysta z biblioteki Raylib.
 * @file coinflip.h Służy generowaniu liczb losowych, a konkretnie symulowania rzutów monetą / kostką procentową
 * @file perlin.h Służy generowaniu szumu Perlina.
 */
/**
 * @brief Wyświetla nową symulację ognia spełniającej podane parametry startowe.
 * @param vars parametry symulacji, które mają zostać załadowane
 * @param font czcionka do renderowania tekstu
 */
void startFireSim(ecs::firevars<double> vars, Font font)
{
    perlin::PerlinGenerator<double> pg;
    EndDrawing();
    display::Screen testscreen(vars.dx,vars.dy);
    ecs::Game test(testscreen);
    RenderTexture2D renderTexture = LoadRenderTexture(vars.dx*16, vars.dy*16);
    std::vector<double> angles = perlin::generateRandomAngles<double>();
    Vector2 pos;
    double f;
    for(int i=0;i < vars.dx;i++)
    {
        for(int j=0;j<vars.dy;j++)
        {
            f=pg.getMultiOctaveNoise2D(i, j, 4, angles);
            if(f>(1-(vars.forest)))test.getRegistry().displayable_positions.emplace(test.createEntity(), ecs::position_component{j,i,'T',Color (0,f*228,f*48,GetRandomValue(255-60,255))});
        }
    }
    int prevfire = 0;
    float firer=0;
    int count=0;
    bool debug =false;
    size_t fire;
    timer::Timer<double> pollTimer{std::chrono::seconds(1)};
    while (test.isRunning()) {
        test.update(testscreen);
        testscreen.empty = GREEN;
        while (!WindowShouldClose()) {

            std::cout << test.getRegistry().displayable_positions.size() << std::endl;
            test.update(testscreen);
            BeginTextureMode(renderTexture);
            ClearBackground(BLACK);
            testscreen.renderFrame(renderTexture);
            pos = GetMousePosition();
            pos.x = (int)(pos.x / ((1.f*GetScreenWidth()) / vars.dx));
            pos.y = (int)(pos.y / ((1.f * GetScreenHeight()) / vars.dy));
            if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {

                drawTextEx(font, TextFormat("Click X: %f Click Y: %f", pos.x, pos.y), 0, 0,
                           16, 0, WHITE,0,0,1);
                if (test.getRegistry().entitypos.contains({pos.x, pos.y}))
                    test.removeEntity(test.getRegistry().entitypos.at({pos.x, pos.y}));
                fire = test.createEntity();
                test.getRegistry().displayable_positions.emplace(fire, ecs::position_component{(int)pos.x, (int)pos.y, '*',
                                                                                               Color(255, 0, 0, 255)});
                test.getRegistry().fires.emplace(fire, ecs::fire_component(std::unordered_set<char>({'T', 'B'}),
                                                                           vars.fire_spread_time, vars.fire_burn_time,
                                                                           vars.fire_spread_chance));
                test.fire_system.firequeue.push(fire);
            }
            EndTextureMode();
            BeginDrawing();
            ClearBackground(BLACK);
            DrawTexturePro(renderTexture.texture, Rectangle{0, 0, renderTexture.texture.width * 1.f,
                                                            -1.f * renderTexture.texture.height},
                           Rectangle{0, 0, 1.f * GetScreenWidth(), 1.f * GetScreenHeight()},
                           (Vector2) {0, 0}, 0.0f, WHITE);
            if(IsKeyReleased(KEY_Q))
            {
                if(debug)
                debug=false;
                else debug=true;
            }
            if(pollTimer.isFinished())
            {
                if (test.getRegistry().fires.size() == 0 || prevfire == 0) firer = 0;
                else firer = test.getRegistry().fires.size() *1.f / prevfire * 1.f;
                pollTimer.reset();
                count = test.getRegistry().fires.size()-prevfire;
                prevfire=test.getRegistry().fires.size();
            }
            if(debug)
            {
                DrawRectangle(GetScreenWidth()-72,0,72,16,BLACK);
                DrawFPS(GetScreenWidth()-72,0);
                drawTextEx(font, TextFormat("Lb. wyswietlanych komorek: %i",test.getRegistry().displayable_positions.size()),0,0,32,0,RAYWHITE,0,0,1);
                drawTextEx(font, TextFormat("Lb. komorek ognia: %i",test.getRegistry().fires.size()),0,16,32,0,RAYWHITE,0,0,1);
                drawTextEx(font, TextFormat("Zmiana ogni w ostatnich 5s: %i (%i%c)",count,(int)(firer*100)-100,'%'),0,32,32,0,WHITE,0,0,1);
            }

            EndDrawing();

        }
        test.stop();
    }

}
/**
 * @brief Wyświetla menu konfiguracyjne symulacji ognia
 * @param font czcionka do renderowania tekstu
 */
void openFireConfigMenu(Font font)
{
    EndDrawing(); // Render buffered menu frame
    ecs::firevars<double> vars;
    // GUI variables
    bool dy_editMode = false;
    int dy = 250;
    bool dx_editMode = false;
    int dx = 250;
    float spreadchancef = 20;
    float forestf = 50;
    float burntimef = 0.4;
    float spreadtimef = 0.1;
    char burntimec[64] = "";
    char spreadtimec[64] = "";
    bool burntime_editMode = false;
    bool spreadtime_editMode = false;
    bool scaledown;
    float hr,vr;
    while(!WindowShouldClose())
    {
        hr=GetScreenWidth()/1000.f;
        vr=GetScreenHeight()/1000.f;
        BeginDrawing();
        (GetScreenWidth()/1000.f<1)?scaledown = true:scaledown = false;
        ClearBackground(BLACK);
        // Draw text
        drawTextEx(font,TextFormat("Symulacja Ognia"),500,64,32,0,RED,1,1,0);
        drawTextEx(font,TextFormat("Wymiary planszy:"),0,128,48,0,RAYWHITE,1,0,1);
        drawTextEx(font,TextFormat("Szansa zaplonu:"),0,128+64,48,0,RAYWHITE,1,0,1);
        drawTextEx(font,TextFormat("Czas zycia ognia:"),0,128+64*2,48,0,RAYWHITE,1,0,1);
        drawTextEx(font,TextFormat("Interwal skoku:"),0,128+64*3,48,0,RAYWHITE,1,0,1);
        drawTextEx(font,TextFormat("Zalesienie:"),0,128+64*4,48,0,RAYWHITE,1,0,1);
        // Handle buttons
        if (GuiSpinner((Rectangle){ 500.0f*hr, 128*vr, 256*std::fmin(vr,hr), 64*std::fmin(vr,hr)} , "X ", &dx, 5, 1000, dx_editMode)) dx_editMode = !dx_editMode;
        if (GuiSpinner((Rectangle){ scaledown?780.f*hr:(500.0f*hr+200.f), 128*vr,fmin(256*std::fmin(hr,vr),GetScreenWidth()-(scaledown?780.f*hr:(500.0f*hr+200.f))), 64*std::fmin(hr,vr)}, "Y ", &dy, 5, 1000, dy_editMode)) dy_editMode = !dy_editMode;
        GuiSlider((Rectangle){ 500.f*hr, (128+64)*vr, 256*std::fmin(hr,vr), 64*std::fmin(hr,vr)},
                  TextFormat("%i%s",(int)spreadchancef,"%"),"", &spreadchancef, 0, 100);

        std::to_chars(&burntimec[0],&burntimec[63],burntimef);
        if (GuiTextBox((Rectangle){ 500.f*hr, (128+64*2)*vr, 256*std::fmin(hr,vr), 64*std::fmin(hr,vr)},burntimec,8,burntime_editMode)) burntime_editMode=!burntime_editMode;
        burntimef=std::atof(burntimec);
        std::to_chars(&spreadtimec[0],&spreadtimec[63],spreadtimef);
        if (GuiTextBox((Rectangle){ 500.f*hr, (128+64*3)*vr, 256*std::fmin(hr,vr), 64*std::fmin(hr,vr)},spreadtimec,8,spreadtime_editMode))spreadtime_editMode=!spreadtime_editMode;
        spreadtimef=std::atof(spreadtimec);
        GuiSlider((Rectangle){ 500.f*hr, (128+64*4)*vr, 256*std::fmin(hr,vr), 64*std::fmin(hr,vr)},
                  TextFormat("%i%s",(int)forestf,"%"),"", &forestf, 0, 100);
        if(GuiButton((Rectangle){scaledown?780.f*hr:(500.0f*hr+200.f), (128+64*5)*vr, fmin(256*std::fmin(hr,vr),GetScreenWidth()-(scaledown?780.f*hr:(500.0f*hr+200.f))), 64*std::fmin(vr,hr)}, GuiIconText(ICON_UNDO,"Powrót"))) {
            break;
        }
        if(GuiButton((Rectangle){ 0*hr, (128+64*5)*vr, 256*std::fmin(vr,hr), 64*std::fmin(vr,hr)}, GuiIconText(ICON_PLAYER_PLAY,"Start"))) {
          vars.dx=dx;
          vars.dy=dy;
          vars.fire_spread_chance=spreadchancef;
          vars.forest=forestf/100;
          vars.fire_burn_time=burntimef;
          vars.fire_spread_time=spreadtimef;
          // Start FireSim
          startFireSim(vars, font);

        };

            EndDrawing();
        }
}
/**
 * @brief Otwiera systemowy dialog wyboru pliku do otworzenia
 * @param filename ścieżka do pliku wykonawczego (pierwszy argument maina)
 * @return ścieżkę pliku do otwarcia
 * @warning Program zakłada, że podfloder /saves jest w katalogu którego podfolderem jest katalog zawierający plik wykonawczy programu!
 */
char *openFileDialog(std::string filename)
{
    filename=filename.substr(0,filename.find_last_of("\\/"));
    filename=filename.substr(0,filename.find_last_of("\\/"));
#ifdef __linux
    filename+="/saves/";
#elifdef WIN32
    filename+="\\saves\\";
#endif
    char const * lFilterPatterns[1] = { "*.gol"};
    return tinyfd_openFileDialog("Wybieranie pliku",filename.c_str(),1,lFilterPatterns,"Pliki zapisu GOL",0);

}
/**
 * @brief Otwiera systemowy dialog zapisywania pliku
 * @param filename ścieżka do pliku wykonawczego (pierwszy argument maina)
 * @return ścieżkę pliku do zapisania
 * @warning Program zakłada, że podfloder /saves jest w katalogu którego podfolderem jest katalog zawierający plik wykonawczy programu!
 */
char *saveFileDialog(std::string filename)
{
    filename=filename.substr(0,filename.find_last_of("\\/"));
    filename=filename.substr(0,filename.find_last_of("\\/"));
    if(display::Screen::getos()==display::os::Unix)
    filename+="/saves/";
    else
    filename+="\\saves\\";
    char const * lFilterPatterns[1] = { "*.gol"};
    return tinyfd_saveFileDialog("Zapisywanie pliku",filename.c_str(),1,lFilterPatterns,"Pliki zapisu GOL");
}
/**
 * @brief Rozpoczyna wyświetlanie i obsługę symulacji "Gry w życie" Conway'a
 * @param filename ścieżka do pliku wykonawczego (pierwszy argument maina)
 * @param vars parametry planszy gry w życie do załadowania/utworzenia
 * @param sep odstęp wyświetlany pomiędzy komórkami
 */
void startLife(std::string filename, ecs::lifevars vars,int sep=1)
{
    std::cout<<vars;
    std::fstream stream;
    display::Screen testscreen(vars.dx,vars.dy);
    ecs::Game test(testscreen);
    bool started=false;

    for(int i=0;i < vars.dx;i++)
    {
        for(int j=0;j<vars.dy;j++)
        {
            size_t ent=test.createEntity();
            test.getRegistry().lives.emplace(ent, ecs::life_component(false));
            test.getRegistry().displayable_positions.emplace(ent, ecs::position_component(i, j, '0',BLACK));

        }
    }
    Vector2 pos,lastpos;
    float dur = 1;
    lastpos={-1,-1};
    RenderTexture2D renderTexture = LoadRenderTexture(vars.dx*16, vars.dy*16);
    test.update(testscreen);
    for(auto a : vars.alives)
    {
        test.game_life_system.switchState(test.getRegistry().entitypos.at({a.first,a.second}),test.getRegistry());
    }
    while(test.isRunning())
    {
        test.update(testscreen);
        if(started) {
            stream.open("cache.temp", std::ios_base::in);
            int x,y;
            if(stream.is_open())
            {
                while(stream>>x>>y)
                {
                     test.game_life_system.switchState(test.getRegistry().entitypos.at({x,y}),test.getRegistry());

                }
                stream.close();
            }
            else tinyfd_messageBox("Błąd: Nie można przywrócić stanu","Plik .temp nie znaleziony lub uszkodzony!","ok","error",1);
        }
        started=false;

        // Loaded
        while(!WindowShouldClose())
        {
            BeginTextureMode(renderTexture);
            ClearBackground(GRAY);
            testscreen.renderFrame(renderTexture,2);
            pos = GetMousePosition();
            pos.x = (int)(pos.x / ((1.f*GetScreenWidth()) / vars.dx));
            pos.y = (int)(pos.y / ((1.f * GetScreenHeight()-32) / vars.dy));
            if (!started&&IsMouseButtonDown(MOUSE_BUTTON_LEFT)&&(pos.x!=lastpos.x||pos.y!=lastpos.y)) {
                if (test.getRegistry().entitypos.contains({pos.x, pos.y}))
                    ecs::life_system::switchState(test.getRegistry().entitypos.at({pos.x, pos.y}),test.getRegistry());
                lastpos=pos;
            }
            if(!started&&!IsMouseButtonDown(MOUSE_BUTTON_LEFT)) lastpos = Vector2(-1,-1);
            EndTextureMode();
            test.update(testscreen);
            BeginDrawing();
            ClearBackground(BLACK);
            DrawTexturePro(renderTexture.texture, Rectangle{0, 0, renderTexture.texture.width * 1.f,
                                                            -1.f * renderTexture.texture.height},
                           Rectangle{0, 0, 1.f * GetScreenWidth(), 1.f * GetScreenHeight()-32},
                           (Vector2) {0, 0}, 0.0f, WHITE);
            if(GuiButton (Rectangle{0,GetScreenHeight()-32.f,GetScreenWidth()/4.0f,32}, GuiIconText(test.game_life_system.isPaused() ? ICON_PLAYER_PLAY : ICON_PLAYER_PAUSE, test.game_life_system.isPaused() ? "Odpauzuj" : "Zapauzuj")) || IsKeyReleased(KEY_SPACE))
            {
                if(test.game_life_system.isPaused()) {
                    // Save state to temp file
                    if(!started) {
                        stream.open("cache.temp", std::ios_base::out);
                        for (auto &[e, r]: test.getRegistry().lives) {
                            if (test.getRegistry().lives.at(e).cellstate) {
                                if (test.getRegistry().displayable_positions.contains(e)) {
                                    stream << test.getRegistry().displayable_positions.at(e).posx << " "
                                           << test.getRegistry().displayable_positions.at(e).posy << std::endl;
                                }
                            }
                        }
                        stream.close();
                        started=true;
                    }
                    test.game_life_system.unpause();
                    }
                else test.game_life_system.pause();
            }
            if(test.game_life_system.isPaused() && GuiButton(Rectangle{GetScreenWidth() / 4.0f, GetScreenHeight() - 32.f, GetScreenWidth() / 4.0f, 32}, GuiIconText(ICON_STEP_OVER, "Krok")) || IsKeyDown(KEY_E))
            {
                if(!started) {
                    stream.open("cache.temp", std::ios_base::out);
                    for (auto &[e, r]: test.getRegistry().lives) {
                        if (test.getRegistry().lives.at(e).cellstate) {
                            if (test.getRegistry().displayable_positions.contains(e)) {
                                stream << test.getRegistry().displayable_positions.at(e).posx << " "
                                       << test.getRegistry().displayable_positions.at(e).posy << std::endl;
                            }
                        }
                    }
                    stream.close();

                    started=true;
                }
                test.game_life_system.update(test.getRegistry(),1);
                started=true;
            }
            if(!started&&GuiButton (Rectangle{2*GetScreenWidth()/4.0f,GetScreenHeight()-32.f,GetScreenWidth()/4.0f,32}, GuiIconText(ICON_FILE_SAVE,"Zapisz"))||IsKeyDown(KEY_LEFT_CONTROL)&&IsKeyPressed(KEY_S))
            {
                char *file = saveFileDialog(filename);
                if(file==NULL) tinyfd_messageBox("Błąd: Nie można zapisać stanu","Błąd zapisu pliku!","ok","error",1);
                else {test.game_life_system.saveState(test.getRegistry(),file,vars.dx,vars.dy);}
            }
            if(started) GuiSlider(Rectangle{7*GetScreenWidth()/12.0f,GetScreenHeight()-32.f,(2*GetScreenWidth()/12.0f),32},
                      TextFormat("%.2f%s",dur,"s"),"", &dur, 0.02, 5);
            if(test.game_life_system.getDur() != dur && !IsMouseButtonDown(MOUSE_BUTTON_LEFT))
            {
                test.game_life_system.changeDur(dur);
                test.game_life_system.update(test.getRegistry(),1);
            };
            if(GuiButton (Rectangle{3*GetScreenWidth()/4.0f,GetScreenHeight()-32.f,GetScreenWidth()/4.0f,32}, GuiIconText(started?ICON_UNDO:ICON_EXIT,started?"Powrót":"Wyjscie")))
            {
                if(!started) test.stop();
                EndDrawing();
                test.game_life_system.pause();
                break;
            }
                EndDrawing();
        }
        if(!started) break;
        for(auto &a : test.getRegistry().lives)
        {
            a.second.cellstate=0;
            test.getRegistry().displayable_positions.at(a.first).col=BLACK;
            test.getRegistry().displayable_positions.at(a.first).what= (char) 178;
        }

    }
    EndDrawing();
    //endwin();
};

/**
 * @brief Otwiera menu konfiguracyjne "Gry w Życie"
 * @param font czcionka do wyświetlania tekstu
 * @param filename ścieżka do pliku wykonawczego (pierwszy argument maina)
 */
void openLifeControl(Font font, std::string filename)
{
    EndDrawing(); // Render buffered menu frame
    // GUI variables
    bool dy_editMode = false;
    int dy = 25;
    bool dx_editMode = false;
    int dx = 25;
    bool scaledown;
    float hr,vr;
    while(!WindowShouldClose())
    {
        hr=GetScreenWidth()/1000.f;
        vr=GetScreenHeight()/1000.f;
        BeginDrawing();
        (GetScreenWidth()/1000.f<1)?scaledown = true:scaledown = false;
        ClearBackground(BLACK);
        // Draw text
        drawTextEx(font,TextFormat("\"Gra w zycie\" Conway'a"),500,64,32,0,RED,1,1,0);
        drawTextEx(font,TextFormat("Wymiary planszy:"),0,128,48,0,RAYWHITE,1,0,1);
        // Handle buttons
        if (GuiSpinner((Rectangle){ 500.0f*hr, 128*vr, 256*std::fmin(vr,hr), 64*std::fmin(vr,hr)} , "X ", &dx, 5, 500, dx_editMode)) dx_editMode = !dx_editMode;
        if (GuiSpinner((Rectangle){ scaledown?780.f*hr:(500.0f*hr+200.f), 128*vr,fmin(256*std::fmin(hr,vr),GetScreenWidth()-(scaledown?780.f*hr:(500.0f*hr+200.f))), 64*std::fmin(hr,vr)}, "Y ", &dy, 5, 500, dy_editMode)) dy_editMode = !dy_editMode;

        if(GuiButton((Rectangle){scaledown?780.f*hr:(500.0f*hr+200.f), (128+64*5)*vr, fmin(256*std::fmin(hr,vr),GetScreenWidth()-(scaledown?780.f*hr:(500.0f*hr+200.f))), 64*std::fmin(vr,hr)}, GuiIconText(ICON_PLAYER_PLAY,"Start"))) {
            startLife(filename,ecs::lifevars(true,dx,dy,{}));
        }
        if(GuiButton((Rectangle){500.0f*hr, (128+64*5)*vr, fmin(256*std::fmin(hr,vr),GetScreenWidth()-(scaledown?780.f*hr:(500.0f*hr+200.f))), 64*std::fmin(vr,hr)}, GuiIconText(ICON_FILE_OPEN,"Laduj"))) {
            char *file = openFileDialog(filename);
            if(file==NULL) tinyfd_messageBox("Błąd: Nie można wczytać stanu","Błąd odczytu pliku!","ok","error",1);
            else {std::cout<<file; startLife(filename,ecs::life_system::loadState(file));}
        }
        if(GuiButton((Rectangle){ 0*hr, (128+64*5)*vr, 256*std::fmin(vr,hr), 64*std::fmin(vr,hr)}, GuiIconText(ICON_UNDO,"Powrót"))) {

            break;

        };

        EndDrawing();
    }
}
/**
 * @brief wyświetla test szumu Perlina
 */
void perlinTest() {
    perlin::PerlinGenerator<double> pg;
    std::vector<double> Permutation = perlin::generateRandomAngles<double>();
    float forestf = 50;
    float preval = -forestf;
    std::vector<std::vector<double>> f(1000);
    double fp;
    display::Screen screen(1000, 1000);
    for (int i = 0; i < 1000; i++) {
        for (int j = 0; j < 1000; j++) {
            f[i].push_back(pg.getMultiOctaveNoise2D(i, j, 4, Permutation));
            }
        }
    RenderTexture2D renderTexture = LoadRenderTexture(2000, 2000);
    while(!WindowShouldClose())
    {
        BeginTextureMode(renderTexture);
        ClearBackground(BLACK);
        screen.renderFrame(renderTexture);
        DrawRectangle(0,0,50,50,RED);
        DrawRectangle(950,950,50,50,RED);
        EndTextureMode();
        BeginDrawing();

        ClearBackground(BLACK);
        DrawTexturePro(renderTexture.texture, Rectangle{0,0,renderTexture.texture.width*1.f,-1.f*renderTexture.texture.height}, Rectangle {0,0,1.f*GetScreenWidth(),1.f*GetScreenHeight()}, (Vector2){0,0}, 0.0f, WHITE);
        GuiSlider((Rectangle){ 0, 0, 256, 64},
                  "",TextFormat("%i%s",(int)forestf,"%"), &forestf, 0, 100);
        if(forestf!=preval) {
            for (int i = 0; i < 1000; i++) {
                for (int j = 0; j < 1000; j++) {
                    fp=f[i][j];
                    if(f[i][j]>(1-(forestf/100.f))) screen.writeToBuffer(Color (0,fp*228,fp*48,255), i, j);
                    else {

                        screen.writeToBuffer(Color (fp*255,fp*255,fp*255,255), i, j);
                    }
                }
            }
            preval=forestf;
        }
        DrawFPS(16, 16);
        EndDrawing();
    }
}
/**
 @brief Inicjalizuje wyświetlanie, ładuje czcionkę, otwiera okno i obsługuje menu główne
 */
int main(int argc, char* argv[]) {
    // Initialise window 
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(500,500,"Projekt PK3");
    SetWindowMinSize(300,300);
    SetTargetFPS(60);
    // Load graphics to VRAM
    GuiLoadStyleLightwawa();
    Font font = LoadFont("../font.ttf");
    // Initialise variables
    Vector2 pos(0,0);
    Color c1,c2,c3;
    c1=c2=c3=RAYWHITE;
    while(!WindowShouldClose())
    {
      BeginDrawing();
      ClearBackground(BLACK);
      // Draw FPS and GUI
      DrawFPS(0,1);
      drawTextEx(font,TextFormat("Projekt PK3"),500,250,128,0,RED,1,1,1);
      drawTextEx(font,TextFormat("Symulacja Ognia"),500,500,64,0,c1,1,1,1 );
      drawTextEx(font,TextFormat("Gra w Zycie Conway'a"),500,574,64,0,c2,1,1,1);
      drawTextEx(font,TextFormat("Wyjdz"),500,638,64,0,c3,1,1,1);
      // Display translated mouse coords/copyright
      pos = GetMousePosition();
      pos.x = pos.x / (GetScreenWidth() / 1000.0);
      pos.y = pos.y / (GetScreenHeight() / 1000.0);
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {

            drawTextEx(font, TextFormat("Click X: %f Click Y: %f", pos.x, pos.y), 500, 1000 - 32,
                                             32, 0, GRAY,1,1,1);
        }
        else drawTextEx(font, TextFormat("Mateusz Kowalec 2024"), 500, 1000 - 32,
                       32, 0, GRAY,1,1,1);
        // Handle input
        //
        if(pos.y>500-32 and pos.y <532)
        {
            c1=RED;
            c2=RAYWHITE;
            c3=RAYWHITE;
            if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) openFireConfigMenu(font);
        }
        else if(pos.y>532 and pos.y <638-32)
        {
            c1=RAYWHITE;c2=RED;c3=RAYWHITE;
            if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) openLifeControl(font,argv[0]);
        }
        else if(pos.y>638-32 and pos.y <638+64)
        {
            c1=RAYWHITE;c2=RAYWHITE;c3=RED;
            if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) return 0;
        }
        else
        {
            c1=RAYWHITE;c2=RAYWHITE;c3=RAYWHITE;

        }
        if(IsKeyReleased(KEY_Q)) {perlinTest();}

        EndDrawing();
    }
    // Close window, unload OpenGL context
    CloseWindow();
    return 0;
}
