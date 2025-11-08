// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <limits>
#include <numeric>
#include <random>
#include <string_view>
#include <thread>
#include <vector>

#include <fmt/format.h>
#include <fmt/ranges.h>

#include "wpi/nt/DoubleArrayTopic.hpp"
#include "wpi/nt/NetworkTableInstance.hpp"
#include "wpi/nt/ntcore.h"
#include "wpi/nt/ntcore_c.h"
#include "wpi/nt/ntcore_cpp.hpp"
#include "wpi/util/Synchronization.h"
#include "wpi/util/print.hpp"
#include "wpi/util/timestamp.h"

void bench();
void bench2();
void stress();
void stress2();
void latency();

int main(int argc, char* argv[]) {
  if (argc == 2 && std::string_view{argv[1]} == "bench") {
    bench();
    return EXIT_SUCCESS;
  }
  if (argc == 2 && std::string_view{argv[1]} == "bench2") {
    bench2();
    return EXIT_SUCCESS;
  }
  if (argc == 2 && std::string_view{argv[1]} == "stress") {
    stress();
    return EXIT_SUCCESS;
  }
  if (argc == 2 && std::string_view{argv[1]} == "stress2") {
    stress2();
    return EXIT_SUCCESS;
  }
  if (argc == 2 && std::string_view{argv[1]} == "latency") {
    latency();
    return EXIT_SUCCESS;
  }

  auto myValue = wpi::nt::GetEntry(wpi::nt::GetDefaultInstance(), "MyValue");

  wpi::nt::SetEntryValue(myValue, wpi::nt::Value::MakeString("Hello World"));

  wpi::util::print("{}\n", wpi::nt::GetEntryValue(myValue).GetString());
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

  wpi::util::print("min: {} max: {}, mean: {}, stdev: {}\n", min, max, mean, stdev);
  wpi::util::print("min 10: {}\n", fmt::join(times.begin(), times.begin() + 10, ","));
  wpi::util::print("max 10: {}\n", fmt::join(times.end() - 10, times.end(), ","));
}

// benchmark
void bench() {
  // set up instances
  auto client = wpi::nt::CreateInstance();
  auto server = wpi::nt::CreateInstance();

  // connect client and server
  wpi::nt::StartServer(server, "bench.json", "127.0.0.1", 10000);
  wpi::nt::StartClient(client, "client");
  wpi::nt::SetServer(client, "127.0.0.1", 10000);

  using namespace std::chrono_literals;
  std::this_thread::sleep_for(1s);

  // add "typical" set of subscribers on client and server
  wpi::nt::SubscribeMultiple(client, {{std::string_view{}}});
  wpi::nt::Subscribe(wpi::nt::GetTopic(client, "highrate"), NT_DOUBLE, "double",
                {.sendAll = true, .keepDuplicates = true});
  wpi::nt::SubscribeMultiple(server, {{std::string_view{}}});
  auto pub = wpi::nt::Publish(wpi::nt::GetTopic(server, "highrate"), NT_DOUBLE, "double");
  wpi::nt::SetDouble(pub, 0);

  // warm up
  for (int i = 1; i <= 10000; ++i) {
    wpi::nt::SetDouble(pub, i * 0.01);
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
  int64_t now = wpi::nt::Now();
  for (int i = 1; i <= 100000; ++i) {
    wpi::nt::SetDouble(pub, i * 0.01, now);
    int64_t prev = now;
    now = wpi::nt::Now();
    times.emplace_back(now - prev);
    if (i % 2000 == 0) {
      wpi::nt::Flush(server);
      flushTimes.emplace_back(wpi::nt::Now() - now);
      std::this_thread::sleep_for(0.02s);
      now = wpi::nt::Now();
    }
  }
  auto stop = std::chrono::high_resolution_clock::now();

  wpi::util::print("total time: {}us\n",
             std::chrono::duration_cast<std::chrono::microseconds>(stop - start)
                 .count());
  PrintTimes(times);
  wpi::util::print("-- Flush --\n");
  PrintTimes(flushTimes);
}

void bench2() {
  // set up instances
  auto client1 = wpi::nt::CreateInstance();
  auto client2 = wpi::nt::CreateInstance();
  auto server = wpi::nt::CreateInstance();

  // connect client and server
  wpi::nt::StartServer(server, "bench2.json", "127.0.0.1", 10000);
  wpi::nt::StartClient(client1, "client1");
  wpi::nt::StartClient(client2, "client2");
  wpi::nt::SetServer(client1, "127.0.0.1", 10000);
  wpi::nt::SetServer(client2, "127.0.0.1", 10000);

  using namespace std::chrono_literals;
  std::this_thread::sleep_for(1s);

  // add "typical" set of subscribers on client and server
  wpi::nt::SubscribeMultiple(client1, {{std::string_view{}}});
  wpi::nt::SubscribeMultiple(client2, {{std::string_view{}}});
  wpi::nt::SubscribeMultiple(server, {{std::string_view{}}});

  // create 1000 entries
  std::array<NT_Entry, 1000> pubs;
  for (int i = 0; i < 1000; ++i) {
    pubs[i] = wpi::nt::GetEntry(
        wpi::nt::GetTopic(server,
                     fmt::format("/some/long/name/with/lots/of/slashes/{}", i)),
        NT_DOUBLE_ARRAY, "double[]");
  }

  // warm up
  for (int i = 1; i <= 100; ++i) {
    for (auto pub : pubs) {
      double vals[3] = {i * 0.01, i * 0.02, i * 0.03};
      wpi::nt::SetDoubleArray(pub, vals);
    }
    wpi::nt::FlushLocal(server);
    std::this_thread::sleep_for(0.02s);
  }

  std::vector<int64_t> flushTimes;
  flushTimes.reserve(1001);

  std::vector<int64_t> times;
  times.reserve(1001);

  // benchmark
  auto start = std::chrono::high_resolution_clock::now();
  int64_t now = wpi::nt::Now();
  for (int i = 1; i <= 1000; ++i) {
    for (auto pub : pubs) {
      double vals[3] = {i * 0.01, i * 0.02, i * 0.03};
      wpi::nt::SetDoubleArray(pub, vals);
    }
    int64_t prev = now;
    now = wpi::nt::Now();
    times.emplace_back(now - prev);
    wpi::nt::FlushLocal(server);
    wpi::nt::Flush(server);
    flushTimes.emplace_back(wpi::nt::Now() - now);
    std::this_thread::sleep_for(0.02s);
    now = wpi::nt::Now();
  }
  auto stop = std::chrono::high_resolution_clock::now();

  wpi::util::print("total time: {}us\n",
             std::chrono::duration_cast<std::chrono::microseconds>(stop - start)
                 .count());
  PrintTimes(times);
  wpi::util::print("-- Flush --\n");
  PrintTimes(flushTimes);
}

static std::random_device r;
static std::mt19937 gen(r());
static std::uniform_real_distribution<double> dist;

void stress() {
  auto server = wpi::nt::CreateInstance();
  wpi::nt::StartServer(server, "stress.json", "127.0.0.1", 10000);
  wpi::nt::SubscribeMultiple(server, {{std::string_view{}}});

  using namespace std::chrono_literals;

  for (int count = 0; count < 10; ++count) {
    std::thread{[] {
      auto client = wpi::nt::CreateInstance();
      wpi::nt::SubscribeMultiple(client, {{std::string_view{}}});
      for (int i = 0; i < 300; ++i) {
        // sleep a random amount of time
        std::this_thread::sleep_for(0.1s * dist(gen));

        // connect
        wpi::nt::StartClient(client, "client");
        wpi::nt::SetServer(client, "127.0.0.1", 10000);

        // sleep a random amount of time
        std::this_thread::sleep_for(0.1s * dist(gen));

        // disconnect
        wpi::nt::StopClient(client);
      }
      wpi::nt::DestroyInstance(client);
    }}.detach();

    std::thread{[server, count] {
      for (int n = 0; n < 300; ++n) {
        // sleep a random amount of time
        std::this_thread::sleep_for(0.01s * dist(gen));

        // create publishers
        NT_Publisher pub[30];
        for (int i = 0; i < 30; ++i) {
          pub[i] =
              wpi::nt::Publish(wpi::nt::GetTopic(server, fmt::format("{}_{}", count, i)),
                          NT_DOUBLE, "double", {});
        }

        // publish values
        for (int i = 0; i < 200; ++i) {
          // sleep a random amount of time between each value set
          std::this_thread::sleep_for(0.001s * dist(gen));
          for (int i = 0; i < 30; ++i) {
            wpi::nt::SetDouble(pub[i], dist(gen));
          }
          wpi::nt::FlushLocal(server);
        }

        // sleep a random amount of time
        std::this_thread::sleep_for(0.1s * dist(gen));

        // remove publishers
        for (int i = 0; i < 30; ++i) {
          wpi::nt::Unpublish(pub[i]);
        }
      }
    }}.detach();
  }

  std::this_thread::sleep_for(100s);
}

void stress2() {
  using namespace std::chrono_literals;

  auto testTopicName = "testTopic";
  auto count = 1000;
  std::atomic_bool isDone{false};
  wpi::nt::PubSubOptions pubSubOptions{
      .periodic = std::numeric_limits<double>::min(),
      .sendAll = true,
      .keepDuplicates = true};
  auto server = wpi::nt::NetworkTableInstance::Create();
  server.StartServer();
  auto serverTopic = server.GetDoubleArrayTopic(testTopicName);
  auto subscriber = serverTopic.Subscribe({}, pubSubOptions);
  std::atomic_int receivedCount{0};
  server.AddListener(subscriber, NT_EVENT_VALUE_REMOTE, [&](auto event) {
    if (receivedCount.fetch_add(1) == count) {
      isDone = true;
    }
    // Warnings about duplicate pubs occur if I either introduce this short
    // delay...
    std::this_thread::sleep_for(1ms);
    // ...or a little IO
    // System.out.println("Got %d: %s"
    // .formatted(receivedCount.get(), Arrays.toString(
    // event.valueData.value.getDoubleArray())));
  });

  auto client = wpi::nt::NetworkTableInstance::Create();
  client.SetServer("localhost");
  auto clientName = "test client";
  client.StartClient(clientName);
  std::this_thread::sleep_for(2s);  // Startup time.
  int sentCount = 0;
  while (sentCount < count) {
    auto clientTopic = client.GetDoubleArrayTopic(testTopicName);
    {
      auto publisher = clientTopic.Publish(pubSubOptions);
      publisher.Set(
          {{static_cast<double>(sentCount), static_cast<double>(sentCount),
            static_cast<double>(sentCount)}});
      // client.Flush();
      sentCount++;
    }
    std::this_thread::yield();
  }

  std::this_thread::sleep_for(10s);
  fmt::print("isDone: {}", isDone.load());
}

void latency() {
  // set up instances
  auto client1 = wpi::nt::CreateInstance();
  auto client2 = wpi::nt::CreateInstance();
  auto server = wpi::nt::CreateInstance();

  // connect client and server
  wpi::nt::StartServer(server, "latency.json", "127.0.0.1", 10000);
  wpi::nt::StartClient(client1, "client1");
  wpi::nt::SetServer(client1, "127.0.0.1", 10000);
  wpi::nt::StartClient(client2, "client2");
  wpi::nt::SetServer(client2, "127.0.0.1", 10000);

  using namespace std::chrono_literals;
  std::this_thread::sleep_for(1s);

  // create publishers and subscribers
  auto pub =
      wpi::nt::Publish(wpi::nt::GetTopic(client1, "highrate"), NT_DOUBLE, "double");
  wpi::nt::SubscribeMultiple(server, {{std::string_view{}}});
  auto sub =
      wpi::nt::Subscribe(wpi::nt::GetTopic(server, "highrate"), NT_DOUBLE, "double");
  auto sub2 =
      wpi::nt::Subscribe(wpi::nt::GetTopic(client2, "highrate"), NT_DOUBLE, "double");

  std::this_thread::sleep_for(1s);

  wpi::nt::SetDouble(pub, 0);
#if 0
  // warm up
  for (int i = 1; i <= 10000; ++i) {
    wpi::nt::SetDouble(pub, i * 0.01);
    if (i % 2000 == 0) {
      std::this_thread::sleep_for(0.02s);
    }
  }
#endif

  std::vector<int64_t> times;
  times.reserve(1001);

  // benchmark client to server
  for (int i = 1; i <= 1000; ++i) {
    int64_t sendTime = wpi::nt::Now();
    wpi::nt::SetDouble(pub, i, sendTime);
    wpi::nt::Flush(client1);
    while (wpi::nt::GetDouble(sub, 0) != i) {
      wpi::util::WaitForObject(sub);
    }
    times.emplace_back(wpi::nt::Now() - sendTime);
  }
  PrintTimes(times);

  // benchmark client to client
  times.resize(0);
  for (int i = 2001; i <= 3000; ++i) {
    int64_t sendTime = wpi::nt::Now();
    wpi::nt::SetDouble(pub, i, sendTime);
    wpi::nt::Flush(client1);
    while (wpi::nt::GetDouble(sub2, 0) != i) {
      wpi::util::WaitForObject(sub2);
    }
    times.emplace_back(wpi::nt::Now() - sendTime);
  }

  PrintTimes(times);
}
