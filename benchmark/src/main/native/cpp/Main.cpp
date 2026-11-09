// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <benchmark/benchmark.h>

#include "CartPoleBenchmark.hpp"
#include "CholeskyRankUpdateBenchmark.hpp"
#include "DAREBenchmark.hpp"
#include "Geometry3dBenchmark.hpp"
#include "MatrixExpBenchmark.hpp"
#include "TravelingSalesmanBenchmark.hpp"

BENCHMARK(BM_CartPole);
BENCHMARK(BM_CholeskyRankDowndate);
BENCHMARK(BM_CholeskyRankUpdate);
BENCHMARK(BM_DARE);
BENCHMARK(BM_Geometry3d_Twist3d_Exp);
BENCHMARK(BM_Geometry3d_Transform3d_Log);
BENCHMARK(BM_MatrixExp);
BENCHMARK(BM_TravelingSalesman_Transform);
BENCHMARK(BM_TravelingSalesman_Twist);

BENCHMARK_MAIN();
