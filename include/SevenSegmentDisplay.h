#ifndef SEVEN_SEGMENT_H
#define SEVEN_SEGMENT_H

#include <SFML/Graphics.hpp>
#include "MyString.h"

// Draws digits/numbers using little rectangle "segments", like a
// calculator display. Deliberately avoids sf::Font / sf::Text so the
// project doesn't depend on locating a .ttf file on the user's system --
// keeps SFML usage to the core drawing primitives only.
class SevenSegmentDisplay {
public:
    // Draws a single character ('0'-'9', '-', '.') with its top-left
    // corner at (x, y). `height` controls overall size; width scales
    // proportionally. Returns the horizontal space (in pixels) it used,
    // so callers can advance the cursor for the next character.
    static float drawChar(sf::RenderWindow& window, char c, float x, float y,
                           float height, const sf::Color& color);

    // Draws a full string left-to-right starting at (x, y).
    static float drawString(sf::RenderWindow& window, const MyString& text,
                             float x, float y, float height, const sf::Color& color);

    // Formats `value` with the given number of decimal places and draws it.
    static void drawNumber(sf::RenderWindow& window, double value, float x, float y,
                            float height, const sf::Color& color, int decimals);

    // Width (in pixels) that drawString would use for a string of length n,
    // handy for right-aligning labels.
    static float measure(const MyString& text, float height);
};

#endif
