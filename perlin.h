
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
    template<typename T>
    struct vector2 {
        T x, y;
        vector2(T xi, T yi) : x(xi), y(yi) {}
/**
 * @brief Metoda licząca dot product pomiędzy tym wektorem a innym
 * @param other drugi wektor
 * @return
 */
        [[nodiscard]] T dot(vector2 other) const {
            return x*other.x + y*other.y;
        }
    };
    /**
     * @brief Klasa reprezentująca pojedyńczą powierzchnię zapełnioną szumem Perlina.
     */
     template <typename T>
    class PerlinGenerator
    {
        /**
         * @brief Ziarno generatora liczb losowych przypisywane przy konstrukcji obiektu
         */
        unsigned m_seed;
        /**
         * @brief Dystrybucja floatów od -1.0 do 1.0
         */
        std::uniform_real_distribution<T> m_dist{-1.0, 1.0};
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

    public:
        PerlinGenerator() : m_seed(std::chrono::system_clock::now().time_since_epoch().count()) {}
      /**
       * @brief Metoda licząca wartość szumu Perlina w podanym punkcie
       * @param x współrzędna x'owa punktu, o który pytamy
       * @param y współrzędna y'owa punktu, o który pytamy
       * @param Permutation wektor kątów wektorów jednostkowych
       * @return wartość szumu w punkcie jako double
       */
        T getNoise2D(T x, T y, std::vector<T> Permutation);
/**
 * @brief Korzysta z @ref perlin::PerlinGenerator::getNoise2D by wygenerować wielooktawowy szum Perlina
 * @param x współrzędna x'owa punktu, o który pytamy
 * @param y współrzędna y'owa punktu, o który pytamy
 * @param numOctaves liczba oktaw którą chcemy sumować
 * @param Permutation wektor kątów wektorów jednostkowych
 * @return wartość szumu w punkcie jako double
 */
        T getMultiOctaveNoise2D(int x, int y, int numOctaves, const std::vector<T>& Permutation);

    };
    /**
     * @brief Zwraca wektor wypełniony losowymi wartościami w zakresie <0;2pi>
     * @return @ref std::vector<double> zapełniony wartościami <0;2pi>
     */
    template<typename R>
    std::vector<R> generateRandomAngles();
    template<typename T>
/**
 * @brief Funkcja do szybkiego przybliżonego liczenia cosinusa. (znacznie szybsza od standardowej)
 * @tparam T
 * @param x
 * @return
 */
    inline T fastcos(T x);
    /**
     * @brief Zwraca wektor jednostkowy nachylony pod kątem v(w radianach) względem osi OY układu współrzędnego
     * @param v kąt z którego robimy wektor
     * @return jednostkowy perlin::vector2 o podanym nachyleniu
     */
    template<typename T>
    vector2<T> getVectorFromAngle(T v);
    template<typename T>
    T fade(T t);
/**
 * @brief Funkcja zwracająca interpolację liniową pomiędzy a1 i a2 o wartości czasu t
 */
    template<typename T>
    T lerp(T t, T a1, T a2);

}