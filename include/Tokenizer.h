#ifndef TOKENIZER_H
#define TOKENIZER_H

#include "Token.h"
#include "DynamicArray.h"
#include "MyString.h"

class Tokenizer {
public:
    // Turns a raw expression string like "y = 2*sin(x) + a" into tokens.
    static DynamicArray<Token> tokenize(const char* input);
};

#endif
