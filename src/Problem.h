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
#include "Polygon.h"
#include "Timer.h"
#include <string>
#include <vector>

struct Piece {
  Piece() {}
  Piece(const Polygon& p) : pol(p) {}

  int id = -1;
  Polygon pol;
  std::vector<double> angles;
};

struct Problem {
  void readInstance();

  std::vector<Piece> pieces;
  std::vector<int> pieceQuantity;

  std::vector<double> anglesVector;
  double phi = 0;

  bool randomAlgorithm;
  std::string optimizeString;
  int orientation;
  Polygon plate;
  int m = 1;
  bool plateIsRectangle = false;
  double plateLeft, plateRight, plateTop, plateBottom;
  std::string inputFile, outputFile;
  int randomSeed = 0;
  int piecesSuperSample = 0, plateSuperSample = 5;
  bool optimizeCompaction = false, optimizeLength = true, optimizeWidth = false;
  bool infinitePolygons = false;
  bool useBB = true;
  bool silent = false;
  bool iteratedGreedy = false;
  bool alphaGreedy = false;
  double alpha = 0.2;
  double areaPercentToDeconstruct = 30.0;

  bool timeLimitExceeded() const {
    return timerSeconds(timer) >= timeLimitSeconds;
  }

  int timeLimitSeconds = 600;
  Timer<> timer;
};

extern Problem problem;