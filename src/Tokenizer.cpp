#include "Tokenizer.h"
#include <cctype>
#include <cstdlib>

DynamicArray<Token> Tokenizer::tokenize(const char* input) {
    DynamicArray<Token> tokens;
    int i = 0;

    while (input[i] != '\0') {
        char c = input[i];

        if (c == ' ' || c == '\t' || c == '\n' || c == '\r') {
            i++;
            continue;
        }

        if (isdigit((unsigned char)c) || (c == '.' && isdigit((unsigned char)input[i + 1]))) {
            int start = i;
            bool seenDot = false;
            while (isdigit((unsigned char)input[i]) || (input[i] == '.' && !seenDot)) {
                if (input[i] == '.') seenDot = true;
                i++;
            }
            int length = i - start;
            char buffer[64];
            for (int k = 0; k < length && k < 63; k++) buffer[k] = input[start + k];
            buffer[length < 63 ? length : 63] = '\0';

            Token t(TokenType::NUMBER);
            t.number = atof(buffer);
            tokens.pushBack(t);
            continue;
        }

        if (isalpha((unsigned char)c) || c == '_') {
            int start = i;
            while (isalnum((unsigned char)input[i]) || input[i] == '_') i++;
            int length = i - start;
            char buffer[64];
            for (int k = 0; k < length && k < 63; k++) buffer[k] = input[start + k];
            buffer[length < 63 ? length : 63] = '\0';

            Token t(TokenType::IDENT);
            t.text = MyString(buffer);
            tokens.pushBack(t);
            continue;
        }

        switch (c) {
            case '+': tokens.pushBack(Token(TokenType::PLUS)); i++; break;
            case '-': tokens.pushBack(Token(TokenType::MINUS)); i++; break;
            case '*': tokens.pushBack(Token(TokenType::STAR)); i++; break;
            case '/': tokens.pushBack(Token(TokenType::SLASH)); i++; break;
            case '^': tokens.pushBack(Token(TokenType::CARET)); i++; break;
            case '(': tokens.pushBack(Token(TokenType::LPAREN)); i++; break;
            case ')': tokens.pushBack(Token(TokenType::RPAREN)); i++; break;
            case ',': tokens.pushBack(Token(TokenType::COMMA)); i++; break;
            case '<':
                i++;
                if (input[i] == '=') { tokens.pushBack(Token(TokenType::LESSEQ)); i++; }
                else tokens.pushBack(Token(TokenType::LESS));
                break;
            case '>':
                i++;
                if (input[i] == '=') { tokens.pushBack(Token(TokenType::GREATEREQ)); i++; }
                else tokens.pushBack(Token(TokenType::GREATER));
                break;
            case '=':
                tokens.pushBack(Token(TokenType::EQUALS)); i++; break;
            default:
                tokens.pushBack(Token(TokenType::UNKNOWN)); i++; break;
        }
    }

    tokens.pushBack(Token(TokenType::END));
    return tokens;
}
