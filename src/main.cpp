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
#include "Print.h"
#include "Problem.h"
#include "Random.h"
#include "Statistics.h"
#include <boost/program_options.hpp>
#include <cstdlib>
#include <ctime>
#include <iostream>

RNG rng;

using namespace std;

void commandLine(int argc, char** argv) {
  namespace po = boost::program_options;

  po::options_description desc("Allowed options");
  desc.add_options()("help", "")(
      "in", po::value<string>(&problem.inputFile)->required(), "input file.")(
      "out", po::value<string>(&problem.outputFile)->default_value("out.txt"),
      "output file.")(
      "seed", po::value<int>(&problem.randomSeed)->default_value(0),
      "random seed to be used. If 0, a random random seed will used.")(
      "m", po::value<int>(&problem.m)->default_value(1))(
      "optimize",
      po::value<string>(&problem.optimizeString)
          ->default_value("length,width,compaction"),
      "values to optimize; can be length, compaction, width, or a "
      "combintion of the three (ex: --optimize=length|compaction|width.")(
      "phi", po::value<double>(&problem.phi)->default_value(0),
      "the angle step by the pieces may be rotated. If 0, then the "
      "angles specified by the input file are used.")(
      "supersample",
      po::value<int>(&problem.piecesSuperSample)->default_value(0),
      "a super-sample value of k means that, between every two vertices "
      "in each polygon, k vertices will be added. This means extra "
      "precision.")(
      "platesupersample",
      po::value<int>(&problem.plateSuperSample)->default_value(5),
      "super-sampling of the plate is necessary, in case no "
      "polygon can be placed at the edges. If set to be lower than 1, "
      "the problem may be infeasible.")(
      "finitepolygons",
      "if this option is set, the program will not add "
      "more polygons than the number available in the instance "
      "description.")(
      "nobb", "if set, no bounding-box tests will be performed (they are "
              "performed by default).")(
      "time", po::value<int>(&problem.timeLimitSeconds)->default_value(600),
      "time limit (seconds)")(
      "silent", "if this option is set, will only output final value")
      // 		("iterated", "use proposed iterated greedy approach")
      ("deconstruct",
       po::value<double>(&problem.areaPercentToDeconstruct)->default_value(0.4),
       "part of area to deconstruct, if "
       "using an iterated greedy approach")
      // 		("alphagreedy", "use alpha greedy approach (it is used by default
      // when" 			" the \'iterated\' option is set")
      ("alpha", po::value<double>(&problem.alpha)->default_value(0.01),
       "alpha paramter for alpha greedy")("random",
                                          "use random placement algorithm");

  po::variables_map vm;
  try {
    po::store(po::parse_command_line(argc, argv, desc), vm);
    if (vm.count("help")) {
      print(desc);
      exit(EXIT_SUCCESS);
    } else {
      po::notify(vm);
    }

    problem.optimizeCompaction =
        (problem.optimizeString.find("compaction") != string::npos);
    problem.optimizeLength =
        (problem.optimizeString.find("length") != string::npos);
    problem.optimizeWidth =
        (problem.optimizeString.find("width") != string::npos);
    problem.infinitePolygons = not vm.count("finitepolygons");
    problem.useBB = not(vm.count("nobb"));
    problem.silent = vm.count("silent");
    problem.iteratedGreedy = problem.areaPercentToDeconstruct > 0.0;
    problem.alphaGreedy = problem.alpha > 0.0;
    problem.randomAlgorithm = vm.count("random");

    assert(problem.optimizeCompaction or problem.optimizeLength or
           problem.optimizeWidth);

    rng.seed(problem.randomSeed ? problem.randomSeed : time(NULL));

  } catch (po::error& e) {
    println("error: ", e.what(), ".");
    print(desc);
    exit(EXIT_FAILURE);
  }
}

int main(int argc, char** argv) {
  commandLine(argc, argv);
  problem.readInstance();
  problem.timer.restart();
  alg.run();
  alg.writeOutput();
  statistics.printFinalStatistics();
}