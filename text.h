//
// Created by mkowa on 27.08.2024.
//
/**
 * @file Plik przechowujący funkcje służące do manipulacji wyświetlania tekstu przez Raylib
 */
#pragma once
/**
 * @brief Funkcja zwracająca pozycję, na której należy kazać Raylibowi tekst, by środek tego tekstu był na podanej jako argument pozycji.
 * @param len długość tekstu, który chcemy wyświetlić
 * @param x współrzędna X docelowego środka
 * @param y współrzędna Y docelowego środka
 * @param fontSize Rozmiar czcionki, której chcemy użyć
 * @return
 */
[[nodiscard]] Vector2 centerPos(int len,float x, float y, int fontSize);
/**
 * @brief Wrapper do funkcji DrawTextEx() z rayliba, ułatwiający zaawansowane użytkowanie.
 * @param relative czy koordynaty oodajemy w pikselach czy względem rozmiaru ekranu
 * @param centered czy koordynaty wskazują lewy górny róg tekstu, czy jego środek
 * @param scaled czy tekst ma dostosowywać swój rozmiar do rozmiaru okna
 */
void drawTextEx(Font font, const char *text, float x, float y, float fontSize, float spacing, Color tint,bool relative=false,bool centered=false, bool scaled=false);