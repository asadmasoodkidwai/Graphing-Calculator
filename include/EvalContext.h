#ifndef EVAL_CONTEXT_H
#define EVAL_CONTEXT_H

#include "MyString.h"
#include "DynamicArray.h"

class NamedValue {
public:
    MyString name;
    double value;

    NamedValue() { value = 0.0; }
    NamedValue(const MyString& n, double v) { name = n; value = v; }
};

// Holds current values for x, y, and any user-defined sliders (a, b, t...).
class EvalContext {
private:
    DynamicArray<NamedValue> values;

public:
    void set(const MyString& name, double value) {
        for (int i = 0; i < values.size(); i++) {
            if (values[i].name == name) {
                values[i].value = value;
                return;
            }
        }
        values.pushBack(NamedValue(name, value));
    }

    double get(const MyString& name, bool* found = 0) const {
        for (int i = 0; i < values.size(); i++) {
            if (values[i].name == name) {
                if (found != 0) *found = true;
                return values[i].value;
            }
        }
        if (found != 0) *found = false;
        return 0.0;
    }

    bool has(const MyString& name) const {
        bool found = false;
        get(name, &found);
        return found;
    }
};

#endif
