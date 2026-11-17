#ifndef EXPR_EVAL_H
#define EXPR_EVAL_H

#include "ExprNode.h"
#include "EvalContext.h"

class ExprEval {
public:
    // Evaluates the tree given current variable values.
    // If the expression is undefined (e.g. sqrt of negative, div by 0,
    // unknown variable) *ok is set to false and 0.0 is returned.
    static double evaluate(const ExprNode* node, const EvalContext& ctx, bool* ok);
};

#endif
