// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <numeric>
#include <string_view>
#include <thread>

#include <fmt/format.h>
#include <wpi/Synchronization.h>

#include "ntcore.h"
#include "ntcore_cpp.h"

void bench();

int main(int argc, char* argv[]) {
  if (argc == 2 && std::string_view{argv[1]} == "bench") {
    bench();
    return EXIT_SUCCESS;
  }
  auto myValue = nt::GetEntry(nt::GetDefaultInstance(), "MyValue");

  nt::SetEntryValue(myValue, nt::Value::MakeString("Hello World"));

  fmt::print("{}\n", nt::GetEntryValue(myValue).GetString());
}

void PrintTimes(std::vector<int64_t>& times) {
  std::sort(times.begin(), times.end());
  int64_t min = times[0];
  int64_t max = times[times.size() - 1];
  double mean =
      static_cast<double>(std::accumulate(times.begin(), times.end(), 0)) /
      times.size();
  double sq_sum =
      std::inner_product(times.begin(), times.end(), times.begin(), 0);
  double stdev = std::sqrt(sq_sum / times.size() - mean * mean);

  fmt::print("min: {} max: {}, mean: {}, stdev: {}\n", min, max, mean, stdev);
  fmt::print("min 10: {}\n", fmt::join(times.begin(), times.begin() + 10, ","));
  fmt::print("max 10: {}\n", fmt::join(times.end() - 10, times.end(), ","));
}

// benchmark
void bench() {
  // set up instances
  auto client = nt::CreateInstance();
  auto server = nt::CreateInstance();

  // connect client and server
  nt::StartServer(server, "bench.json", "127.0.0.1", 0, 10000);
  nt::StartClient4(client, "client");
  nt::SetServer(client, "127.0.0.1", 10000);

  using namespace std::chrono_literals;
  std::this_thread::sleep_for(1s);

  // add "typical" set of subscribers on client and server
  nt::SubscribeMultiple(client, {{std::string_view{}}});
  nt::Subscribe(nt::GetTopic(client, "highrate"), NT_DOUBLE, "double",
                {{nt::PubSubOption::KeepDuplicates(true),
                  nt::PubSubOption::SendAll(true)}});
  nt::SubscribeMultiple(server, {{std::string_view{}}});
  auto pub = nt::Publish(nt::GetTopic(server, "highrate"), NT_DOUBLE, "double");
  nt::SetDouble(pub, 0);

  // warm up
  for (int i = 1; i <= 10000; ++i) {
    nt::SetDouble(pub, i * 0.01);
    if (i % 2000 == 0) {
      std::this_thread::sleep_for(0.02s);
    }
  }

  std::vector<int64_t> flushTimes;
  flushTimes.reserve(100);

  std::vector<int64_t> times;
  times.reserve(100001);

  // benchmark
  auto start = std::chrono::high_resolution_clock::now();
  int64_t now = nt::Now();
  for (int i = 1; i <= 100000; ++i) {
    nt::SetDouble(pub, i * 0.01, now);
    int64_t prev = now;
    now = nt::Now();
    times.emplace_back(now - prev);
    if (i % 2000 == 0) {
      nt::Flush(server);
      flushTimes.emplace_back(nt::Now() - now);
      std::this_thread::sleep_for(0.02s);
      now = nt::Now();
    }
  }
  auto stop = std::chrono::high_resolution_clock::now();

  fmt::print("total time: {}us\n",
             std::chrono::duration_cast<std::chrono::microseconds>(stop - start)
                 .count());
  PrintTimes(times);
  fmt::print("-- Flush --\n");
  PrintTimes(flushTimes);
}
