//
// Created by mkowa on 02.04.2024.
//

#pragma once
#include <random>
#include <chrono>
/**
 * @brief Przestrzeń nazw dla @ref coinflip.h
 */
namespace roll {
    /**
     * @brief Klasa reprezentująca generator liczb losowych seedowany w momencie konstrukcji
     */
    class RNG {
        std::mt19937 generator;
    public:
        static std::uniform_int_distribution<> percentile;
        RNG() : generator(std::chrono::system_clock::now().time_since_epoch().count()) {}
        static std::uniform_int_distribution<> uniformDistribution;
        friend bool flipCoin();
        friend bool percentileRoll(int chance);
    };
    /**
     * @brief Funkcja symulująca rzut monetą
     * @return 1 lub 0, z równym prawdopodobieństwem
     */
        bool flipCoin();
        /**
         * @brief Funkcja symulująca próbę o podanym prawdopodobieństwie sukcesu
         * @param chance szansa na sukces
         * @return 1 przy sukcesie, 0 jeśli próba się nie powiodła
         */
        bool percentileRoll(int chance);
}
