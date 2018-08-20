/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "wpi/spinlock.h"  // NOLINT(build/include_order)

#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>

#include "gtest/gtest.h"
#include "wpi/mutex.h"

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
    int value = 0;

    auto start = high_resolution_clock::now();
    for (int i = 0; i < 10000000; i++) {
      std::lock_guard<std::mutex> lock(std_mutex);
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
      std::lock_guard<std::mutex> lock(std_mutex);
      ++value;
    }
    auto stop = high_resolution_clock::now();
    std::cout << "std::mutex sizeof: " << sizeof(std_mutex)
              << " time: " << duration_cast<microseconds>(stop - start).count()
              << " value: " << value << "\n";
  });
  thrb.join();

  std::thread thrb2([]() {
    int value = 0;

    auto start = high_resolution_clock::now();
    for (int i = 0; i < 1000000; i++) {
      std::lock_guard<std::recursive_mutex> lock(std_recursive_mutex);
      ++value;
    }
    auto stop = high_resolution_clock::now();
    std::cout << "std::recursive_mutex sizeof: " << sizeof(std_recursive_mutex)
              << " time: " << duration_cast<microseconds>(stop - start).count()
              << " value: " << value << "\n";
  });
  thrb2.join();

  std::thread thr2([]() {
    int value = 0;

    auto start = high_resolution_clock::now();
    for (int i = 0; i < 1000000; i++) {
      std::lock_guard<wpi::mutex> lock(wpi_mutex);
      ++value;
    }
    auto stop = high_resolution_clock::now();
    std::cout << "wpi::mutex sizeof: " << sizeof(wpi_mutex)
              << " time: " << duration_cast<microseconds>(stop - start).count()
              << " value: " << value << "\n";
  });
  thr2.join();

  std::thread thr2b([]() {
    int value = 0;

    auto start = high_resolution_clock::now();
    for (int i = 0; i < 1000000; i++) {
      std::lock_guard<wpi::recursive_mutex> lock(wpi_recursive_mutex);
      ++value;
    }
    auto stop = high_resolution_clock::now();
    std::cout << "wpi::recursive_mutex sizeof: " << sizeof(wpi_recursive_mutex)
              << " time: " << duration_cast<microseconds>(stop - start).count()
              << " value: " << value << "\n";
  });
  thr2b.join();

  std::thread thr3([]() {
    int value = 0;

    auto start = high_resolution_clock::now();
    for (int i = 0; i < 1000000; i++) {
      std::lock_guard<wpi::spinlock> lock(spinlock);
      ++value;
    }
    auto stop = high_resolution_clock::now();
    std::cout << "spinlock sizeof: " << sizeof(spinlock)
              << " time: " << duration_cast<microseconds>(stop - start).count()
              << " value: " << value << "\n";
  });
  thr3.join();

  std::thread thr4([]() {
    int value = 0;

    auto start = high_resolution_clock::now();
    for (int i = 0; i < 1000000; i++) {
      std::lock_guard<wpi::recursive_spinlock1> lock(recursive_spinlock1);
      ++value;
    }
    auto stop = high_resolution_clock::now();
    std::cout << "recursive_spinlock1 sizeof: " << sizeof(recursive_spinlock1)
              << " time: " << duration_cast<microseconds>(stop - start).count()
              << " value: " << value << "\n";
  });
  thr4.join();

  std::thread thr4b([]() {
    int value = 0;

    auto start = high_resolution_clock::now();
    for (int i = 0; i < 1000000; i++) {
      std::lock_guard<wpi::recursive_spinlock2> lock(recursive_spinlock2);
      ++value;
    }
    auto stop = high_resolution_clock::now();
    std::cout << "recursive_spinlock2 sizeof: " << sizeof(recursive_spinlock2)
              << " time: " << duration_cast<microseconds>(stop - start).count()
              << " value: " << value << "\n";
  });
  thr4b.join();

  std::thread thr4c([]() {
    int value = 0;

    auto start = high_resolution_clock::now();
    for (int i = 0; i < 1000000; i++) {
      std::lock_guard<wpi::recursive_spinlock> lock(recursive_spinlock);
      ++value;
    }
    auto stop = high_resolution_clock::now();
    std::cout << "recursive_spinlock sizeof: " << sizeof(recursive_spinlock)
              << " time: " << duration_cast<microseconds>(stop - start).count()
              << " value: " << value << "\n";
  });
  thr4c.join();
}
