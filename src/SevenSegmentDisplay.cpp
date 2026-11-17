#include "SevenSegmentDisplay.h"

// Segment order: a(top), b(top-right), c(bottom-right), d(bottom),
// e(bottom-left), f(top-left), g(middle).
static const bool DIGIT_SEGMENTS[10][7] = {
    { true,  true,  true,  true,  true,  true,  false }, // 0
    { false, true,  true,  false, false, false, false }, // 1
    { true,  true,  false, true,  true,  false, true  }, // 2
    { true,  true,  true,  true,  false, false, true  }, // 3
    { false, true,  true,  false, false, true,  true  }, // 4
    { true,  false, true,  true,  false, true,  true  }, // 5
    { true,  false, true,  true,  true,  true,  true  }, // 6
    { true,  true,  true,  false, false, false, false }, // 7
    { true,  true,  true,  true,  true,  true,  true  }, // 8
    { true,  true,  true,  true,  false, true,  true  }, // 9
};

static void addSegment(sf::VertexArray& quads, float x, float y, float w, float h, const sf::Color& color) {
    quads.append(sf::Vertex(sf::Vector2f(x, y), color));
    quads.append(sf::Vertex(sf::Vector2f(x + w, y), color));
    quads.append(sf::Vertex(sf::Vector2f(x + w, y + h), color));
    quads.append(sf::Vertex(sf::Vector2f(x, y + h), color));
}

float SevenSegmentDisplay::drawChar(sf::RenderWindow& window, char c, float x, float y,
                                     float height, const sf::Color& color) {
    float width = height * 0.55f;
    float thickness = height * 0.12f;

    if (c == '-') {
        sf::RectangleShape rect(sf::Vector2f(width - thickness, thickness));
        rect.setPosition(x + thickness / 2.0f, y + height / 2.0f - thickness / 2.0f);
        rect.setFillColor(color);
        window.draw(rect);
        return width + thickness;
    }

    if (c == '.') {
        sf::RectangleShape rect(sf::Vector2f(thickness, thickness));
        rect.setPosition(x, y + height - thickness);
        rect.setFillColor(color);
        window.draw(rect);
        return thickness * 2.0f;
    }

    if (c < '0' || c > '9') {
        return width + thickness; // unsupported char: leave blank space
    }

    const bool* segs = DIGIT_SEGMENTS[c - '0'];
    sf::VertexArray quads(sf::Quads);

    float halfH = height / 2.0f;
    // a: top horizontal
    if (segs[0]) addSegment(quads, x + thickness, y, width - 2 * thickness, thickness, color);
    // g: middle horizontal
    if (segs[6]) addSegment(quads, x + thickness, y + halfH - thickness / 2.0f, width - 2 * thickness, thickness, color);
    // d: bottom horizontal
    if (segs[3]) addSegment(quads, x + thickness, y + height - thickness, width - 2 * thickness, thickness, color);
    // f: top-left vertical
    if (segs[5]) addSegment(quads, x, y, thickness, halfH, color);
    // b: top-right vertical
    if (segs[1]) addSegment(quads, x + width - thickness, y, thickness, halfH, color);
    // e: bottom-left vertical
    if (segs[4]) addSegment(quads, x, y + halfH, thickness, halfH, color);
    // c: bottom-right vertical
    if (segs[2]) addSegment(quads, x + width - thickness, y + halfH, thickness, halfH, color);

    window.draw(quads);
    return width + thickness;
}

float SevenSegmentDisplay::drawString(sf::RenderWindow& window, const MyString& text,
                                       float x, float y, float height, const sf::Color& color) {
    float cursor = x;
    for (int i = 0; i < text.length(); i++) {
        cursor += drawChar(window, text[i], cursor, y, height, color);
    }
    return cursor - x;
}

float SevenSegmentDisplay::measure(const MyString& text, float height) {
    float width = height * 0.55f;
    float thickness = height * 0.12f;
    float total = 0.0f;
    for (int i = 0; i < text.length(); i++) {
        char c = text[i];
        if (c == '-') total += width + thickness;
        else if (c == '.') total += thickness * 2.0f;
        else total += width + thickness;
    }
    return total;
}

void SevenSegmentDisplay::drawNumber(sf::RenderWindow& window, double value, float x, float y,
                                      float height, const sf::Color& color, int decimals) {
    MyString text = MyString::fromDouble(value, decimals);
    drawString(window, text, x, y, height, color);
}
