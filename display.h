//
// Created by bones384 on 10.03.2024.
//
#include <iostream>
#include <raylib.h>
#ifdef WIN32
#elifdef __linux
#endif
#pragma once
/**
 * @brief Przestrzeń nazw dla plików z @ref display.h
 */
namespace display {
    /**
     * @brief Typ wyliczeniowy przechowujący informacje o używanym systemie operacyjnym, w celu poprawnej obsługi ścieżek plików i wyświetlania
     */
    enum os {
        Windows, /**< zdefiniowano WIN32 */
        Unix, /**< zdefiniowano __linux */
        Other /**< nie zdefiniowano żadnej z nich*/
    };
    /**
     * @brief Funkcja zwracająca kolor odpowiadający wartości gradientu 2 lub 3 kolorowego w danym punkcie
     * @param fade położenie w gradiencie, od 0 do 1.0
     * @param color1 pierwszy kolor
     * @param color2 drugi kolor
     * @param color3 opcjonalny trzeci kolor
     * @return kolor w danym miejscu w gradiencie
     */
    Color colorGradient(float fade, Color color1, Color color2, Color color3={0,0,0,0}) ;
    /**
     * @brief Funkcja inicjalizująca ekran
     */
        void initScreen();
        /**
         * @brief Klasa ekranu, przechowująca jego bufor i zawierająca metody do manipulowania tym buforem, służy do wyświetlania zawartości bufora na ekranie
         */
    class Screen {
    private:
        int m_dx, /**< @brief wymiar x buforu */
        m_dy; /**< @brief wymiar y buforu */
        bool m_isactive = true; /**< @brief stan ekranu */

    public:
        /**
         * @brief Kolor używany do wyświetlania pustych elementów buforu
         */
        Color empty;
        /**
         * @brief Wykryty system operacyjny
         */
        static os operating_system;
        /**
         * @brief Zwraca szerokość buforu ekranu
         */
        [[nodiscard]] int getdx() const;
        /**
         * @brief Zwraca wysokość buforu ekranu
         */
        [[nodiscard]] int getdy() const;
        /**
         * @brief Zwraca wykryty rodzaj systemu
         * @return display::os
         */
        [[nodiscard]] static os getos() ;
        /**
         * @brief Zwraca wartość @ref m_isactive
         * @return true jeśli ekran jest aktywny, w przeciwnym razie false
         */
        [[nodiscard]] bool isActive() const;
        /**
         * @brief Metoda ustawiająca @ref m_isactive na false
         */
        void stop();
        /**
         * @brief Metoda ustawiająca @ref m_isactive na true
         */
        void start();
        /**
         * @brief Bufor ekranu alokowany przy tworzeniu ekranu
         */
        Color **buffer;
        bool writeToBuffer(Color what, int x, int y);
        /**
         * @brief Zeruje element buforu pod podanymi współrzędnymi
         * @param x
         * @param y
         * @return 0 jeśli podano koordynaty poza buforem, w przeciwnych wypadkach 1
         */
        bool removeFromBuffer(int x, int y);
        /**
         * @brief Dodaje bufor do wyrenderowania w kolejnej klatce
         * @param sep opcjonalna odległość między kwadratami
         */
        void renderFrame(int sep=0);
        /**
         * @brief Dodaje bufor do podanej tekstury
         * @param rendertexture
         * @param sep opcjonalna odległość między kwadratami
         */
        void renderFrame(RenderTexture2D &rendertexture, int sep=0);
        /**
         * @brief Metoda zerująca bufor ekranu
         */
        [[maybe_unused]] void clearBuffer();
        /**
         * @brief Konstruktor ekranu z buforem o wymiarach x i y
         * @param x
         * @param y
         */
        Screen(int x, int y);
        /**
         * @brief Destruktor ekranu, dealokuje bufor
         */
        ~Screen();

    };
}