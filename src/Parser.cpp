#include "Parser.h"
#include "Tokenizer.h"

Parser::Parser() {
    pos = 0;
    hadError = false;
}

Token Parser::peek() const {
    if (pos < tokens.size()) return tokens[pos];
    return Token(TokenType::END);
}

Token Parser::advance() {
    Token t = peek();
    if (pos < tokens.size()) pos++;
    return t;
}

bool Parser::check(int type) const {
    return peek().type == type;
}

bool Parser::match(int type) {
    if (check(type)) { advance(); return true; }
    return false;
}

bool Parser::startsFactor() const {
    int t = peek().type;
    return t == TokenType::NUMBER || t == TokenType::IDENT || t == TokenType::LPAREN;
}

ExprNode* Parser::parse(const char* input) {
    tokens = Tokenizer::tokenize(input);
    pos = 0;
    hadError = false;
    errorMessage = MyString();

    ExprNode* result = parseEquation();

    if (!check(TokenType::END)) {
        hadError = true;
        errorMessage = MyString("Unexpected extra input after expression");
    }
    return result;
}

ExprNode* Parser::parseEquation() {
    ExprNode* lhs = parseExpr();

    int t = peek().type;
    if (t == TokenType::EQUALS) {
        advance();
        ExprNode* rhs = parseExpr();
        // Fold "lhs = rhs" into the implicit form (lhs - rhs), which is
        // zero exactly where the equation holds. This lets one renderer
        // handle both explicit (y = f(x)) and implicit (x^2+y^2=4) forms.
        return ExprNode::makeBinary('-', lhs, rhs);
    }
    if (t == TokenType::LESS || t == TokenType::GREATER ||
        t == TokenType::LESSEQ || t == TokenType::GREATEREQ) {
        char opChar = '<';
        if (t == TokenType::LESS) opChar = '<';
        else if (t == TokenType::GREATER) opChar = '>';
        else if (t == TokenType::LESSEQ) opChar = 'l'; // '<='
        else if (t == TokenType::GREATEREQ) opChar = 'g'; // '>='
        advance();
        ExprNode* rhs = parseExpr();
        return ExprNode::makeComparison(opChar, lhs, rhs);
    }

    return lhs;
}

ExprNode* Parser::parseExpr() {
    ExprNode* node = parseTerm();
    while (check(TokenType::PLUS) || check(TokenType::MINUS)) {
        char opChar = check(TokenType::PLUS) ? '+' : '-';
        advance();
        ExprNode* rhs = parseTerm();
        node = ExprNode::makeBinary(opChar, node, rhs);
    }
    return node;
}

ExprNode* Parser::parseTerm() {
    ExprNode* node = parseUnary();
    for (;;) {
        if (check(TokenType::STAR)) {
            advance();
            ExprNode* rhs = parseUnary();
            node = ExprNode::makeBinary('*', node, rhs);
        } else if (check(TokenType::SLASH)) {
            advance();
            ExprNode* rhs = parseUnary();
            node = ExprNode::makeBinary('/', node, rhs);
        } else if (startsFactor()) {
            // Implicit multiplication, e.g. "2x", "2(x+1)", "x(x+1)"
            ExprNode* rhs = parseUnary();
            node = ExprNode::makeBinary('*', node, rhs);
        } else {
            break;
        }
    }
    return node;
}

ExprNode* Parser::parseUnary() {
    if (check(TokenType::MINUS)) {
        advance();
        ExprNode* operand = parseUnary();
        return ExprNode::makeUnary('-', operand);
    }
    if (check(TokenType::PLUS)) {
        advance();
        return parseUnary();
    }
    return parsePower();
}

ExprNode* Parser::parsePower() {
    ExprNode* base = parseFactor();
    if (check(TokenType::CARET)) {
        advance();
        ExprNode* exponent = parseUnary(); // right-associative, allows x^-1
        return ExprNode::makeBinary('^', base, exponent);
    }
    return base;
}

ExprNode* Parser::parseFactor() {
    if (check(TokenType::NUMBER)) {
        Token t = advance();
        return ExprNode::makeNumber(t.number);
    }

    if (check(TokenType::LPAREN)) {
        advance();
        ExprNode* inner = parseExpr();
        if (!match(TokenType::RPAREN)) {
            hadError = true;
            errorMessage = MyString("Expected ')'");
        }
        return inner;
    }

    if (check(TokenType::IDENT)) {
        Token t = advance();
        if (check(TokenType::LPAREN)) {
            // Function call: name(arg). Extra args after commas are parsed
            // and discarded except the first (keeps things simple).
            advance();
            ExprNode* arg = parseExpr();
            while (check(TokenType::COMMA)) {
                advance();
                delete parseExpr(); // ignore extra args for now
            }
            if (!match(TokenType::RPAREN)) {
                hadError = true;
                errorMessage = MyString("Expected ')' after function args");
            }
            return ExprNode::makeFunctionCall(t.text, arg);
        }
        return ExprNode::makeVariable(t.text);
    }

    hadError = true;
    errorMessage = MyString("Unexpected token in expression");
    return ExprNode::makeNumber(0.0);
}
