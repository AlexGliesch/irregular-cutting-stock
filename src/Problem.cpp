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
#include "Problem.h"
#include "Print.h"
#include <algorithm>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <cassert>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <map>

using namespace std;

Problem problem;

enum Orientation { Clockwise, CounterClockwise };

Polygon superSample(const Polygon& pol, const int numberOfSamples = 1) {
  /* Will create numberOfSamples new points between each two points in the
   * plate */
  Polygon newPol;
  for (int i = 0; i < (int)pol.v.size(); ++i) {
    newPol.v.push_back(pol.v[i]);
    int j = (i + 1) % pol.v.size();
    auto vec = (pol.v[j] - pol.v[i]) / (numberOfSamples + 1);
    for (int k = 1; k <= numberOfSamples; ++k) {
      newPol.v.push_back(pol.v[i] + vec * k);
    }
  }
  return newPol;
}

void Problem::readInstance() {
  assert(not inputFile.empty());
  if (inputFile.find(".xml") != string::npos) {
    using boost::property_tree::ptree;
    ptree pt;
    read_xml(inputFile, pt);

    string ori = pt.get<string>("nesting.verticesOrientation");
    orientation = (ori == "clockwise" ? Clockwise : CounterClockwise);

    println("reading polygons...");
    map<string, Polygon> polygons;
    for (auto& i : pt.get_child("nesting.polygons")) {
      assert(i.first == "polygon");
      string polygonName = i.second.get<string>("<xmlattr>.id", "");
      vector<Point> v;
      for (auto& j : i.second.get_child("lines")) {
        assert(j.first == "segment");
        double x = j.second.get<double>("<xmlattr>.x0");
        double y = j.second.get<double>("<xmlattr>.y0");
        v.emplace_back(x, y);
      }
      polygons.emplace(polygonName, Polygon(move(v)));
    }

    println("reading pieces...");
    pieceQuantity.clear();
    int pieceID = 0;
    for (auto& i : pt.get_child("nesting.problem.lot")) {
      assert(i.first == "piece");
      string piece_name = i.second.get<string>("<xmlattr>.id", "");
      int quantity = i.second.get<int>("<xmlattr>.quantity", 0);

      string polygonName = i.second.get_child("component")
                               .get<string>("<xmlattr>.idPolygon", "");
      Piece pc;
      pc.pol = polygons[polygonName];
      pc.id = pieceID++;

      for (auto& j : i.second.get_child("orientation")) {
        assert(j.first == "enumeration");
        double angle = j.second.get<double>("<xmlattr>.angle", 0);
        pc.angles.push_back(degToRad(angle));
      }

      pieceQuantity.push_back(quantity);
      pieces.push_back(pc);
    }

    println("super sampling...");

    plate = polygons["polygon0"];
  } else {
    cout << ".txt input file" << endl;

    ifstream f(inputFile);
    assert(not f.fail());

    int numPlateVertices;
    f >> numPlateVertices;
    plate.v.resize(numPlateVertices);
    for (int i = 0; i < numPlateVertices; ++i) {
      f >> plate.v[i].x >> plate.v[i].y;
    }
    int numPieces;
    f >> numPieces;
    pieces.clear();
    pieces.resize(numPieces);
    for (int i = 0; i < numPieces; ++i) {
      pieces[i].id = i;
      pieces[i].angles.push_back(0);
      int numPcPolVer;
      f >> numPcPolVer;
      pieces[i].pol.v.resize(numPcPolVer);
      for (int j = 0; j < numPcPolVer; ++j) {
        f >> pieces[i].pol.v[j].x >> pieces[i].pol.v[j].y;
      }
    }

    infinitePolygons = true;
    pieceQuantity.clear();
    pieceQuantity.resize(numPieces);
  }

  /* pre-process phase */

  if (plate.v.size() == 4) {
    plateIsRectangle = true;
    for (int i = 0; i < 4; ++i) {
      int j = (i + 1) % 4;
      if (plate.v[i].x != plate.v[j].x and plate.v[i].y != plate.v[j].y)
        plateIsRectangle = false;
    }
    if (plateIsRectangle) {
      println("plate is a rectangle");
      // 			plate.getBB(plateLeft, plateRight, plateTop, plateBottom);
      auto& plateBB = plate.get_bb();
      plateLeft = plateBB.left;
      plateRight = plateBB.right;
      plateTop = plateBB.top;
      plateBottom = plateBB.bottom;
    }
  }

  plate = superSample(plate, plateSuperSample);
  if (piecesSuperSample > 0) {
    println("super sample ", piecesSuperSample);
    for (auto& pc : pieces) {
      pc.pol = superSample(pc.pol, piecesSuperSample);
    }
  }
  if (infinitePolygons) {
    for (auto& pc : pieces) {
      pieceQuantity[pc.id] = 1 + plate.area() / pc.pol.area();
    }
  }

  if (phi != 0) {
    assert(anglesVector.empty());
    for (double i = 0; i <= 360; i += phi)
      anglesVector.push_back(degToRad(i));
  }
}
