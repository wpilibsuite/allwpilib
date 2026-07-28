// Copyright (c) Sleipnir contributors

#include "sleipnir/optimization/solver/sqp.hpp"

template SLEIPNIR_DLLEXPORT slp::ExitStatus slp::sqp(
    const SQPMatrixCallbacks<double>& matrix_callbacks,
    std::span<std::function<bool(const IterationInfo<double>& info)>>
        iteration_callbacks,
    const slp::Options& options, Eigen::Vector<double, Eigen::Dynamic>& x);
