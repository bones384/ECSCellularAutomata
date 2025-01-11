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
    template <typename T>
    class Timer {
        std::chrono::duration<T> m_elapsed_seconds; /**< zmienna wewnętrzna przechowująca informacje o tym, ile czasu minęło od rozpoczęcia liczenia. @warning Aktualizuje się tylko po wywołaniu @ref timer::Timer::isFinished()*/
        std::chrono::time_point<std::chrono::high_resolution_clock> m_start, m_end;
    public:
        std::chrono::duration<T> duration; /**< Długość minutnika */
        /**
         * @brief Metoda zwracająca stan minutnika
         * @return true jeśli czas @ref dur minął, w przeciwnym wypadku false
         */
        bool isFinished()
        {
            m_end = std::chrono::high_resolution_clock::now();
            m_elapsed_seconds = m_end - m_start;
            if (m_elapsed_seconds < duration) return false;
            return true;
        };
        /**
         * @brief Metoda resetująca minutnik
         */
        void reset()
        {
            m_start = std::chrono::high_resolution_clock::now();
        };
        /**
         * @brief Metoda zwracająca ile czasu minęło od rozpoczęcia liczenia
         * @return minięty czas od @ref m_start
         */
        std::chrono::duration<T> getElapsed()
        {
            return m_elapsed_seconds;
        }
        /**
         * @brief Konstruktor tworzący minutnik o długości @ref dur
         * @param dur długość minutnika
         */
        Timer(std::chrono::duration<T> dur ) : duration(dur)
        {    m_start = std::chrono::high_resolution_clock::now();};
    };
}
