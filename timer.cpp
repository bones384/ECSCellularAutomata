//
// Created by mkowa on 02.04.2024.
//
#include <iostream>
#include "timer.h"

bool timer::Timer::isFinished() {
    m_end = std::chrono::high_resolution_clock::now();
     m_elapsed_seconds = m_end - m_start;
    if (m_elapsed_seconds < duration) return false;
    return true;
}
void timer::Timer::reset() {
    m_start = std::chrono::high_resolution_clock::now();
}
timer::Timer::Timer(std::chrono::duration<double> dur) : duration(dur) {
    m_start = std::chrono::high_resolution_clock::now();
}

std::chrono::duration<double> timer::Timer::getElapsed() {
    return m_elapsed_seconds;
}

timer::Timer timer::operator+(timer::Timer &l, std::chrono::duration<long long int> &e) {
    std::cout<<l.m_start;
    l.m_start+=e;
    std::cout<<l.m_start;
    return l;
}
