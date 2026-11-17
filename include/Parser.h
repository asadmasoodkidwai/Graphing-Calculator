#ifndef PARSER_H
#define PARSER_H

#include "Token.h"
#include "DynamicArray.h"
#include "ExprNode.h"
#include "MyString.h"

// Recursive-descent parser.
//
// Grammar (roughly):
//   equation   := expr ( ('=' | '<' | '>' | '<=' | '>=') expr )?
//   expr       := term (('+' | '-') term)*
//   term       := unary (('*' | '/') unary | implicitFactor)*
//   unary      := '-' unary | power
//   power      := factor ('^' unary)?          // right-associative
//   factor     := NUMBER
//               | IDENT '(' expr (',' expr)* ')'   // function call
//               | IDENT                             // variable
//               | '(' expr ')'
class Parser {
private:
    DynamicArray<Token> tokens;
    int pos;

    Token peek() const;
    Token advance();
    bool check(int type) const;
    bool match(int type);

    ExprNode* parseEquation();
    ExprNode* parseExpr();
    ExprNode* parseTerm();
    ExprNode* parseUnary();
    ExprNode* parsePower();
    ExprNode* parseFactor();

    bool startsFactor() const;

public:
    bool hadError;
    MyString errorMessage;

    Parser();

    // Parses a full line such as "y = sin(x) + a" or "x^2 + y^2 < 4".
    // Returns a tree representing (lhs - rhs) for '=' equations, or a
    // COMPARISON node for inequalities. Caller owns the returned pointer.
    ExprNode* parse(const char* input);
};

#endif
