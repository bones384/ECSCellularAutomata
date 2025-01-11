//
// Created by mkowa on 15.03.2024.
//
#pragma once
#include <chrono>
/**
 * @brief Przestrzeń nazw dla @ref timer.h
 */
namespace timer {
    /**
     * @brief Prosty miernik czasu.
     * @details Zawiera metody do sprawdzania, czy już się skończył, do ponownego rozpoczęcia liczenia,
     * przechowuje swoją długość, którą przyjmuje jako argument konstruktora.
     */
    class Timer {
        std::chrono::duration<double> m_elapsed_seconds; /**< zmienna wewnętrzna przechowująca informacje o tym, ile czasu minęło od rozpoczęcia liczenia. @warning Aktualizuje się tylko po wywołaniu @ref timer::Timer::isFinished()*/
        std::chrono::time_point<std::chrono::high_resolution_clock> m_start, m_end;
    public:
        friend Timer operator+(Timer &l,std::chrono::duration<long long int> &e);
        std::chrono::duration<double> duration; /**< Długość minutnika */
        /**
         * @brief Metoda zwracająca stan minutnika
         * @return true jeśli czas @ref dur minął, w przeciwnym wypadku false
         */
        bool isFinished();
        /**
         * @brief Metoda resetująca minutnik
         */
        void reset();
        /**
         * @brief Metoda zwracająca ile czasu minęło od rozpoczęcia liczenia
         * @return minięty czas od @ref m_start
         */
        std::chrono::duration<double> getElapsed();
        /**
         * @brief Konstruktor tworzący minutnik o długości @ref dur
         * @param dur długość minutnika
         */
        Timer(std::chrono::duration<double> dur);
    };
}
