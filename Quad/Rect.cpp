//
// Created by ignac on 9/02/2026.
//

#include "Rect.h"

bool Rect::contains(const Point& p) const {
  return (p.x >= x && p.x <= x + width && p.y >= y && p.y <= y + height);
}

bool Rect::intersects(const Rect& r) const {
  return !(x + width < r.x || r.x + r.width < x || y + height < r.y || r.y + r.height < y);
}

Point Rect::center() const {
  return {(x + width) / 2.0f, (y + height) / 2.0f};
}
