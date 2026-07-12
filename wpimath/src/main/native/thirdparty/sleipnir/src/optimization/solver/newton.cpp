// Copyright (c) Sleipnir contributors

#include "sleipnir/optimization/solver/newton.hpp"

template SLEIPNIR_DLLEXPORT slp::ExitStatus slp::newton(
    const NewtonMatrixCallbacks<double>& matrix_callbacks,
    std::span<std::function<bool(const IterationInfo<double>& info)>>
        iteration_callbacks,
    const Options& options, Eigen::Vector<double, Eigen::Dynamic>& x);
