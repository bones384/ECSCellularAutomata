//
// Created by mkowa on 02.04.2024.
//

#pragma once
#include <chrono>
#include <random>
#include <iostream>
#include <raylib.h> // by RaySan, Open Source license (zlib)
/**
 * @brief Przestrzeń nazw dla @ref perlin.h
 */
namespace perlin {
    /**
     * @brief Prosta struktura reprezentująca dwuwymiarowy wektor.
     */
    struct vector2 {
        double x, y;
        vector2(double xi, double yi) : x(xi), y(yi) {}
/**
 * @brief Metoda licząca dot product pomiędzy tym wektorem a innym
 * @param other drugi wektor
 * @return
 */
        [[nodiscard]] double dot(vector2 other) const {
            return x*other.x + y*other.y;
        }
    };
    /**
     * @brief Klasa reprezentująca pojedyńczą powierzchnię zapełnioną szumem Perlina.
     */
    class PerlinGenerator
    {
        /**
         * @brief Ziarno generatora liczb losowych przypisywane przy konstrukcji obiektu
         */
        unsigned m_seed;
        /**
         * @brief Dystrybucja floatów od -1.0 do 1.0
         */
        std::uniform_real_distribution<float> m_dist{-1.0, 1.0};
        /**
         * @brief Dystrybucja intów od 0 do 1 (losowy bool)
         */
        std::uniform_int_distribution<> m_uniformDistribution{0, 1};
        /**
         * @brief Generator liczb losowych
         */
        std::mt19937 m_generator;
        /**
         * @brief Stara metoda interpolująca. Daje ładny wynik (wariant funkcji smoothstep), lecz jest bardziej skomplikowana od jej zastępcy, @ref perlin::lerp()
         *
         *  Metoda zostaje w kodzie tylko dla porównania z zastępcą.
         */
        [[deprecated("Stara wersja funkcji, perlin::lerp() jest szybsza")]] float interpolate(float a0, float a1, float w);
        [[deprecated]] vector2 randomGradient(int ix, int iy);
        [[deprecated]] double dotGridGradient(int ix, int iy, float x, float y);
    public:
        PerlinGenerator() : m_seed(std::chrono::system_clock::now().time_since_epoch().count()) {}
      /**
       * @brief Stara funkcja licząca wartość szumu w punkcie x,y. Generuje bardzo ładne wyniki, jednak robi to bardzo powolnie. Zastąpiona przez @ref perlin::PerlinGenerator::getNoise2D która korzystając z pewnych przybliżeń i uproszczeń jest znacznie szybsza.
       * @param x
       * @param y
       * @return
       */
      [[deprecated("Zastąpiona przez perlin::PerlinGenerator::getNoise2D, która jest szybsza")]] double perlin(double x, double y);
      /**
       * @brief Metoda licząca wartość szumu Perlina w podanym punkcie
       * @param x współrzędna x'owa punktu, o który pytamy
       * @param y współrzędna y'owa punktu, o który pytamy
       * @param Permutation wektor kątów wektorów jednostkowych
       * @return wartość szumu w punkcie jako double
       */
        double getNoise2D(double x, double y, std::vector<double> Permutation);
/**
 * @brief Korzysta z @ref perlin::PerlinGenerator::getNoise2D by wygenerować wielooktawowy szum Perlina
 * @param x współrzędna x'owa punktu, o który pytamy
 * @param y współrzędna y'owa punktu, o który pytamy
 * @param numOctaves liczba oktaw którą chcemy sumować
 * @param Permutation wektor kątów wektorów jednostkowych
 * @return wartość szumu w punkcie jako double
 */
        double getMultiOctaveNoise2D(int x, int y, int numOctaves, const std::vector<double>& Permutation);

    };
    /**
     * @brief Zwraca wektor wypełniony losowymi wartościami w zakresie <0;2pi>
     * @return @ref std::vector<double> zapełniony wartościami <0;2pi>
     */
    std::vector<double> generateRandomAngles();
    template<typename T>
/**
 * @brief Funkcja do szybkiego przybliżonego liczenia cosinusa. (znacznie szybsza od standardowej)
 * @tparam T
 * @param x
 * @return
 */
    inline T fastcos(T x) noexcept;
    /**
     * @brief Zwraca wektor jednostkowy nachylony pod kątem v(w radianach) względem osi OY układu współrzędnego
     * @param v kąt z którego robimy wektor
     * @return jednostkowy perlin::vector2 o podanym nachyleniu
     */
    vector2 getVectorFromAngle(double v);

    double fade(double t);
/**
 * @brief Funkcja zwracająca interpolację liniową pomiędzy a1 i a2 o wartości czasu t
 */
    double lerp(double t, double a1, double a2);

}