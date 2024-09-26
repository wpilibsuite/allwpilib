// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <chrono>
#include <numeric>
#include <string>
#include <utility>
#include <vector>

#include "wpi/DataLogBackgroundWriter.h"
#include "wpi/print.h"

int main(int argc, char** argv) {
  using std::chrono::duration_cast;
  using std::chrono::high_resolution_clock;
  using std::chrono::microseconds;

  int kNumRuns = 10;

  if (argc == 2) {
    kNumRuns = std::stoi(argv[1]);
  }

  wpi::log::DataLogBackgroundWriter log;
  log.SetFilename("test.wpilog");

  auto testVec =
      std::vector<std::pair<std::string, void (*)(wpi::log::DataLog&)>>();

  testVec.push_back({"50 double append", [](auto& log) {
                       wpi::log::DoubleLogEntry entry{log, "fifty", 1};
                       for (int i = 0; i < 50; ++i) {
                         entry.Append(1.3 * i, 20000 * i);
                       }
                     }});
#if 0
  testVec.push_back({"500k double append", [](auto& log) {
                       wpi::log::DoubleLogEntry entry{log, "500k", 1};
                       for (uint64_t i = 0; i < 500000; ++i) {
                         entry.Append(1.3 * i, 20000 * i);
                       }
                     }});
#endif
  testVec.push_back({"50 string append", [](auto& log) {
                       wpi::log::StringLogEntry entry{log, "string", 1};
                       for (int i = 0; i < 50; ++i) {
                         entry.Append("hello", 20000 * i);
                       }
                     }});

  testVec.push_back({"Double array append", [](auto& log) {
                       wpi::log::DoubleArrayLogEntry entry{log, "double_array",
                                                           1};
                       entry.Append({1, 2, 3}, 20000);
                       entry.Append({4, 5}, 30000);
                     }});

  testVec.push_back({"String array append", [](auto& log) {
                       wpi::log::StringArrayLogEntry entry{log, "string_array",
                                                           1};
                       entry.Append({"Hello", "World"}, 20000);
                       entry.Append({"This", "Is", "Fun"}, 30000);
                     }});

  for (const auto& [name, fn] : testVec) {
    auto resVec = std::vector<microseconds::rep>();
    wpi::print("{}: ", name);

    for (int i = 0; i < kNumRuns; ++i) {
      auto start = high_resolution_clock::now();
      fn(log);
      auto stop = high_resolution_clock::now();
      resVec.push_back(duration_cast<microseconds>(stop - start).count());
    }

    wpi::print("{}us\n",
               std::accumulate(resVec.begin(), resVec.end(), 0) / kNumRuns);
  }

  return EXIT_SUCCESS;
}
