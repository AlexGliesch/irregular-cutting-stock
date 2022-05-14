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
#define EPS 1e-9
#define PI 3.14159265359
#include <cfloat>
#include <ciso646>
#include <cmath>
#include <iostream>
#include <limits>

inline bool doubleEqual(double a, double b) { return abs(a - b) < EPS; }

struct Point {
  Point() {}
  Point(double x, double y) : x(x), y(y) {}

  bool operator==(const Point& p) const {
    return doubleEqual(x, p.x) and doubleEqual(y, p.y);
  }
  bool operator!=(const Point& p) const { return not(*this == p); }
  bool operator<(const Point& p) const {
    return y < p.y or (y == p.y and x > p.x);
  }
  Point operator-(const Point& p) const { return {x - p.x, y - p.y}; }
  Point operator+(const Point& p) const { return {x + p.x, y + p.y}; }
  Point operator/(double d) const { return {x / d, y / d}; }
  Point operator*(double d) const { return {x * d, y * d}; }
  Point& operator+=(const Point& p) {
    x += p.x;
    y += p.y;
    return *this;
  }
  Point& operator/=(double d) {
    x /= d;
    y /= d;
    return *this;
  }
  Point& operator=(const Point& p) = default;

  Point operator-() const { return {-x, -y}; }

  Point rotated(double theta) const {
    return {x * cos(theta) - y * sin(theta), x * sin(theta) + y * cos(theta)};
  }
  Point translated(const Point& p) const { return *this + p; }

  static Point infinity() { return {DBL_MAX, DBL_MAX}; }
  double x = 0, y = 0;
};

inline std::ostream& operator<<(std::ostream& os, const Point& p) {
  os << "(" << p.x << ", " << p.y << ")";
  return os;
}

constexpr double degToRad(double x) { return x * 0.0174533; }
constexpr double radToDeg(double x) { return x * 57.2958; }

double distSquared(const Point& p, const Point& q);

double dist(const Point& p, const Point& q);

/* > 0 ccw, < 0 cw, =0 collinear */
int cross(const Point& p, const Point& q, const Point& r);

bool collinear(const Point& p, const Point& q, const Point& r);

bool ccw(const Point& p, const Point& q, const Point& r);

double angleThreePoints(const Point& p, const Point& q, const Point& r);

bool lineSegmentsIntersect(const Point& A, const Point& B, const Point& C,
                           const Point& D);