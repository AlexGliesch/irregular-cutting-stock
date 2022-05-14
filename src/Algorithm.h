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
#include "Problem.h"
#include <algorithm>
#include <limits>
#include <string>
#include <vector>

struct Algorithm {

  void constructiveGreedy();

  void alphaGreedy();

  void iteratedGreedy();

  void randomPlacementAlgorithm();

  void deconstruct();

  void run();

  void writeOutput();

  struct PiecePlacement {
    PiecePlacement() {}
    PiecePlacement(Polygon&& pol, int floatIndex, double value, int vertex,
                   int attached, int av)
        : pol(std::move(pol)), floatIndex(floatIndex), value(value),
          vertex(vertex), attached(attached), attachedVertex(av) {}
    PiecePlacement(PiecePlacement&&) = default;
    PiecePlacement(const PiecePlacement&) = default;
    PiecePlacement& operator=(PiecePlacement&&) = default;
    PiecePlacement& operator=(const PiecePlacement&) = default;
    Polygon pol;
    int floatIndex = -1;
    double value = std::numeric_limits<double>::max();
    int vertex = -1;         /* vertex connected to another piece */
    int attached = -1;       /* index of piece that 'pol' is attached to */
    int attachedVertex = -1; /* index of connected vertex in 'attached' */
  };

  PiecePlacement
  selectPiecePlacement(int floatIndex,
                       std::vector<PiecePlacement>* placements = nullptr);

  bool isFeasible(const Polygon& pol) const;

  double objective(Polygon& pol);

  double fixedArea = 0.0;
  Polygon chFixed;
  std::vector<Piece> floating, fixed;
  std::vector<int> pieceQuantity;
  std::vector<std::vector<int>> g;
};

extern Algorithm alg;