#ifndef TOKEN_H
#define TOKEN_H

#include "MyString.h"

// Token type "constants" grouped in a struct instead of using `enum`,
// per the "stick to basics" constraint. Plain ints under the hood.
struct TokenType {
    static const int NUMBER    = 0;
    static const int IDENT     = 1; // variable or function name
    static const int PLUS      = 2;
    static const int MINUS     = 3;
    static const int STAR      = 4;
    static const int SLASH     = 5;
    static const int CARET     = 6; // ^
    static const int LPAREN    = 7;
    static const int RPAREN    = 8;
    static const int COMMA     = 9;
    static const int EQUALS    = 10;
    static const int LESS      = 11;
    static const int GREATER   = 12;
    static const int LESSEQ    = 13;
    static const int GREATEREQ = 14;
    static const int END       = 15;
    static const int UNKNOWN   = 16;
};

class Token {
public:
    int type;
    MyString text;   // used for IDENT
    double number;    // used for NUMBER

    Token() {
        type = TokenType::UNKNOWN;
        number = 0.0;
    }

    Token(int t) {
        type = t;
        number = 0.0;
    }
};

#endif
