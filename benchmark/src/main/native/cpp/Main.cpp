// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <benchmark/benchmark.h>

#include "CartPoleBenchmark.hpp"
#include "TravelingSalesmanBenchmark.hpp"

BENCHMARK(BM_CartPole);
BENCHMARK(BM_TravelingSalesman_Transform);
BENCHMARK(BM_TravelingSalesman_Twist);

BENCHMARK_MAIN();
