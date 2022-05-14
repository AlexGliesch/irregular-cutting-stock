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
#include "Polygon.h"
#include <algorithm>
#include <cmath>

using namespace std;

Point Polygon::centroid() const {
  auto& c = const_cast<Point&>(centroid_);
  if (c == Point::infinity() and v.size() > 0) {
    c = Point(0, 0);
    for (const auto& p : v)
      c += p;
    c /= (double)v.size();
  }
  return c;
}

double Polygon::area() const {
  auto& a = const_cast<double&>(area_);
  if (a == -1.0 and v.size() >= 3) {
    /* O(n); assumes polygon is not self-intersecting */
    a = 0;
    for (int i = 0; i < (int)v.size(); ++i) {
      int j = ((i + 1) % v.size());
      a += v[i].x * v[j].y - v[j].x * v[i].y;
    }
    a = abs(a) / 2.0;
  }
  return a;
}

bool Polygon::pointInside(const Point& p, bool considerBoundary) const {
  /* If consider_boundary is true, then it will mark as 'inside' a point that
   * is on the boundary of the polygon; otherwise, it wont. */
  int i, j = v.size() - 1;
  bool oddNodes = false;
  for (i = 0; i < (int)v.size(); i++) {
    auto& pi = v[i];
    auto& pj = v[j];
    if (pi == p) {
      return considerBoundary;
    }
    if (((pi.y < p.y and pj.y >= p.y) or (pj.y < p.y and pi.y >= p.y)) and
        (pi.x <= p.x or pj.x <= p.x)) {
      oddNodes ^= (pi.x + (p.y - pi.y) / (pj.y - pi.y) * (pj.x - pi.x) < p.x);
    }
    j = i;
  }
  return oddNodes;
}

Polygon Polygon::translated(const Point& p) const {
  Polygon cp(*this);
  for (auto& i : cp.v)
    i = i.translated(p);
  if (cp.centroid_ != Point::infinity()) {
    cp.centroid_ = cp.centroid_.translated(p);
  }
  cp.reset_bb();
  return cp;
}

Polygon Polygon::rotated(double theta) const {
  Polygon cp(*this);
  for (auto& i : cp.v)
    i = i.rotated(theta);
  if (cp.centroid_ != Point::infinity()) {
    cp.centroid_ = cp.centroid_.rotated(theta);
  }
  cp.reset_bb();
  return cp;
}

Polygon convexHull(std::vector<Point> P) {

  int n = P.size(), k = 0;
  vector<Point> H(2 * n);

  // Sort points lexicographically
  sort(P.begin(), P.end());

  // Build lower hull
  for (int i = 0; i < n; ++i) {
    while (k >= 2 && cross(H[k - 2], H[k - 1], P[i]) <= 0)
      k--;
    H[k++] = P[i];
  }

  // Build upper hull
  for (int i = n - 2, t = k + 1; i >= 0; i--) {
    while (k >= t && cross(H[k - 2], H[k - 1], P[i]) <= 0)
      k--;
    H[k++] = P[i];
  }

  H.resize(k - 1);
  if (H[0] == H[H.size() - 1]) H.pop_back();
  // 	reverse(H.begin(), H.end());
  return Polygon(move(H));
}

bool boundingBoxesIntersect(const Polygon& a, const Polygon& b) {
  auto& abb = const_cast<Polygon&>(a).get_bb();
  auto& bbb = const_cast<Polygon&>(b).get_bb();
  if (abb.right < bbb.left) return false; // a is left of b
  if (abb.left > bbb.right) return false; // a is right of b
  if (abb.top < bbb.bottom) return false; // a is above b
  if (abb.bottom > bbb.top) return false; // a is below b
  return true;                            // boxes overlap
}

bool pointInRectangle(const Point& p, double left, double right, double top,
                      double bottom) {
  return p.x >= left and p.x <= right and p.y <= top and p.y >= bottom;
}