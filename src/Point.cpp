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
#include "Point.h"
#include <algorithm>

using namespace std;

double distSquared(const Point& p, const Point& q) {
  return pow(p.x - q.x, 2) + pow(p.y - q.y, 2);
}

double dist(const Point& p, const Point& q) { return sqrt(distSquared(p, q)); }

int cross(const Point& p, const Point& q, const Point& r) {
  return (q.x - p.x) * (r.y - p.y) - (q.y - p.y) * (r.x - p.x);
}

bool collinear(const Point& p, const Point& q, const Point& r) {
  return std::abs(cross(p, q, r)) < EPS;
}

bool ccw(const Point& p, const Point& q, const Point& r) {
  return cross(p, q, r) > 0;
}

double angleThreePoints(const Point& p, const Point& q, const Point& r) {
  double ux = q.x - p.x, uy = q.y - p.y;
  double vx = r.x - p.x, vy = r.y - p.y;
  return acos((ux * vx + uy * vy) /
              sqrt((ux * ux + uy * uy) * (vx * vx + vy * vy)));
}

bool inRange(const Point& p, const Point& q, const Point& r) {
  return r.x <= max(p.x, q.x) and r.x >= min(p.x, q.x) and
         r.y <= max(p.y, q.y) and r.y >= min(p.y, q.y);
}

bool lineSegmentsIntersect(const Point& A, const Point& B, const Point& C,
                           const Point& D) {
  double DxAx = D.x - A.x;
  double DxBx = D.x - B.x;
  double CxAx = C.x - A.x;
  double CxBx = C.x - B.x;
  double BxAx = B.x - A.x;
  double DyAy = D.y - A.y;
  double DyBy = D.y - B.y;
  double CyAy = C.y - A.y;
  double ByAy = B.y - A.y;
  double CyBy = C.y - B.y;

  bool crossACD = CxAx * DyAy > CyAy * DxAx;
  bool crossBCD = CxBx * DyBy > CyBy * DxBx;
  bool crossABC = BxAx * CyAy > ByAy * CxAx;
  bool crossABD = BxAx * DyAy > ByAy * DxAx;

  return crossACD != crossBCD and crossABC != crossABD;
  // 		ccw(A, C, D) != ccw(B, C, D) and
  // 		ccw(A, B, C) != ccw(A, B, D);
}