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
#include "Algorithm.h"
#include "Polygon.h"
#include "Print.h"
#include "Random.h"
#include "Statistics.h"
#include <algorithm>
#include <cassert>
#include <cfloat>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <queue>

using namespace std;

Algorithm alg;

bool Algorithm::isFeasible(const Polygon& pol) const {
  /* Test 1. plate test: test if every vertex in pol is inside plate
   * TODO: optimize for when plate is a rectangle */
  ++statistics.numFeasabilityTests;

  for (auto& p : pol.v) {
    if (problem.plateIsRectangle and
        not pointInRectangle(p, problem.plateLeft, problem.plateRight,
                             problem.plateTop, problem.plateBottom)) {
      return false;
    } else if (not problem.plate.pointInside(p, true)) {
      return false;
    }
  }

  for (int i = 1; i < (int)fixed.size(); ++i) {
    const Polygon& polF = fixed[i].pol;

    /* Test 2. bounds test: if the bounding boxes of pol and qol do not
     * intersect, then ok. This is just an optimization */
    if (problem.useBB) {
      ++statistics.numBBTests;
      if (not boundingBoxesIntersect(pol, polF)) {
        ++statistics.numBBTestsSucceeded;
        continue;
      }
    }

    /* Test 4. polygon intersection: test for every pair of lines if they
     * intersect */
    for (int k = 0; k < (int)pol.v.size(); ++k) {
      auto &p1 = pol.v[k], &q1 = pol.v[(k + 1) % pol.v.size()];
      for (int j = 0; j < (int)polF.v.size(); ++j) {
        auto& p2 = polF.v[j];
        auto& q2 = polF.v[(j + 1) % polF.v.size()];
        if (lineSegmentsIntersect(p1, q1, p2, q2)) {
          return false;
        }
      }
    }

    /* Test 3. point inclusion: test if no points in pol are inside qol */
    for (auto& p : pol.v)
      if (polF.pointInside(p, false)) {
        return false;
      }
    for (auto& p : polF.v)
      if (pol.pointInside(p, false)) {
        return false;
      }

    /* Test 4. polygon equal */
    if (pol == polF) return false;
  }
  return true;
}

double Algorithm::objective(Polygon& pol) {
  double r;
  pol.centroid_ = Point::infinity();

  if (problem.optimizeLength && problem.optimizeWidth) {
    r = pol.centroid().x / pol.centroid().y;
  } else if (problem.optimizeWidth) {
    r = pol.centroid().y;
  } else if (problem.optimizeLength) {
    r = pol.centroid().x;
  } else {
    r = 1;
  }

  if (problem.optimizeCompaction) {
    auto pts = chFixed.v;
    for (auto& p : pol.v)
      pts.push_back(p);
    auto ch = convexHull(pts);

    // 		assert(ch.area() - (fixedArea + pol.area()) >= -EPS);

    double d = (ch.area() - (fixedArea + pol.area())) / ch.area();
    d = std::max(0.0, EPS);
    r *= d;
  }

  return r;
}

Algorithm::PiecePlacement
Algorithm::selectPiecePlacement(int floatIndex,
                                vector<PiecePlacement>* candidates) {
  const Piece& pc = floating[floatIndex];
  Polygon pol = pc.pol;
  PiecePlacement bestPlacement;
  bestPlacement.value = DBL_MAX;

  const std::vector<double>* angles =
      (problem.phi == 0 ? &pc.angles : &problem.anglesVector);

  for (const auto& angle : *angles) {
    auto polRotated = pol.rotated(angle);
    for (int vi = 0; vi < (int)polRotated.v.size(); ++vi) {
      const auto& v = polRotated.v[vi];
      for (int pci = 0; pci < (int)fixed.size(); ++pci) {
        const auto& pc = fixed[pci];
        for (int ui = 0; ui < (int)pc.pol.v.size(); ++ui) {
          const auto& u = pc.pol.v[ui];
          if (problem.timeLimitExceeded()) {
            goto leave;
          }
          auto polTranslated = polRotated.translated(u - v);
          if (isFeasible(polTranslated)) {
            double value = objective(polTranslated);
            auto candidate = PiecePlacement(move(polTranslated), floatIndex,
                                            value, vi, pci, ui);
            if (value < bestPlacement.value) {
              bestPlacement = candidate;
            }
            if (candidates != nullptr) {
              candidates->push_back(move(candidate));
            }
          }
        }
      }
    }
  }
leave:;
  return bestPlacement;
}

void Algorithm::constructiveGreedy() {
  floating = problem.pieces;
  pieceQuantity = problem.pieceQuantity;
  if (g.empty()) g.emplace_back();

  /* Sort floating pieces according to area */
  sort(begin(floating), end(floating), [&](const Piece& a, const Piece& b) {
    return a.pol.area() > b.pol.area();
  });

  vector<PiecePlacement> alphaCandidates;

  while (floating.size()) {
    if (problem.timeLimitExceeded()) break;
    int lm = std::min(problem.m, (int)floating.size());
    PiecePlacement bestPlacement;
    vector<int> indicesToRemove;

    if (problem.alphaGreedy) {
      alphaCandidates.clear();
      for (int i = 0; i < lm; ++i) {
        selectPiecePlacement(i, &alphaCandidates);
      }
      // 			cout << "number of candidates: " << alphaCandidates.size() <<
      // endl;

      double bestVal = -1, worstVal = DBL_MAX;
      for (auto& c : alphaCandidates) {
        bestVal = max(bestVal, c.value);
        worstVal = min(worstVal, c.value);
      }

      int candidateNumber = 0;
      for (auto& c : alphaCandidates) {
        if (c.value >= bestVal - problem.alpha * (bestVal - worstVal)) {
          ++candidateNumber;
          if (rng.reservoirSampling(candidateNumber)) {
            bestPlacement = move(c);
          }
        }
      }
    } else {
      for (int i = 0; i < lm; ++i) {
        assert((int)floating.size() >= i + 1);
        auto placement = selectPiecePlacement(i, nullptr);
        if (placement.value == DBL_MAX) {
          indicesToRemove.push_back(i);
        }
        if (placement.value < bestPlacement.value) {
          bestPlacement = move(placement);
        }
      }
    }

    if (bestPlacement.floatIndex == -1) {
      /* no possible placements for any of the the first lm pieces;
       * remove them */
      floating.erase(floating.begin(), floating.begin() + lm);
      continue;
    }

    assert(find(indicesToRemove.begin(), indicesToRemove.end(),
                bestPlacement.floatIndex) == indicesToRemove.end());

    --pieceQuantity[floating[bestPlacement.floatIndex].id];
    if (pieceQuantity[floating[bestPlacement.floatIndex].id] == 0) {
      floating.erase(floating.begin() + bestPlacement.floatIndex);
      for (auto& i : indicesToRemove) {
        if (i > bestPlacement.floatIndex) --i;
      }
    }

    for (int i = indicesToRemove.size() - 1; i >= 0; --i) {
      floating.erase(floating.begin() + indicesToRemove[i]);
    }

    fixed.push_back(Piece(bestPlacement.pol));
    g.emplace_back();
    g[fixed.size() - 1].push_back(bestPlacement.attached);
    g[bestPlacement.attached].push_back(fixed.size() - 1);

    if (problem.optimizeCompaction) {
      fixedArea += bestPlacement.pol.area();
      for (auto& p : bestPlacement.pol.v)
        chFixed.v.push_back(p);
      chFixed = convexHull(chFixed.v);
    }
    println(fixed.size(), " polygons fixed, ", floating.size(),
            " polygons left, ", 100.0 * fixedArea / problem.plate.area(),
            " % utilization, ", timerSeconds(problem.timer), " seconds");
  }
}

void Algorithm::alphaGreedy() {
  vector<Piece> bestFixed;
  double bestFixedArea = 0.0;
  statistics.iterations = 0;
  double bestMaxX = DBL_MAX;
  while (not problem.timeLimitExceeded()) {
    fixedArea = 0.0;
    chFixed = Polygon();
    g.clear();
    fixed.clear();
    fixed.push_back(Piece(problem.plate));
    constructiveGreedy();

    if (problem.optimizeCompaction) {
      if (fixedArea > bestFixedArea) {
        bestFixedArea = fixedArea;
        bestFixed = fixed;
      }
    } else {
      double maxX = -100000;
      for (int i = 1; i < (int)fixed.size(); ++i) {
        auto& pc = fixed[i];
        for (auto& p : pc.pol.v)
          maxX = max(maxX, p.x);
      }
      if (maxX < bestMaxX) {
        bestMaxX = maxX;
        bestFixedArea = fixedArea;
        bestFixed = fixed;
      }
      cout << "maxX: " << maxX << ", bestMaxX: " << bestFixedArea << endl;
    }

    if (statistics.iterations == 0) {
      statistics.v0 = 100 * bestFixedArea / problem.plate.area();
    }

    cout << "alpha greedy #" << ++statistics.iterations
         << ": current value: " << 100 * fixedArea / problem.plate.area()
         << " %"
         << ", best value: " << 100 * bestFixedArea / problem.plate.area()
         << " %" << endl;
  }
  fixed = bestFixed;
  fixedArea = bestFixedArea;
}

void Algorithm::iteratedGreedy() {
  vector<Piece> bestFixed;
  double bestFixedArea = 0.0;
  double bestMaxX = DBL_MAX;
  vector<vector<int>> bestG;
  statistics.iterations = 0;
  while (not problem.timeLimitExceeded()) {
    constructiveGreedy();

    if (problem.optimizeCompaction) {
      if (fixedArea > bestFixedArea) {
        bestFixedArea = fixedArea;
        bestFixed = fixed;
        bestG = g;
      }
    } else {
      double maxX = -100000;
      for (int i = 1; i < (int)fixed.size(); ++i) {
        auto& pc = fixed[i];
        for (auto& p : pc.pol.v)
          maxX = max(maxX, p.x);
      }
      if (maxX < bestMaxX) {
        bestMaxX = maxX;
        bestFixedArea = fixedArea;
        bestFixed = fixed;
        bestG = g;
      }
      cout << "maxX: " << maxX << ", bestMaxX: " << bestFixedArea << endl;
    }

    if (statistics.iterations == 0) {
      statistics.v0 = 100 * bestFixedArea / problem.plate.area();
    }

    cout << "iterated greedy #" << ++statistics.iterations
         << ": current value: " << 100 * fixedArea / problem.plate.area()
         << " %"
         << ", best value: " << 100 * bestFixedArea / problem.plate.area()
         << " %" << endl;
    fixedArea = bestFixedArea;
    fixed = bestFixed;
    g = bestG;
    if (problem.timeLimitExceeded()) break;
    deconstruct();

    // 		problem.outputFile = "deconstruction.txt"; writeOutput();

    // 		cout << "area after deconstructing: " << fixedArea << endl;
  }
  fixed = bestFixed;
  fixedArea = bestFixedArea;
}

void Algorithm::randomPlacementAlgorithm() {
  floating = problem.pieces;
  while (floating.size() and not problem.timeLimitExceeded()) {
    int pci = rng.randomInt(0, floating.size() - 1);
    int ang = rng.randomInt(0, problem.anglesVector.size() - 1);

    auto polR = floating[pci].pol.rotated(ang);
    // 		polR = polR.translated(-polR.centroid());
    for (auto& v : polR.v) {
      for (auto& pc : fixed) {
        for (auto& u : pc.pol.v) {
          auto polT = polR.translated(u - v);
          if (isFeasible(polT)) {
            fixed.push_back(Piece(polT));
            fixedArea += polT.area();
            println("adding ", pci,
                    ", value: ", 100.0 * fixedArea / problem.plate.area(),
                    " %");
            goto exitForLoop;
          }
        }
      }
    }
    floating.erase(floating.begin() + pci);
  exitForLoop:;
  }
}

void Algorithm::deconstruct() {
  double areaToDeconstruct =
      problem.areaPercentToDeconstruct * problem.plate.area();

  /* choose a randomly fixed vertex */
  int v = rng.randomInt(1, fixed.size() - 1);
  queue<int> q;
  vector<bool> visited(fixed.size(), false);
  vector<bool> removed(fixed.size(), false);
  double area = 0.0;

  visited[v] = true;
  q.push(v);
  while (area < areaToDeconstruct) {
    if (problem.timeLimitExceeded()) return;
    int p;
    if (q.empty()) {
      // 			cout << "selecting random p" << endl;
      p = rng.randomInt(1, fixed.size() - 1);
      while (removed[p]) {
        p = (p + 1) % fixed.size();
        p = max(p, 1);
      }
      visited[p] = true;
    } else {
      // 			cout << "popping p" << endl;
      p = q.front();
      q.pop();
    }
    // 		cout << "p = " << p << endl;
    assert(p > 0);
    assert(visited[p]);
    assert(not removed[p]);
    removed[p] = true;
    area += fixed[p].pol.area();
    for (int i = 0; i < (int)g[p].size(); ++i) {
      int v = g[p][i];
      if (v > 0 and not visited[v]) {
        q.push(v);
        visited[v] = true;
      }
    }
  }

  assert(removed[0] == false);
  vector<int> index(fixed.size());
  int numRemoved = 0;
  for (int i = 0; i < (int)fixed.size(); ++i)
    index[i] = i;
  for (int r = 0; r < (int)removed.size(); ++r) {
    if (removed[r]) {
      ++numRemoved;
      index[r] = -1;
      for (int j = r + 1; j < (int)fixed.size(); ++j)
        --index[j];
    }
  }
  // 	cout << "removed " << numRemoved << " polygons" << endl;
  assert(index[0] == 0);

  for (int i = fixed.size() - 1; i >= 0; --i) {
    if (index[i] < 0) {
      fixed.erase(fixed.begin() + i);
      g.erase(g.begin() + i);
    } else {
      auto& v = g[i];
      v.erase(
          remove_if(v.begin(), v.end(), [&](int a) { return index[a] < 0; }),
          v.end());
    }
  }
  assert(g.size() == fixed.size());
  fixedArea = 0;
  vector<Point> pts;
  for (int i = 1; i < (int)fixed.size(); ++i) {
    auto& pc = fixed[i];
    fixedArea += pc.pol.area();
    for (auto& p : pc.pol.v)
      pts.push_back(p);
  }
  chFixed = convexHull(pts);
}

void Algorithm::run() {
  fixed.clear();
  fixed.push_back(Piece(problem.plate));
  if (problem.randomAlgorithm) {
    println("running random algorithm");
    randomPlacementAlgorithm();
  } else if (problem.iteratedGreedy) {
    println("running iterated greedy");
    iteratedGreedy();
  } else if (problem.alphaGreedy) {
    println("running alpha greedy");
    alphaGreedy();
  } else {
    println("running constructive greedy");
    constructiveGreedy();
  }
}

void Algorithm::writeOutput() {
  ofstream f(problem.outputFile);
  assert(not f.fail());
  f << problem.plate.v.size() << endl;
  for (auto& p : problem.plate.v)
    f << p.x << " " << p.y << endl;
  f << fixed.size() - 1 << endl;
  for (int i = 1; i < (int)fixed.size(); ++i) {
    auto& pc = fixed[i];
    f << pc.pol.v.size() << endl;
    for (auto& p : pc.pol.v)
      f << p.x << " " << p.y << endl;
  }
}
