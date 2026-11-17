#include "Grid2D.h"
#include "ExprEval.h"
#include <iostream>
#include <cstring>

Grid2D::Grid2D(int columns, int rowCount, double xmin, double xmax, double ymin, double ymax) {
    cols = columns;
    rows = rowCount;
    xMin = xmin; xMax = xmax; yMin = ymin; yMax = ymax;
    buffer = new char[rows * cols];
    clear();
}

Grid2D::~Grid2D() {
    delete[] buffer;
}

void Grid2D::setViewport(double xmin, double xmax, double ymin, double ymax) {
    xMin = xmin; xMax = xmax; yMin = ymin; yMax = ymax;
}

void Grid2D::clear() {
    for (int i = 0; i < rows * cols; i++) buffer[i] = ' ';
}

int Grid2D::worldToCol(double x) const {
    double t = (x - xMin) / (xMax - xMin);
    return (int)(t * (cols - 1) + 0.5);
}

int Grid2D::worldToRow(double y) const {
    // Row 0 is the top of the screen, which corresponds to yMax.
    double t = (yMax - y) / (yMax - yMin);
    return (int)(t * (rows - 1) + 0.5);
}

double Grid2D::colToWorldX(int col) const {
    double t = (double)col / (double)(cols - 1);
    return xMin + t * (xMax - xMin);
}

double Grid2D::rowToWorldY(int row) const {
    double t = (double)row / (double)(rows - 1);
    return yMax - t * (yMax - yMin);
}

void Grid2D::plotPixel(int row, int col, char glyph) {
    if (row < 0 || row >= rows || col < 0 || col >= cols) return;
    buffer[row * cols + col] = glyph;
}

void Grid2D::drawAxes() {
    if (yMin <= 0.0 && yMax >= 0.0) {
        int row = worldToRow(0.0);
        for (int c = 0; c < cols; c++) {
            if (buffer[row * cols + c] == ' ') buffer[row * cols + c] = '-';
        }
    }
    if (xMin <= 0.0 && xMax >= 0.0) {
        int col = worldToCol(0.0);
        for (int r = 0; r < rows; r++) {
            if (buffer[r * cols + col] == ' ') buffer[r * cols + col] = '|';
        }
    }
    if (yMin <= 0.0 && yMax >= 0.0 && xMin <= 0.0 && xMax >= 0.0) {
        plotPixel(worldToRow(0.0), worldToCol(0.0), '+');
    }
}

void Grid2D::plotExplicit(const Equation& eq, EvalContext& ctx) {
    const int SUBSAMPLES = 4; // catch steep curves within a single column
    for (int col = 0; col < cols; col++) {
        for (int s = 0; s < SUBSAMPLES; s++) {
            double frac = (double)s / (double)SUBSAMPLES;
            double x = colToWorldX(col) + frac * (xMax - xMin) / (cols - 1);
            ctx.set(MyString("x"), x);
            bool ok = true;
            double y = ExprEval::evaluate(eq.tree->right, ctx, &ok);
            if (!ok) continue;
            if (y < yMin || y > yMax) continue;
            int row = worldToRow(y);
            plotPixel(row, col, eq.plotChar);
        }
    }
}

void Grid2D::plotImplicitOrComparison(const Equation& eq, EvalContext& ctx) {
    // Evaluate f (or the comparison truth value) at every pixel center once.
    double* values = new double[rows * cols];
    bool* valid = new bool[rows * cols];

    bool isComparison = (eq.tree->type == NodeType::COMPARISON);

    for (int r = 0; r < rows; r++) {
        double y = rowToWorldY(r);
        for (int c = 0; c < cols; c++) {
            double x = colToWorldX(c);
            ctx.set(MyString("x"), x);
            ctx.set(MyString("y"), y);
            bool ok = true;
            double v = ExprEval::evaluate(eq.tree, ctx, &ok);
            values[r * cols + c] = v;
            valid[r * cols + c] = ok;
        }
    }

    if (isComparison) {
        // Shade every pixel where the inequality holds.
        for (int r = 0; r < rows; r++) {
            for (int c = 0; c < cols; c++) {
                if (valid[r * cols + c] && values[r * cols + c] > 0.5) {
                    if (buffer[r * cols + c] == ' ') plotPixel(r, c, '.');
                }
            }
        }
    } else {
        // Curve = wherever f changes sign between neighboring pixels.
        for (int r = 0; r < rows; r++) {
            for (int c = 0; c < cols; c++) {
                if (!valid[r * cols + c]) continue;
                double v = values[r * cols + c];

                bool crossing = false;
                if (c + 1 < cols && valid[r * cols + c + 1]) {
                    double vRight = values[r * cols + c + 1];
                    if ((v <= 0.0 && vRight > 0.0) || (v >= 0.0 && vRight < 0.0)) crossing = true;
                }
                if (r + 1 < rows && valid[(r + 1) * cols + c]) {
                    double vDown = values[(r + 1) * cols + c];
                    if ((v <= 0.0 && vDown > 0.0) || (v >= 0.0 && vDown < 0.0)) crossing = true;
                }
                if (crossing) plotPixel(r, c, eq.plotChar);
            }
        }
    }

    delete[] values;
    delete[] valid;
}

void Grid2D::plotAll(const DynamicArray<Equation>& equations, EvalContext& ctx) {
    clear();
    drawAxes();
    for (int i = 0; i < equations.size(); i++) {
        const Equation& eq = equations[i];
        if (eq.tree == 0) continue;
        if (eq.isExplicitY) {
            plotExplicit(eq, ctx);
        } else {
            plotImplicitOrComparison(eq, ctx);
        }
    }
}

void Grid2D::render() const {
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            std::cout << buffer[r * cols + c];
        }
        std::cout << "\n";
    }
}
