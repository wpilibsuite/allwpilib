// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/spinlock.h"  // NOLINT(build/include_order)

#include <chrono>
#include <mutex>
#include <thread>

#include <gtest/gtest.h>

#include "wpi/mutex.h"
#include "wpi/print.h"

static std::mutex std_mutex;
static std::recursive_mutex std_recursive_mutex;
static wpi::mutex wpi_mutex;
static wpi::recursive_mutex wpi_recursive_mutex;
static wpi::spinlock spinlock;
static wpi::recursive_spinlock1 recursive_spinlock1;
static wpi::recursive_spinlock2 recursive_spinlock2;
static wpi::recursive_spinlock recursive_spinlock;

TEST(SpinlockTest, Benchmark) {
  using std::chrono::duration_cast;
  using std::chrono::high_resolution_clock;
  using std::chrono::microseconds;

  // warmup
  std::thread thr([]() {
    [[maybe_unused]]
    int value = 0;

    auto start = high_resolution_clock::now();
    for (int i = 0; i < 10000000; i++) {
      std::scoped_lock lock(std_mutex);
      ++value;
    }
    auto stop = high_resolution_clock::now();
    (void)start;
    (void)stop;
  });
  thr.join();

  std::thread thrb([]() {
    int value = 0;

    auto start = high_resolution_clock::now();
    for (int i = 0; i < 1000000; i++) {
      std::scoped_lock lock(std_mutex);
      ++value;
    }
    auto stop = high_resolution_clock::now();
    wpi::print("std::mutex sizeof: {} time: {} value: {}\n", sizeof(std_mutex),
               duration_cast<microseconds>(stop - start).count(), value);
  });
  thrb.join();

  std::thread thrb2([]() {
    int value = 0;

    auto start = high_resolution_clock::now();
    for (int i = 0; i < 1000000; i++) {
      std::scoped_lock lock(std_recursive_mutex);
      ++value;
    }
    auto stop = high_resolution_clock::now();
    wpi::print("std::recursive_mutex sizeof: {} time: {} value: {}\n",
               sizeof(std_recursive_mutex),
               duration_cast<microseconds>(stop - start).count(), value);
  });
  thrb2.join();

  std::thread thr2([]() {
    int value = 0;

    auto start = high_resolution_clock::now();
    for (int i = 0; i < 1000000; i++) {
      std::scoped_lock lock(wpi_mutex);
      ++value;
    }
    auto stop = high_resolution_clock::now();
    wpi::print("wpi::mutex sizeof: {} time: {} value: {}\n", sizeof(wpi_mutex),
               duration_cast<microseconds>(stop - start).count(), value);
  });
  thr2.join();

  std::thread thr2b([]() {
    int value = 0;

    auto start = high_resolution_clock::now();
    for (int i = 0; i < 1000000; i++) {
      std::scoped_lock lock(wpi_recursive_mutex);
      ++value;
    }
    auto stop = high_resolution_clock::now();
    wpi::print("wpi::recursive_mutex sizeof: {} time: {} value: {}\n",
               sizeof(wpi_recursive_mutex),
               duration_cast<microseconds>(stop - start).count(), value);
  });
  thr2b.join();

  std::thread thr3([]() {
    int value = 0;

    auto start = high_resolution_clock::now();
    for (int i = 0; i < 1000000; i++) {
      std::scoped_lock lock(spinlock);
      ++value;
    }
    auto stop = high_resolution_clock::now();
    wpi::print("spinlock sizeof: {} time: {} value: {}\n", sizeof(spinlock),
               duration_cast<microseconds>(stop - start).count(), value);
  });
  thr3.join();

  std::thread thr4([]() {
    int value = 0;

    auto start = high_resolution_clock::now();
    for (int i = 0; i < 1000000; i++) {
      std::scoped_lock lock(recursive_spinlock1);
      ++value;
    }
    auto stop = high_resolution_clock::now();
    wpi::print("recursive_spinlock1 sizeof: {} time: {} value: {}\n",
               sizeof(recursive_spinlock1),
               duration_cast<microseconds>(stop - start).count(), value);
  });
  thr4.join();

  std::thread thr4b([]() {
    int value = 0;

    auto start = high_resolution_clock::now();
    for (int i = 0; i < 1000000; i++) {
      std::scoped_lock lock(recursive_spinlock2);
      ++value;
    }
    auto stop = high_resolution_clock::now();
    wpi::print("recursive_spinlock2 sizeof: {} time: {} value: {}\n",
               sizeof(recursive_spinlock2),
               duration_cast<microseconds>(stop - start).count(), value);
  });
  thr4b.join();

  std::thread thr4c([]() {
    int value = 0;

    auto start = high_resolution_clock::now();
    for (int i = 0; i < 1000000; i++) {
      std::scoped_lock lock(recursive_spinlock);
      ++value;
    }
    auto stop = high_resolution_clock::now();
    wpi::print("recursive_spinlock sizeof: {} time: {} value: {}\n",
               sizeof(recursive_spinlock),
               duration_cast<microseconds>(stop - start).count(), value);
  });
  thr4c.join();
}
