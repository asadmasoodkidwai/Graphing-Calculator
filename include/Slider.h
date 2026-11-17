#ifndef SLIDER_H
#define SLIDER_H

#include "MyString.h"

class Slider {
public:
    MyString name;
    double value;
    double minVal;
    double maxVal;
    double step;

    Slider() {
        value = 0.0; minVal = -5.0; maxVal = 5.0; step = 0.1;
    }
};

#endif
