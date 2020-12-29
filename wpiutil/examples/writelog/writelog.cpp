/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <chrono>
#include <iostream>
#include <vector>
#include <array>
#include <string>
#include <numeric>

#include "wpi/DataLog.h"

int main(int argc, char** argv) {
  using std::chrono::duration_cast;
  using std::chrono::high_resolution_clock;
  using std::chrono::microseconds;

  int kNumRuns = 10;

  if (argc == 2) {
    kNumRuns = std::stoi(argv[1]);
  }

  auto testVec = std::vector<std::pair<std::string, void (*)()>>();

  testVec.push_back({"50 double append (test.log)", []{
    auto log =
        wpi::log::DoubleLog::Open("test.log", wpi::log::CD_CreateAlways);

    if (!log) {
      throw std::runtime_error("Could not open log");
    }

    for (int i = 0; i < 50; ++i) {
      log->Append(20000 * i, 1.3 * i);
    }
  }});

  testVec.push_back({"500k double append (test2.log)", []{
    wpi::log::DoubleLog::Config config;
    config.periodicFlush = 1000;
    auto log =
        wpi::log::DoubleLog::Open("test2.log", wpi::log::CD_CreateAlways);

    if (!log) {
      throw std::runtime_error("Could not open log");
    }

    for (uint64_t i = 0; i < 500000; ++i) {
      log->Append(20000 * i, 1.3 * i);
    }
  }});

  /*
  testVec.push_back("Int array append (test2.log)", []{
    auto data = std::vector<uint8_t>(5000);
    auto log = wpi::log::DataLog::Open("test2.log", "image", "image", 0,
                                       wpi::log::CD_OpenAlways);
    if (!log)
      throw std::runtime_error("Could not open log");
    
    for (int i = 0; i < 1000; ++i) log->Append(20000 * i, data);
  }
  */

  testVec.push_back({"50 string append (test-string.log)", []{
    auto log = wpi::log::StringLog::Open("test-string.log",
                                         wpi::log::CD_CreateAlways);
    if (!log) {
      throw std::runtime_error("Could not open log");
    }

    for (int i = 0; i < 50; ++i) {
      log->Append(20000 * i, "hello");
    }
  }});

  testVec.push_back({"Double array append (test-double-array.log)", []{
    auto log = wpi::log::DoubleArrayLog::Open("test-double-array.log",
                                              wpi::log::CD_CreateAlways);
    if (!log) {
      throw std::runtime_error("Could not open log");
    }

    log->Append(20000, {1, 2, 3});
    log->Append(30000, {4, 5});
  }});

  testVec.push_back({"String array append (test-string-array.log)", []{
    auto log = wpi::log::StringArrayLog::Open("test-string-array.log",
                                              wpi::log::CD_CreateAlways);
    if (!log) {
      throw std::runtime_error("Could not open log");
    }

    log->Append(20000, {"Hello", "World"});
    log->Append(30000, {"This", "Is", "Fun"});
  }});

  for (const auto& [name, fn] : testVec) {
    auto resVec = std::vector<microseconds::rep>();
    std::cout << name << ": ";

    for (int i = 0; i < kNumRuns; ++i) {
      auto start = high_resolution_clock::now();
      fn();
      auto stop = high_resolution_clock::now();
      resVec.push_back(duration_cast<microseconds>(stop - start).count());
    }

    std::cout << std::accumulate(resVec.begin(), resVec.end(), 0)/kNumRuns << "us\n";
  }

  return EXIT_SUCCESS;
}
