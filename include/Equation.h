#ifndef EQUATION_H
#define EQUATION_H

#include "MyString.h"
#include "ExprNode.h"

// One graphed equation/inequality, e.g. "y = sin(x)" or "x^2 + y^2 < 4".
class Equation {
public:
    MyString text;   // original input, for display
    ExprNode* tree;   // owned; for '=' this is (lhs - rhs), for comparisons a COMPARISON node
    char plotChar;    // ASCII glyph used to draw this equation in the terminal
    bool isExplicitY; // true if this is "y = f(x)" with no y on the rhs

    Equation() {
        tree = 0;
        plotChar = '*';
        isExplicitY = false;
    }

    // Equation owns `t` after construction.
    Equation(const MyString& sourceText, ExprNode* t, char glyph) {
        text = sourceText;
        tree = t;
        plotChar = glyph;
        isExplicitY = false;

        if (t != 0 && t->type == NodeType::BINARY_OP && t->op == '-' &&
            t->left != 0 && t->left->type == NodeType::VARIABLE &&
            t->left->variableName == MyString("y")) {
            if (t->right == 0 || !t->right->containsVariable(MyString("y"))) {
                isExplicitY = true;
            }
        }
    }

    // NOTE: no destructor deletes `tree` here on purpose -- ownership is
    // managed by whoever holds the DynamicArray<Equation>, since
    // DynamicArray's copy constructor does shallow (pointer) copies of
    // members and we don't want a double-free when the array resizes.
    // Call freeTree() explicitly when an Equation is really being removed.
    void freeTree() {
        delete tree;
        tree = 0;
    }
};

#endif
