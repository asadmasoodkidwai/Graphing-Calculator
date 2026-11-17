# desmos-cpp

A Desmos-style graphing calculator written from scratch in C++, with a terminal ASCII renderer and a real-graphics window built on SFML.

Built under a deliberate constraint: **only basic C++ from intro programming and OOP courses.** No `std::string`, no `std::vector`, no STL containers, no `enum`, no smart pointers. Strings, dynamic arrays, the tokenizer, the parser and the evaluator are all hand-written.

<!-- Add a screenshot: put screenshot.png in the repo and uncomment the line below -->
<!-- ![screenshot](screenshot.png) -->

## Features

- Explicit equations: `y = sin(x)`
- Implicit equations: `x^2 + y^2 = 16`
- Inequalities with shaded regions: `y < x^2 - 2`
- Multiple variables and **sliders**: `y = a*sin(x)` with `a` adjustable live
- Implicit multiplication (`2x`, `2(x+1)`), right-associative powers, unary minus
- Functions: `sin cos tan asin acos atan sqrt abs ln log exp floor ceil`
- Constants: `pi`, `e`
- **Two renderers**
  - Terminal: ASCII graph printed in the console
  - SFML window: mouse-drag pan, scroll-wheel zoom, labeled grid, per-equation colors, draggable slider handles

## How it works

Every equation `lhs = rhs` is folded into `f(x, y) = lhs - rhs` at parse time, so one pipeline handles both explicit and implicit forms.

- If the tree is `y - g(x)` with no `y` on the right, it is plotted explicitly: one sample per pixel column, connected with line segments.
- Otherwise `f(x, y)` is sampled on a grid and the curve is drawn wherever the sign flips between neighboring samples.
- Inequalities evaluate to true or false per sample and the true region is shaded.

Pipeline: `text -> Tokenizer -> Parser (recursive descent) -> ExprNode AST -> ExprEval -> renderer`

## Project structure

```
include/
  MyString.h            custom string class
  DynamicArray.h        custom growable array template
  Token.h, Tokenizer.h  lexer (TokenType uses static const int, not enum)
  ExprNode.h, Parser.h  AST and recursive-descent parser
  ExprEval.h            AST evaluator
  EvalContext.h         variable and slider values
  Equation.h, Slider.h  graphed equation and slider data
  Grid2D.h              terminal ASCII renderer
  GraphWindow.h         SFML window, pan/zoom, sliders
  SevenSegmentDisplay.h number rendering without sf::Font
src/
  *.cpp                 implementations + main.cpp (console REPL)
```

SFML usage is limited to window, events and basic drawing primitives. Numbers are drawn by a custom seven-segment renderer, so no font file is needed.

## Build (Windows, Visual Studio 2022)

Open **x64 Native Tools Command Prompt for VS 2022** (or Developer PowerShell set to amd64). Download **SFML 2.6.1, Visual C++ 17 (2022) 64-bit** from <https://www.sfml-dev.org/download/sfml/2.6.1/> and extract it next to this folder so you have `SFML-2.6.1\` beside `desmos-cpp\`.

```bat
cl /EHsc /std:c++14 /DUSE_SFML /I include /I ..\SFML-2.6.1\include src\*.cpp /Fe:grapher.exe /link /LIBPATH:..\SFML-2.6.1\lib sfml-graphics.lib sfml-window.lib sfml-system.lib
copy ..\SFML-2.6.1\bin\sfml-graphics-2.dll .
copy ..\SFML-2.6.1\bin\sfml-window-2.dll .
copy ..\SFML-2.6.1\bin\sfml-system-2.dll .
```

This project targets the **SFML 2.x API**. SFML 3 has breaking changes and will not compile.

### Terminal-only build (no SFML needed)

```bat
cl /EHsc /I include src\MyString.cpp src\Tokenizer.cpp src\Parser.cpp src\ExprEval.cpp src\Grid2D.cpp src\main.cpp /Fe:grapher_terminal.exe
```

### Linux / macOS

```bash
g++ -std=c++11 -DUSE_SFML -Iinclude src/*.cpp -o grapher -lsfml-graphics -lsfml-window -lsfml-system
```

Terminal-only:

```bash
g++ -std=c++98 -Iinclude src/MyString.cpp src/Tokenizer.cpp src/Parser.cpp src/ExprEval.cpp src/Grid2D.cpp src/main.cpp -o grapher_terminal
```

## Usage

Run `grapher.exe` and type one command per line at the `>` prompt:

| Command | Example | What it does |
|---|---|---|
| `add <expr>` | `add x^2 + y^2 = 16` | Add an equation or inequality |
| `slider <name> <min> <max> <step> [start]` | `slider a -5 5 0.1 2` | Create a slider variable |
| `set <name> <value>` | `set a 2.5` | Change a variable |
| `view <xmin> <xmax> <ymin> <ymax>` | `view -8 8 -8 8` | Set the viewport |
| `list` | | Show equations and sliders |
| `remove <index>` | `remove 0` | Remove an equation |
| `render` | | Draw the ASCII graph in the terminal |
| `window` | | Open the SFML window (needs `USE_SFML`) |
| `help`, `quit` | | |

In the window: **drag** to pan, **scroll** to zoom, **drag a slider handle** to change its value, **Esc** to close and return to the prompt. Pan and zoom carry over to the console view.

Example session:

```
add y = sin(x)
slider a -5 5 0.1 2
add y = a*sin(x)
add x^2 + y^2 = 16
add y < x^2 - 2
window
```

## Roadmap

- [ ] In-window text box for typing equations
- [ ] Smoother implicit curves (marching squares)
- [ ] 3D surfaces `z = f(x, y)` with rotation

## Notes

- Sliders in the window are labeled by index since the custom display only draws digits; use `list` to see which index is which.
- Slider-driven redraws recompute implicit curves every frame, which is fine at normal sizes but can slow down on very large windows.
