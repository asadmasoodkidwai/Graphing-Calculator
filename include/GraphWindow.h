#ifndef GRAPH_WINDOW_H
#define GRAPH_WINDOW_H

#include <SFML/Graphics.hpp>
#include "Equation.h"
#include "DynamicArray.h"
#include "EvalContext.h"
#include "Slider.h"

// Real-graphics 2D grapher: opens an SFML window, supports mouse-drag pan,
// scroll-wheel zoom, and draggable slider handles. Runs its own event loop
// until the window is closed or Escape is pressed, then control returns to
// the caller (the console REPL).
class GraphWindow {
private:
    int pixelWidth;
    int pixelHeight;
    double xMin, xMax, yMin, yMax;

    bool panning;
    sf::Vector2i lastMousePixel;

    int draggingSliderIndex; // -1 if none

    sf::Vector2f worldToScreen(double x, double y) const;
    sf::Vector2f screenToWorld(int px, int py) const;

    double niceStep(double roughStep) const;

    void drawGridAndAxes(sf::RenderWindow& window);
    void drawEquation(sf::RenderWindow& window, const Equation& eq, EvalContext& ctx, const sf::Color& color);
    void drawExplicit(sf::RenderWindow& window, const Equation& eq, EvalContext& ctx, const sf::Color& color);
    void drawImplicitOrComparison(sf::RenderWindow& window, const Equation& eq, EvalContext& ctx, const sf::Color& color);

    // Slider bar layout: sliders are stacked along the bottom of the window.
    sf::FloatRect sliderTrackRect(int index) const;
    void drawSliders(sf::RenderWindow& window, const DynamicArray<Slider>& sliders);
    int hitTestSlider(const DynamicArray<Slider>& sliders, sf::Vector2i mousePixel) const;
    void updateSliderFromMouse(DynamicArray<Slider>& sliders, EvalContext& ctx, int index, int mouseX) const;

public:
    GraphWindow(int width, int height, double xmin, double xmax, double ymin, double ymax);

    // Blocks until the user closes the window. Viewport bounds (xMin..yMax)
    // are read back into xmin/xmax/ymin/ymax so the console session keeps
    // whatever pan/zoom the user left it at.
    void run(DynamicArray<Equation>& equations, DynamicArray<Slider>& sliders, EvalContext& ctx,
              double& xmin, double& xmax, double& ymin, double& ymax);
};

#endif
