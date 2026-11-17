#ifndef GRID2D_H
#define GRID2D_H

#include "Equation.h"
#include "DynamicArray.h"
#include "EvalContext.h"

// Renders 2D graphs as ASCII art in the terminal.
// Screen space: columns [0, cols), rows [0, rows).
// World space: [xMin, xMax] x [yMin, yMax], y increases upward.
class Grid2D {
private:
    int cols;
    int rows;
    double xMin, xMax, yMin, yMax;
    char* buffer; // rows * cols chars, row-major

    int worldToCol(double x) const;
    int worldToRow(double y) const;
    double colToWorldX(int col) const;
    double rowToWorldY(int row) const;

    void plotPixel(int row, int col, char glyph);
    void drawAxes();

    void plotExplicit(const Equation& eq, EvalContext& ctx);
    void plotImplicitOrComparison(const Equation& eq, EvalContext& ctx);

public:
    Grid2D(int columns, int rowCount, double xmin, double xmax, double ymin, double ymax);
    ~Grid2D();

    void setViewport(double xmin, double xmax, double ymin, double ymax);
    void clear();

    // Draws every equation into the internal buffer.
    void plotAll(const DynamicArray<Equation>& equations, EvalContext& ctx);

    // Prints the buffer to stdout.
    void render() const;
};

#endif
