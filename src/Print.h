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
#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <locale>
#include <sstream>
#include <string>

template <typename T> std::string toString(T val) {
  std::ostringstream ss;
  ss << val;
  return ss.str();
}

template <typename Head> void print(const Head& h) {
  if (not problem.silent) {
    std::cout << h;
  }
}

template <typename Head, typename... Tail>
void print(const Head& h, Tail... tail) {
  if (not problem.silent) {
    print(h);
    print(tail...);
  }
}

template <typename Head> void println(const Head& h) {
  if (not problem.silent) {
    std::cout << h << std::endl;
  }
}

template <typename Head, typename... Tail>
void println(const Head& h, Tail... tail) {
  if (not problem.silent) {
    print(h);
    print(tail...);
    print('\n');
  }
}

template <typename Head> void printForced(const Head& h) { std::cout << h; }

template <typename Head, typename... Tail>
void printForced(const Head& h, Tail... tail) {
  printForced(h);
  printForced(tail...);
}

template <typename Head> void printlnForced(const Head& h) {
  std::cout << h << std::endl;
}

template <typename Head, typename... Tail>
void printlnForced(const Head& h, Tail... tail) {
  printForced(h);
  printForced(tail...);
  printForced('\n');
}

struct comma_numpunct : public std::numpunct<char> {
protected:
  virtual char do_thousands_sep() const { return ','; }

  virtual std::string do_grouping() const { return "\03"; }
};