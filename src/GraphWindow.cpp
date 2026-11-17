#include "GraphWindow.h"
#include "SevenSegmentDisplay.h"
#include "ExprEval.h"
#include <cmath>
#include <iostream>

static const sf::Color CURVE_COLORS[] = {
    sf::Color(220, 50, 47),   // red
    sf::Color(38, 139, 210),  // blue
    sf::Color(133, 153, 0),   // green
    sf::Color(211, 54, 130),  // magenta
    sf::Color(181, 137, 0),   // yellow/brown
    sf::Color(42, 161, 152)   // teal
};
static const int NUM_CURVE_COLORS = 6;

GraphWindow::GraphWindow(int width, int height, double xmin, double xmax, double ymin, double ymax) {
    pixelWidth = width;
    pixelHeight = height;
    xMin = xmin; xMax = xmax; yMin = ymin; yMax = ymax;
    panning = false;
    draggingSliderIndex = -1;
}

sf::Vector2f GraphWindow::worldToScreen(double x, double y) const {
    float px = (float)((x - xMin) / (xMax - xMin) * pixelWidth);
    float py = (float)(pixelHeight - (y - yMin) / (yMax - yMin) * pixelHeight);
    return sf::Vector2f(px, py);
}

sf::Vector2f GraphWindow::screenToWorld(int px, int py) const {
    double x = xMin + (double)px / (double)pixelWidth * (xMax - xMin);
    double y = yMin + (double)(pixelHeight - py) / (double)pixelHeight * (yMax - yMin);
    return sf::Vector2f((float)x, (float)y);
}

double GraphWindow::niceStep(double roughStep) const {
    if (roughStep <= 0.0) return 1.0;
    double exponent = floor(log10(roughStep));
    double base = pow(10.0, exponent);
    double mantissa = roughStep / base;
    double niceMantissa;
    if (mantissa < 1.5) niceMantissa = 1.0;
    else if (mantissa < 3.5) niceMantissa = 2.0;
    else if (mantissa < 7.5) niceMantissa = 5.0;
    else niceMantissa = 10.0;
    return niceMantissa * base;
}

void GraphWindow::drawGridAndAxes(sf::RenderWindow& window) {
    double step = niceStep((xMax - xMin) / 12.0);

    sf::Color gridColor(230, 230, 230);
    sf::Color axisColor(60, 60, 60);
    sf::Color labelColor(90, 90, 90);

    // Vertical grid lines + x labels
    double startX = ceil(xMin / step) * step;
    for (double x = startX; x <= xMax; x += step) {
        sf::Vector2f top = worldToScreen(x, yMax);
        sf::Vector2f bottom = worldToScreen(x, yMin);
        sf::Vertex line[] = {
            sf::Vertex(top, gridColor),
            sf::Vertex(bottom, gridColor)
        };
        window.draw(line, 2, sf::Lines);

        if (fabs(x) > step / 2.0) { // skip label right at the origin, gets crowded
            sf::Vector2f axisPos = worldToScreen(x, 0.0);
            SevenSegmentDisplay::drawNumber(window, x, axisPos.x + 2, axisPos.y + 4, 12.0f, labelColor, 1);
        }
    }

    // Horizontal grid lines + y labels
    double startY = ceil(yMin / step) * step;
    for (double y = startY; y <= yMax; y += step) {
        sf::Vector2f left = worldToScreen(xMin, y);
        sf::Vector2f right = worldToScreen(xMax, y);
        sf::Vertex line[] = {
            sf::Vertex(left, gridColor),
            sf::Vertex(right, gridColor)
        };
        window.draw(line, 2, sf::Lines);

        if (fabs(y) > step / 2.0) {
            sf::Vector2f axisPos = worldToScreen(0.0, y);
            SevenSegmentDisplay::drawNumber(window, y, axisPos.x + 4, axisPos.y - 14, 12.0f, labelColor, 1);
        }
    }

    // Axes themselves, drawn thicker on top of the grid
    if (yMin <= 0.0 && yMax >= 0.0) {
        sf::Vector2f left = worldToScreen(xMin, 0.0);
        sf::Vector2f right = worldToScreen(xMax, 0.0);
        sf::RectangleShape axis(sf::Vector2f(right.x - left.x, 2.0f));
        axis.setPosition(left.x, left.y - 1.0f);
        axis.setFillColor(axisColor);
        window.draw(axis);
    }
    if (xMin <= 0.0 && xMax >= 0.0) {
        sf::Vector2f top = worldToScreen(0.0, yMax);
        sf::Vector2f bottom = worldToScreen(0.0, yMin);
        sf::RectangleShape axis(sf::Vector2f(2.0f, bottom.y - top.y));
        axis.setPosition(top.x - 1.0f, top.y);
        axis.setFillColor(axisColor);
        window.draw(axis);
    }
}

void GraphWindow::drawExplicit(sf::RenderWindow& window, const Equation& eq, EvalContext& ctx, const sf::Color& color) {
    sf::VertexArray segments(sf::Lines);
    bool havePrev = false;
    sf::Vector2f prevPoint;

    for (int px = 0; px < pixelWidth; px++) {
        double x = xMin + (double)px / (double)(pixelWidth - 1) * (xMax - xMin);
        ctx.set(MyString("x"), x);
        bool ok = true;
        double y = ExprEval::evaluate(eq.tree->right, ctx, &ok);

        if (!ok || y < yMin - (yMax - yMin) || y > yMax + (yMax - yMin)) {
            havePrev = false;
            continue;
        }

        sf::Vector2f point = worldToScreen(x, y);
        if (havePrev) {
            segments.append(sf::Vertex(prevPoint, color));
            segments.append(sf::Vertex(point, color));
        }
        prevPoint = point;
        havePrev = true;
    }

    window.draw(segments);
}

void GraphWindow::drawImplicitOrComparison(sf::RenderWindow& window, const Equation& eq, EvalContext& ctx, const sf::Color& color) {
    // Sample on a coarser grid than pixel-for-pixel to stay fast while the
    // window is being dragged/resized, then draw small filled squares.
    const int step = 3;
    int sampleW = pixelWidth / step + 2;
    int sampleH = pixelHeight / step + 2;

    double* values = new double[sampleW * sampleH];
    bool* valid = new bool[sampleW * sampleH];

    for (int sy = 0; sy < sampleH; sy++) {
        int py = sy * step;
        for (int sx = 0; sx < sampleW; sx++) {
            int px = sx * step;
            sf::Vector2f w = screenToWorld(px, py);
            ctx.set(MyString("x"), (double)w.x);
            ctx.set(MyString("y"), (double)w.y);
            bool ok = true;
            double v = ExprEval::evaluate(eq.tree, ctx, &ok);
            values[sy * sampleW + sx] = v;
            valid[sy * sampleW + sx] = ok;
        }
    }

    bool isComparison = (eq.tree->type == NodeType::COMPARISON);

    if (isComparison) {
        sf::VertexArray quads(sf::Quads);
        sf::Color fill = color;
        fill.a = 60;
        for (int sy = 0; sy < sampleH; sy++) {
            for (int sx = 0; sx < sampleW; sx++) {
                if (valid[sy * sampleW + sx] && values[sy * sampleW + sx] > 0.5) {
                    float x = (float)(sx * step);
                    float y = (float)(sy * step);
                    quads.append(sf::Vertex(sf::Vector2f(x, y), fill));
                    quads.append(sf::Vertex(sf::Vector2f(x + step, y), fill));
                    quads.append(sf::Vertex(sf::Vector2f(x + step, y + step), fill));
                    quads.append(sf::Vertex(sf::Vector2f(x, y + step), fill));
                }
            }
        }
        window.draw(quads);
    } else {
        sf::VertexArray quads(sf::Quads);
        for (int sy = 0; sy < sampleH; sy++) {
            for (int sx = 0; sx < sampleW; sx++) {
                if (!valid[sy * sampleW + sx]) continue;
                double v = values[sy * sampleW + sx];
                bool crossing = false;
                if (sx + 1 < sampleW && valid[sy * sampleW + sx + 1]) {
                    double vr = values[sy * sampleW + sx + 1];
                    if ((v <= 0.0 && vr > 0.0) || (v >= 0.0 && vr < 0.0)) crossing = true;
                }
                if (sy + 1 < sampleH && valid[(sy + 1) * sampleW + sx]) {
                    double vd = values[(sy + 1) * sampleW + sx];
                    if ((v <= 0.0 && vd > 0.0) || (v >= 0.0 && vd < 0.0)) crossing = true;
                }
                if (crossing) {
                    float x = (float)(sx * step);
                    float y = (float)(sy * step);
                    quads.append(sf::Vertex(sf::Vector2f(x - 1, y - 1), color));
                    quads.append(sf::Vertex(sf::Vector2f(x + 2, y - 1), color));
                    quads.append(sf::Vertex(sf::Vector2f(x + 2, y + 2), color));
                    quads.append(sf::Vertex(sf::Vector2f(x - 1, y + 2), color));
                }
            }
        }
        window.draw(quads);
    }

    delete[] values;
    delete[] valid;
}

void GraphWindow::drawEquation(sf::RenderWindow& window, const Equation& eq, EvalContext& ctx, const sf::Color& color) {
    if (eq.tree == 0) return;
    if (eq.isExplicitY) drawExplicit(window, eq, ctx, color);
    else drawImplicitOrComparison(window, eq, ctx, color);
}

sf::FloatRect GraphWindow::sliderTrackRect(int index) const {
    float margin = 20.0f;
    float rowHeight = 30.0f;
    float y = pixelHeight - rowHeight * (index + 1) - 10.0f;
    return sf::FloatRect(margin + 60.0f, y + rowHeight / 2.0f - 2.0f, pixelWidth - 2 * margin - 120.0f, 4.0f);
}

void GraphWindow::drawSliders(sf::RenderWindow& window, const DynamicArray<Slider>& sliders) {
    for (int i = 0; i < sliders.size(); i++) {
        const Slider& s = sliders[i];
        sf::FloatRect track = sliderTrackRect(i);

        sf::RectangleShape trackShape(sf::Vector2f(track.width, track.height));
        trackShape.setPosition(track.left, track.top);
        trackShape.setFillColor(sf::Color(200, 200, 200));
        window.draw(trackShape);

        double t = (s.value - s.minVal) / (s.maxVal - s.minVal);
        if (t < 0.0) t = 0.0;
        if (t > 1.0) t = 1.0;
        float handleX = track.left + (float)t * track.width;
        float handleY = track.top + track.height / 2.0f;

        sf::CircleShape handle(8.0f);
        handle.setOrigin(8.0f, 8.0f);
        handle.setPosition(handleX, handleY);
        handle.setFillColor(sf::Color(38, 139, 210));
        window.draw(handle);

        // Slider name (letters aren't supported by the digit display, so
        // just show its index) and current value.
        SevenSegmentDisplay::drawChar(window, '0' + (i % 10), track.left - 45.0f, track.top - 8.0f, 18.0f, sf::Color::Black);
        SevenSegmentDisplay::drawNumber(window, s.value, track.left + track.width + 10.0f, track.top - 8.0f, 18.0f, sf::Color::Black, 2);
    }
}

int GraphWindow::hitTestSlider(const DynamicArray<Slider>& sliders, sf::Vector2i mousePixel) const {
    for (int i = 0; i < sliders.size(); i++) {
        sf::FloatRect track = sliderTrackRect(i);
        sf::FloatRect hitBox(track.left - 10.0f, track.top - 15.0f, track.width + 20.0f, track.height + 30.0f);
        if (hitBox.contains((float)mousePixel.x, (float)mousePixel.y)) return i;
    }
    return -1;
}

void GraphWindow::updateSliderFromMouse(DynamicArray<Slider>& sliders, EvalContext& ctx, int index, int mouseX) const {
    Slider& s = sliders[index];
    sf::FloatRect track = sliderTrackRect(index);
    double t = ((double)mouseX - track.left) / track.width;
    if (t < 0.0) t = 0.0;
    if (t > 1.0) t = 1.0;
    double raw = s.minVal + t * (s.maxVal - s.minVal);
    if (s.step > 0.0) raw = s.minVal + floor((raw - s.minVal) / s.step + 0.5) * s.step;
    s.value = raw;
    ctx.set(s.name, s.value);
}

void GraphWindow::run(DynamicArray<Equation>& equations, DynamicArray<Slider>& sliders, EvalContext& ctx,
                        double& xmin, double& xmax, double& ymin, double& ymax) {
    xMin = xmin; xMax = xmax; yMin = ymin; yMax = ymax;

    sf::RenderWindow window(sf::VideoMode(pixelWidth, pixelHeight), "Graph (drag to pan, scroll to zoom, Esc to close)");
    window.setFramerateLimit(60);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            } else if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Escape) window.close();
            } else if (event.type == sf::Event::MouseWheelScrolled) {
                double factor = (event.mouseWheelScroll.delta > 0) ? 0.9 : 1.1;
                sf::Vector2f worldMouse = screenToWorld(event.mouseWheelScroll.x, event.mouseWheelScroll.y);
                xMin = worldMouse.x - (worldMouse.x - xMin) * factor;
                xMax = worldMouse.x + (xMax - worldMouse.x) * factor;
                yMin = worldMouse.y - (worldMouse.y - yMin) * factor;
                yMax = worldMouse.y + (yMax - worldMouse.y) * factor;
            } else if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2i mousePixel(event.mouseButton.x, event.mouseButton.y);
                    int slider = hitTestSlider(sliders, mousePixel);
                    if (slider >= 0) {
                        draggingSliderIndex = slider;
                        updateSliderFromMouse(sliders, ctx, slider, mousePixel.x);
                    } else {
                        panning = true;
                        lastMousePixel = mousePixel;
                    }
                }
            } else if (event.type == sf::Event::MouseButtonReleased) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    panning = false;
                    draggingSliderIndex = -1;
                }
            } else if (event.type == sf::Event::MouseMoved) {
                sf::Vector2i mousePixel(event.mouseMove.x, event.mouseMove.y);
                if (draggingSliderIndex >= 0) {
                    updateSliderFromMouse(sliders, ctx, draggingSliderIndex, mousePixel.x);
                } else if (panning) {
                    int dx = mousePixel.x - lastMousePixel.x;
                    int dy = mousePixel.y - lastMousePixel.y;
                    double worldDX = (double)dx / pixelWidth * (xMax - xMin);
                    double worldDY = (double)dy / pixelHeight * (yMax - yMin);
                    xMin -= worldDX; xMax -= worldDX;
                    yMin += worldDY; yMax += worldDY;
                    lastMousePixel = mousePixel;
                }
            }
        }

        window.clear(sf::Color::White);
        drawGridAndAxes(window);
        for (int i = 0; i < equations.size(); i++) {
            drawEquation(window, equations[i], ctx, CURVE_COLORS[i % NUM_CURVE_COLORS]);
        }
        drawSliders(window, sliders);
        window.display();
    }

    xmin = xMin; xmax = xMax; ymin = yMin; ymax = yMax;
}
