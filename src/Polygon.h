/*
 * MIT License
 *
 * Copyright (c) 2016 Alex Gliesch
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#pragma once
#include "Point.h"
#include <algorithm>
#include <cfloat>
#include <vector>

struct Polygon {
  Polygon() {}

  Polygon(const Polygon& p) = default;
  Polygon(Polygon&& p) = default;
  Polygon(std::vector<Point>&& pts) : v(move(pts)) {}

  Polygon& operator=(const Polygon& p) = default;
  Polygon& operator=(Polygon&& p) = default;

  bool operator==(const Polygon& p) const {
    if (v.size() != p.v.size()) return false;
    for (int i = 0; i < (int)v.size(); ++i)
      if (v[i] != p.v[i]) return false;
    return true;
  }

  Point centroid() const;

  double area() const;

  bool pointInside(const Point& p, bool considerBoundary) const;

  Polygon translated(const Point& p) const;

  Polygon rotated(double theta) const;

  std::vector<Point> v;

  struct BB {
    double left = DBL_MAX, right = DBL_MAX, top = DBL_MAX, bottom = DBL_MAX;
  };

  void reset_bb() {
    bb.left = DBL_MAX;
    bb.right = -DBL_MAX;
    bb.bottom = DBL_MAX;
    bb.top = -DBL_MAX;
  }

  const BB& get_bb() {
    if (bb.left == DBL_MAX) {
      reset_bb();
      for (const auto& p : v) {
        bb.left = std::min(bb.left, p.x);
        bb.right = std::max(bb.right, p.x);
        bb.bottom = std::min(bb.bottom, p.y);
        bb.top = std::max(bb.top, p.y);
      }
    }
    return bb;
  }

  BB bb;

  Point centroid_ = Point::infinity();
  double area_ = -1.0;
};

Polygon convexHull(std::vector<Point> P);

bool boundingBoxesIntersect(const Polygon& a, const Polygon& b);

bool pointInRectangle(const Point& p, double left, double right, double top,
                      double bottom);