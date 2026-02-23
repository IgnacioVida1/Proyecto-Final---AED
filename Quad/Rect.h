//
// Created by ignac on 9/02/2026.
//

#ifndef RECT_H
#define RECT_H

#include "Point.h"

struct Rect {

    float x, y;
    float width, height;

    Rect(float x_ = 0, float y_ = 0, float w_ = 0, float h_ = 0) : x(x_), y(y_), width(w_), height(h_) {}

    bool contains(const Point& p) const;

    bool intersects(const Rect& r) const;

    Point center() const;
};



#endif //RECT_H
