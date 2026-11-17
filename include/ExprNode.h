#ifndef EXPR_NODE_H
#define EXPR_NODE_H

#include "MyString.h"

// Node type "constants" instead of enum.
struct NodeType {
    static const int NUMBER = 0;
    static const int VARIABLE = 1;
    static const int BINARY_OP = 2;
    static const int UNARY_OP = 3;
    static const int FUNCTION_CALL = 4;
    static const int COMPARISON = 5; // for <, >, <=, >= (inequalities)
};

// A node in the expression tree. Relational nodes ('=','<','>','<=','>=')
// are represented specially: for '=' we fold lhs-rhs into a BINARY_OP tree
// at parse time (see Parser), but comparisons keep the operator so we can
// shade regions instead of drawing a curve.
class ExprNode {
public:
    int type;

    double numberValue;      // NUMBER
    MyString variableName;   // VARIABLE
    char op;                 // BINARY_OP / UNARY_OP / COMPARISON: '+','-','*','/','^','<','>' etc.
    MyString functionName;   // FUNCTION_CALL: "sin", "cos", "sqrt", ...

    ExprNode* left;
    ExprNode* right; // also used as "the single argument" for FUNCTION_CALL

    ExprNode() {
        type = NodeType::NUMBER;
        numberValue = 0.0;
        op = '\0';
        left = 0;
        right = 0;
    }

    ~ExprNode() {
        delete left;
        delete right;
    }

    static ExprNode* makeNumber(double value) {
        ExprNode* n = new ExprNode();
        n->type = NodeType::NUMBER;
        n->numberValue = value;
        return n;
    }

    static ExprNode* makeVariable(const MyString& name) {
        ExprNode* n = new ExprNode();
        n->type = NodeType::VARIABLE;
        n->variableName = name;
        return n;
    }

    static ExprNode* makeBinary(char op, ExprNode* left, ExprNode* right) {
        ExprNode* n = new ExprNode();
        n->type = NodeType::BINARY_OP;
        n->op = op;
        n->left = left;
        n->right = right;
        return n;
    }

    static ExprNode* makeUnary(char op, ExprNode* operand) {
        ExprNode* n = new ExprNode();
        n->type = NodeType::UNARY_OP;
        n->op = op;
        n->left = operand;
        return n;
    }

    static ExprNode* makeFunctionCall(const MyString& name, ExprNode* arg) {
        ExprNode* n = new ExprNode();
        n->type = NodeType::FUNCTION_CALL;
        n->functionName = name;
        n->right = arg;
        return n;
    }

    static ExprNode* makeComparison(char op, ExprNode* left, ExprNode* right) {
        ExprNode* n = new ExprNode();
        n->type = NodeType::COMPARISON;
        n->op = op;
        n->left = left;
        n->right = right;
        return n;
    }

    // True if this subtree references the given variable name.
    bool containsVariable(const MyString& name) const {
        if (type == NodeType::VARIABLE) return variableName == name;
        bool found = false;
        if (left != 0) found = found || left->containsVariable(name);
        if (right != 0) found = found || right->containsVariable(name);
        return found;
    }
};

#endif
