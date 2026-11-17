#include "ExprEval.h"
#include <cmath>

static bool namesEqual(const MyString& a, const char* b) {
    return a == MyString(b);
}

double ExprEval::evaluate(const ExprNode* node, const EvalContext& ctx, bool* ok) {
    if (node == 0) { *ok = false; return 0.0; }

    switch (node->type) {
        case NodeType::NUMBER:
            return node->numberValue;

        case NodeType::VARIABLE: {
            bool found = false;
            double v = ctx.get(node->variableName, &found);
            if (found) return v;
            if (namesEqual(node->variableName, "pi")) return 3.14159265358979323846;
            if (namesEqual(node->variableName, "e")) return 2.71828182845904523536;
            *ok = false;
            return 0.0;
        }

        case NodeType::UNARY_OP: {
            double v = evaluate(node->left, ctx, ok);
            if (!*ok) return 0.0;
            if (node->op == '-') return -v;
            return v;
        }

        case NodeType::BINARY_OP: {
            double l = evaluate(node->left, ctx, ok);
            if (!*ok) return 0.0;
            double r = evaluate(node->right, ctx, ok);
            if (!*ok) return 0.0;

            switch (node->op) {
                case '+': return l + r;
                case '-': return l - r;
                case '*': return l * r;
                case '/':
                    if (r == 0.0) { *ok = false; return 0.0; }
                    return l / r;
                case '^': {
                    double result = pow(l, r);
                    if (result != result) { *ok = false; return 0.0; } // NaN check
                    return result;
                }
                default:
                    *ok = false;
                    return 0.0;
            }
        }

        case NodeType::FUNCTION_CALL: {
            double a = evaluate(node->right, ctx, ok);
            if (!*ok) return 0.0;
            const MyString& fn = node->functionName;

            if (namesEqual(fn, "sin")) return sin(a);
            if (namesEqual(fn, "cos")) return cos(a);
            if (namesEqual(fn, "tan")) return tan(a);
            if (namesEqual(fn, "asin")) {
                if (a < -1.0 || a > 1.0) { *ok = false; return 0.0; }
                return asin(a);
            }
            if (namesEqual(fn, "acos")) {
                if (a < -1.0 || a > 1.0) { *ok = false; return 0.0; }
                return acos(a);
            }
            if (namesEqual(fn, "atan")) return atan(a);
            if (namesEqual(fn, "sqrt")) {
                if (a < 0.0) { *ok = false; return 0.0; }
                return sqrt(a);
            }
            if (namesEqual(fn, "abs")) return fabs(a);
            if (namesEqual(fn, "ln")) {
                if (a <= 0.0) { *ok = false; return 0.0; }
                return log(a);
            }
            if (namesEqual(fn, "log")) {
                if (a <= 0.0) { *ok = false; return 0.0; }
                return log10(a);
            }
            if (namesEqual(fn, "exp")) return exp(a);
            if (namesEqual(fn, "floor")) return floor(a);
            if (namesEqual(fn, "ceil")) return ceil(a);

            *ok = false; // unknown function
            return 0.0;
        }

        case NodeType::COMPARISON: {
            // Comparisons evaluate to 1.0 (true) / 0.0 (false); mainly used
            // by the renderer directly rather than through this path.
            double l = evaluate(node->left, ctx, ok);
            if (!*ok) return 0.0;
            double r = evaluate(node->right, ctx, ok);
            if (!*ok) return 0.0;
            switch (node->op) {
                case '<': return (l < r) ? 1.0 : 0.0;
                case '>': return (l > r) ? 1.0 : 0.0;
                case 'l': return (l <= r) ? 1.0 : 0.0;
                case 'g': return (l >= r) ? 1.0 : 0.0;
                default: *ok = false; return 0.0;
            }
        }

        default:
            *ok = false;
            return 0.0;
    }
}
