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
#include <chrono>
#include <cstring>
#include <ctime>
#include <string>

template <typename Clock = std::chrono::steady_clock> class Timer {
public:
  Timer() { restart(); }

  void restart() { t = Clock::now(); }

  template <typename R = std::milli, typename T = double> T elapsed() const {
    return std::chrono::duration_cast<std::chrono::duration<T, R>>(
               Clock::now() - t)
        .count();
  }

private:
  typename Clock::time_point t;
};

inline double timerSeconds(const Timer<>& timer) {
  return timer.elapsed() / 1000.0;
}

inline std::string currentDateTimeStr() {
  // taken from http://stackoverflow.com/a/10467633
  time_t now = time(0);
  struct tm tstruct;
  char buf[80];
  tstruct = *localtime(&now);
  strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tstruct);
  return buf;
  // auto now = chrono::system_clock::to_time_t(chrono::system_clock::now());
  // return std::ctime(&now);
}