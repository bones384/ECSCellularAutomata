//
// Created by mkowa on 27.08.2024.
//
#pragma once

#include <unordered_set>
#include <unordered_map>
#include <queue>
#include <thread>
#include <iostream>
#include <string>
#include <cstring>
#include <charconv>
#include <fstream>
#include "raylib.h"
#include "timer.h"
#include "coinflip.h"
#include "display.h"
#include "perlin.h"
#include "tinyfiledialogs.h" // Native file dialogs by Guillaume Vareille http://ysengrin.com, Open Source license (zlib)
#include "text.h"
/**
 * @brief Przestrzeń nazw dla @ref ecs.h
 */
namespace ecs {
    /**
     * @brief Struktura przechowująca konfigurację symulacji ognia
     */
    struct firevars
    {
        int fire_spread_chance,dx,dy;
        float fire_burn_time, fire_spread_time, forest;
    };
    /**
     * @brief Struktura przechowująca konfigurację gry w życie
     */
    struct lifevars
    {
        bool ok = false;
        int dx,dy;
        std::vector<std::pair<int,int>> alives;
    };
    std::ostream& operator << (std::ostream & F, firevars const & w);
    std::ostream& operator << (std::ostream & F, lifevars const & w);
    /**
     * @brief Komponent pozycji, przechowuje dane o tym gdzie jest dana jednostka, i jak tą jednostkę wyświetlić
     */
    struct position_component {
        int posx, posy;
        char what;
        Color col;
        position_component(int x, int y, char w, Color a = Color(0, 0, 0, 0));
    };
/**
 * @brief Komponent życia (do gry w życie Conwaya), przechowuje czy jednostka jest żywa lub martwa, oraz czy jej stan ma się zmienić przy kolejnej aktualizacji
 */
    struct life_component {
        bool cellstate;
        bool should_switch = false;
        life_component(bool state);
    };
/**
 * @brief Komponent rozprzestrzeniania, przechowuje zestaw rodzajów jednostek, które może zastąpić oraz licznik czasu mówiący jak często może to robić
 */
    struct spreadable_component {
        std::unordered_set<char> spreads_to;
        timer::Timer dspread_timer;
        spreadable_component(std::unordered_set<char> spread, float interval);
    };
/**
 * @brief Komponent ognia, oprócz danych przechowywanych w typowym @ref ecs::spreadable_component przechowuje dodatkowo licznik czasu do jego usunięcia oraz szansę na rozprzestrzenienie.
 */
    struct fire_component : public spreadable_component {
        timer::Timer burnout_timer;
        int chance;

        fire_component(std::unordered_set<char> spread, float interval, float btimer, int chanced);
    };
    /**
    * @brief Przechowuje funkcję hashującą, umożliwiającą stworzenie mapy par
    */
    struct PairHash {
        template<class T1, class T2>
        unsigned long long int operator()(const std::pair<T1, T2> &v) const;
    };
/**
 * @brief Rejestr komponentów. Przechowuje dane o tym, ile jednostek trzyma w nim komponenty, oraz wszystkie pod-rejestry komponentów
 */
    struct registry {
        unsigned long long int max_entity = 0; /**< @brief Zmienna przechowująca ile maksymalnie jest jednostek w rejestrze */
        std::unordered_map<unsigned long long int, position_component> displayable_positions; /**< @brief Przechowuje komponenty ognia*/
        std::unordered_map<unsigned long long int, spreadable_component> spreadables; /**< @brief Przechowuje komponenty rozprzestrzeniające się */
        std::unordered_map<unsigned long long int, fire_component> fires; /**< @brief Przechowuje komponenty ognia */
        std::unordered_map<unsigned long long int, life_component> lives; /**< @brief Przechowuje komponenty  życia*/
        std::unordered_map<std::pair<int, int>, unsigned long long int, PairHash> entitypos; /**< @brief Przechowuje identyfikatory jednostek na danych położeniach */

    };
/**
 * @brief System obsługi jednostek mogących się rozprzestrzenianiać.
 */
    struct spreading_system {
        /**
         * @brief Jak każda funkcja systemowa update, Wywoływana co klatkę. Aktualizuje i rozprzestrzenia odpowiednie komponenty.
         * @param r_reg rejestr komponentów do zaktualizowania
         */
        virtual void update(ecs::registry &r_reg);
        /**
         * @brief Rozprzestrzenia jednostkę e zastępując jednostkę b.
         * @param r_reg rejestr, do którego należą jednostki
         * @param e
         * @param b
         */
        virtual void spread(ecs::registry &r_reg, unsigned long long int e, unsigned long long int b);
        // Obiekt nie sąsiaduje z innymi obiektami na jego polu
        /**
         * @brief Sprawdza czy dwie jednostki są sąsiadami. Zastąpiona przez @ref ecs::spreading_system::getNeighbours
         * @param r_reg rejestr, do którego należą jednostki
         * @param a pierwsza jednostka
         * @param b druga jednostka
         * @return 1 jeśli są sąsiednie, 0 jeśli nie
         */
        [[deprecated("Lepiej korzystać z ecs::spreading_system::getNeighbours, która od razu odnajdzie sąsiadów jednostki używając tablicy entitypos")]] static bool areNeighbours(ecs::registry &r_reg, unsigned long long int a, unsigned long long int b);
        /**
         * @brief Zwraca sąsiadów danej jednostki
         * @param r_reg rejestr, w której jest ta jednostka
         * @param a jednostka
         * @return std::vector identyfikatorów sąsiednich jednostek
         */
        static std::vector<unsigned long long int> getNeighbours(ecs::registry &r_reg, unsigned long long int a);
    };
    /**
    * @brief System obsługi jednostek ognia.
    */
    struct fire_system : spreading_system {
        static unsigned int counter;
        std::queue<unsigned long long int> firequeue;
        /**
         * @brief Jak każda funkcja systemowa update, Wywoływana co klatkę. Aktualizuje i rozprzestrzenia odpowiednie komponenty.
         * @param r_reg rejestr komponentów do zaktualizowania
         * @param screen ekran, na którym wyświetlany jest ogień ( w celu zmiany jego koloru zależnie od wypalenia)
         */
        void update(ecs::registry &r_reg, display::Screen &screen);
        /**
         * @brief Sprawia że ogień wygasa - kasuje jednostke, która posiada jego komponent ze wszystkich rejestrów i usuwa ją z buforu
         * @param e jednostka do wypalenia
         * @param r_reg rejestr, w którym się znajduje
         * @param screen ekran, na którym jest wyświetlona
         */
        void burnout(unsigned long long int e, ecs::registry &r_reg, display::Screen &screen);
        /**
         * @brief Rozprzestrzenia jednostkę e zastępując jednostkę b. Jednostka b otrzymuje kopię komponentu ognia jednostki e, tyle że z odnowionym czasem płonięcia
         * @param r_reg rejestr, do którego należą jednostki
         * @param e
         * @param b
         */
        void spread(ecs::registry &r_reg, unsigned long long int e, unsigned long long int b);
    };
    class Game;
    /**
     * @brief System obsługi jednostek mogących się rozprzestrzenianiać.
     */
    class display_system {
    private:
        friend class ecs::Game;
        int dx, dy; /**< @brief Wymiary symulacji */
    public:
        /**
        * @brief Jak każda funkcja systemowa update, Wywoływana co klatkę. Aktualizuje i wyświetla odpowiednie komponenty
        * @param r_reg rejestr komponentów do zaktualizowania
        * @param screen ekran, na którym wyświetlone mają zostać komponenty
        */
        void update(ecs::registry &r_reg, display::Screen &screen);
        /**
         * @brief Renderuje stan gry na podany ekran dopuki ekran ten jest aktywny
         */
        static void render(display::Screen &screen);
    };
    /**
     * @brief System obsługi jednostek z komponentem gry w życie Conwaya.
     */
    class life_system {
        bool m_is_paused=true; /**< @brief Czy gra jest zapauzowana? */
        float m_steptimerdur; /**< @brief Ile czasu pomiędzy krokami symulacji? */
        timer::Timer m_steptimer; /**< @brief Licznik czasu do kolejnego kroku */
    public:
        /**
         * @brief Zwraca czy symulacja gry życia jest zapauzowana, czy nie, jako bool
         */
        bool isPaused();
        /**
         * @brief Pauzuje symulację gry w życie
         */
        void pause();
        /**
         * @brief Odpauzowuje symulację gry w życie
         */
        void unpause();
        /**
         * @brief Zmienia długość kroku symulacji
         * @param dur nowa długość kroku
         */
        void changeDur(float dur);
        /**
         * @brief Zwraca długość kroku symulacji
         * @return długość kroku symulacji
         */
        float getDur();
        /**
         * @brief Kolejka komponentów, których stan trzeba zmienić
         */
        std::queue<unsigned long long int> switchq;
        /**
         * @brief Jak każda funkcja systemowa update, Wywoływana co klatkę. Aktualizuje i zmienia stan odpowiednich komponentów
         * @param r_reg rejestr komponentów do zaktualizowania
         * @param forced opcjonalny argument wymuszający aktualizacje nawet jeśli nie minął jeszcze odpowiedni czas
         */
        void update(ecs::registry &r_reg,bool forced=false);
        /**
         * @brief Zmienia stan życia jednostki
         * @param e jednostka do zmienienia
         * @param r_reg rejest, w którym jest jednostka
         */
        static void switchState(unsigned long long int &e, ecs::registry &r_reg);
        /**
         * @brief Zapisuje stan gry w życie do pliku
         * @param r_reg rejestr do zapisania
         * @param filename ścieżka do projektu (1-szy argument funkcji main)
         * @param dx wymiar x zapisywanego stanu
         * @param dy wymiar y zapisywanego stanu
         * @return 1 jeśli zapis się udał, w przeciwnym wypadku 0
         */
        bool saveState(ecs::registry &r_reg, std::string filename,int dx, int dy);
        /**
         * @brief Ładuje stan gry w życie z pliku
         * @param filename ścieżka do projektu (1-szy argument funkcji main)
         * @return 1 jeśli odczyt się udał, w przeciwnym wypadku 0
         */
        static ecs::lifevars loadState(std::string filename);
        /**
         * @brief Zwraca kolejkę żywych sąsiadów danej jednostki
         * @param r_reg rejestr, w którym jest jednostka
         * @param a badana jednostka
         * @return kolejka identyfikatorów żywych sąsiadów tej komórki
         */
        static std::queue<unsigned long long int> getLiveNeighbours(ecs::registry &r_reg, unsigned long long int a);
        life_system(float dur = 5);
    };
    /**
     * @brief Klasa obsługująca stan gry, "world space"/"game world". Przechowuje rejestr, aktualizuje go i zawiera zmienne stanu gry
     */
    class Game {
    private:
        bool m_is_running; /**< @brief Czy gra jest uruchomiona? */
        bool m_is_rendering; /**< @brief Czy gra jest renderowana? */
        registry m_registry; /**< @brief Rejestr gry */
        display_system m_display_system;
        spreading_system m_spreading_system;

    public:
        life_system game_life_system;
        ecs::fire_system fire_system;
        std::thread render_thread;
/**
 * @brief Metoda służąca tworzeniu nowej jednostki
 * @return zwraca kolejny wolny identyfikator jednostki, który można używać by zdefiniować jakiś nowy obiekt
 */
        unsigned long long int createEntity();
        /**
         * @brief Usuwa jednostkę ze wszystkich rejestrów.
         * @param entity jednostka do usunięcia
         */
        void removeEntity(unsigned long long int entity);
        /**
         * @brief Zwraca referencję do rejestru
         * @return referencja do rejestru
         */
        registry &getRegistry();
        /**
         * @brief  Metoda zwracająca czy gra jest uruchomiona, czy nie
         * @return 1 jeśli tak, 0 jeśli nie
         */
        bool isRunning() const;
        /**
         * @brief Wywołuje aktualizację wszystkich systemów
         * @param screen ekran, na którym systemy mają wyświetlać zmiany
         */
        void update(display::Screen &screen);
        /**
         * @brief Stara funkcja, która uruchamiała renderowanie na osobnym wątku. Niestety RayLib nie pozwala na renderowanie poza głównym wątkiem
         * @param screen ekran do renderowania
         */
         /**
          * @brief Aktywuje podany ekran i rozpoczyna wyświetlanie na nim
          * @param screen
          */
        [[deprecated]] void startDisplay(display::Screen &screen);
        /**
         * @brief Zatrzymuje wyświetlanie na danym ekranie
         * @param screen
         */
        void stopDisplay(display::Screen &screen);
        /**
         * @brief Zatrzymuje grę
         */
        void stop();
        Game(const display::Screen &Scr);
    };


}
