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
#include "Statistics.h"
#include "Algorithm.h"
#include "Print.h"
#include "Problem.h"
#include <algorithm>
#include <cfloat>
#include <climits>
#include <iomanip>
#include <iostream>
#include <limits>

using namespace std;

Statistics statistics;

void Statistics::printFinalStatistics() {
  double minX = DBL_MAX, maxX = -DBL_MAX, minY = DBL_MAX, maxY = -DBL_MAX;

  for (int i = 1; i < (int)alg.fixed.size(); ++i) {
    const auto& pol = alg.fixed[i].pol;
    for (const auto& p : pol.v) {
      minX = min(p.x, minX);
      minY = min(p.y, minY);
      maxX = max(p.x, maxX);
      maxY = max(p.y, maxY);
    }
  }

  int numVertices = 0, minVertices = INT_MAX, maxVertices = -1;
  double area = 0, minArea = DBL_MAX, maxArea = -1;

  for (auto& pc : problem.pieces) {
    numVertices += pc.pol.v.size();
    minVertices = min(minVertices, (int)pc.pol.v.size());
    maxVertices = max(maxVertices, (int)pc.pol.v.size());
    area += pc.pol.area();
    minArea = min(minArea, pc.pol.area());
    maxArea = max(maxArea, pc.pol.area());
  }

  println("--statistic instance ", problem.inputFile);
  println("--statistic alpha ", problem.alpha);
  println("--statistic m ", problem.m);
  println("--statistic phi ", problem.phi);
  println("--statistic deconstruct ", problem.areaPercentToDeconstruct);
  println("--statistic optimize ", problem.optimizeString);
  println("--statistic seed ", problem.randomSeed);

  println("--statistic num-polys ", problem.pieces.size());
  println("--statistic total-vertices ", numVertices);
  println("--statistic min-vertices ", minVertices);
  println("--statistic max-vertices ", maxVertices);
  println("--statistic avg-vertices ",
          numVertices / double(problem.pieces.size()));

  println("--statistic plate-area ", problem.plate.area());
  println("--statistic min-area ", minArea);
  println("--statistic max-area ", maxArea);
  println("--statistic avg-area ", area / double(problem.pieces.size()));

  println("--statistic total-length ", maxX - minX);
  println("--statistic total-width ", maxY - minY);

  // 	println("--statistic convex hull area: ", alg.chFixed.area());
  println("--statistic final-polygons-area ", alg.fixedArea);
  println("--statistic value ", 100.0 * alg.fixedArea / problem.plate.area());
  println("--statistic v0 ", v0);
  // 	println("--statistic utilization of convex hull: ",
  // 		100.0 * alg.fixedArea / alg.chFixed.area(), " %");
  // 	println("--statistic utilization of plate: ",
  // 		100.0 * alg.fixedArea / problem.plate.area(), " %");

  println("--statistic bb-tests ", numBBTests);
  println("--statistic bb-tests-succeeded ", numBBTestsSucceeded);
  println("--statistic time ", timerSeconds(problem.timer));
  println("--statistic iterations ", iterations);

  if (problem.silent)
    printForced(-100.0 * alg.fixedArea / problem.plate.area());
}
