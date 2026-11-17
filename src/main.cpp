#include <iostream>
#include <cstring>
#include <cstdlib>
#include "MyString.h"
#include "DynamicArray.h"
#include "Parser.h"
#include "Equation.h"
#include "EvalContext.h"
#include "Grid2D.h"
#include "Slider.h"
#ifdef USE_SFML
#include "GraphWindow.h"
#endif

static const char PLOT_GLYPHS[] = { '*', '#', '@', '%', '+', 'o' };
static const int NUM_GLYPHS = 6;

// Splits a line on whitespace into words. No std::string/std::vector used.
static DynamicArray<MyString> splitWords(const char* line) {
    DynamicArray<MyString> words;
    int i = 0;
    while (line[i] != '\0') {
        while (line[i] == ' ' || line[i] == '\t') i++;
        if (line[i] == '\0') break;
        int start = i;
        while (line[i] != '\0' && line[i] != ' ' && line[i] != '\t') i++;
        int count = i - start;
        char buf[128];
        for (int k = 0; k < count && k < 127; k++) buf[k] = line[start + k];
        buf[count < 127 ? count : 127] = '\0';
        words.pushBack(MyString(buf));
    }
    return words;
}

// Returns everything in `line` after the first whitespace-delimited word.
static MyString restAfterFirstWord(const char* line) {
    int i = 0;
    while (line[i] == ' ' || line[i] == '\t') i++;
    while (line[i] != '\0' && line[i] != ' ' && line[i] != '\t') i++;
    while (line[i] == ' ' || line[i] == '\t') i++;
    return MyString(line + i);
}

static void printHelp() {
    std::cout << "\nCommands:\n"
              << "  add <expr>            e.g. add y = sin(x)\n"
              << "                         e.g. add x^2 + y^2 = 4\n"
              << "                         e.g. add y < x^2\n"
              << "  slider <name> <min> <max> <step> [start]   e.g. slider a -5 5 0.1\n"
              << "  set <name> <value>     e.g. set a 2.5\n"
              << "  view <xmin> <xmax> <ymin> <ymax>\n"
              << "  list                   show equations and sliders\n"
              << "  remove <index>         remove an equation by its list index\n"
              << "  render                 redraw the ASCII graph in this terminal\n"
#ifdef USE_SFML
              << "  window                 open a real graphics window (drag=pan, scroll=zoom)\n"
#endif
              << "  help                   show this message\n"
              << "  quit                   exit\n\n";
}

int main() {
    DynamicArray<Equation> equations;
    DynamicArray<Slider> sliders;
    EvalContext ctx;

    double xmin = -10, xmax = 10, ymin = -10, ymax = 10;
    int cols = 81, rows = 41;

    std::cout << "===================================================\n";
    std::cout << " Terminal Graphing Calculator (Desmos-style, 2D)\n";
    std::cout << "===================================================\n";
    printHelp();

    char line[256];
    for (;;) {
        std::cout << "> ";
        if (!std::cin.getline(line, sizeof(line))) break;
        if (strlen(line) == 0) continue;

        DynamicArray<MyString> words = splitWords(line);
        if (words.isEmpty()) continue;
        MyString cmd = words[0];

        if (cmd == MyString("quit") || cmd == MyString("exit")) {
            break;
        } else if (cmd == MyString("help")) {
            printHelp();
        } else if (cmd == MyString("add")) {
            MyString exprText = restAfterFirstWord(line);
            if (exprText.isEmpty()) {
                std::cout << "Usage: add <expression>\n";
                continue;
            }
            Parser parser;
            ExprNode* tree = parser.parse(exprText.c_str());
            if (parser.hadError) {
                std::cout << "Parse error: " << parser.errorMessage.c_str() << "\n";
                delete tree;
                continue;
            }
            char glyph = PLOT_GLYPHS[equations.size() % NUM_GLYPHS];
            Equation eq(exprText, tree, glyph);
            equations.pushBack(eq);
            std::cout << "Added [" << (equations.size() - 1) << "] '" << glyph
                      << "' " << exprText.c_str() << "\n";
        } else if (cmd == MyString("slider")) {
            if (words.size() < 5) {
                std::cout << "Usage: slider <name> <min> <max> <step> [start]\n";
                continue;
            }
            Slider s;
            s.name = words[1];
            s.minVal = words[2].toDouble();
            s.maxVal = words[3].toDouble();
            s.step = words[4].toDouble();
            s.value = (words.size() >= 6) ? words[5].toDouble() : (s.minVal + s.maxVal) / 2.0;
            sliders.pushBack(s);
            ctx.set(s.name, s.value);
            std::cout << "Slider '" << s.name.c_str() << "' = " << s.value << "\n";
        } else if (cmd == MyString("set")) {
            if (words.size() < 3) {
                std::cout << "Usage: set <name> <value>\n";
                continue;
            }
            MyString name = words[1];
            double val = words[2].toDouble();
            bool found = false;
            for (int i = 0; i < sliders.size(); i++) {
                if (sliders[i].name == name) { sliders[i].value = val; found = true; break; }
            }
            ctx.set(name, val);
            if (!found) std::cout << "(note: '" << name.c_str() << "' isn't a declared slider, setting it anyway)\n";
            std::cout << name.c_str() << " = " << val << "\n";
        } else if (cmd == MyString("view")) {
            if (words.size() < 5) {
                std::cout << "Usage: view <xmin> <xmax> <ymin> <ymax>\n";
                continue;
            }
            xmin = words[1].toDouble();
            xmax = words[2].toDouble();
            ymin = words[3].toDouble();
            ymax = words[4].toDouble();
            std::cout << "View set to x:[" << xmin << "," << xmax << "] y:[" << ymin << "," << ymax << "]\n";
        } else if (cmd == MyString("list")) {
            std::cout << "Equations:\n";
            for (int i = 0; i < equations.size(); i++) {
                std::cout << "  [" << i << "] '" << equations[i].plotChar << "' "
                          << equations[i].text.c_str() << "\n";
            }
            std::cout << "Sliders:\n";
            for (int i = 0; i < sliders.size(); i++) {
                std::cout << "  " << sliders[i].name.c_str() << " = " << sliders[i].value
                          << "  [" << sliders[i].minVal << ", " << sliders[i].maxVal << "] step " << sliders[i].step << "\n";
            }
        } else if (cmd == MyString("remove")) {
            if (words.size() < 2) {
                std::cout << "Usage: remove <index>\n";
                continue;
            }
            int idx = (int)words[1].toDouble();
            if (idx < 0 || idx >= equations.size()) {
                std::cout << "Invalid index\n";
                continue;
            }
            equations[idx].freeTree();
            equations.removeAt(idx);
            std::cout << "Removed [" << idx << "]\n";
        } else if (cmd == MyString("render") || cmd == MyString("draw")) {
            Grid2D grid(cols, rows, xmin, xmax, ymin, ymax);
            grid.plotAll(equations, ctx);
            grid.render();
#ifdef USE_SFML
        } else if (cmd == MyString("window")) {
            GraphWindow win(900, 700, xmin, xmax, ymin, ymax);
            win.run(equations, sliders, ctx, xmin, xmax, ymin, ymax);
            std::cout << "Window closed. View is now x:[" << xmin << "," << xmax
                      << "] y:[" << ymin << "," << ymax << "]\n";
#endif
        } else {
            std::cout << "Unknown command '" << cmd.c_str() << "'. Type 'help' for options.\n";
        }
    }

    for (int i = 0; i < equations.size(); i++) equations[i].freeTree();

    std::cout << "Goodbye!\n";
    return 0;
}
