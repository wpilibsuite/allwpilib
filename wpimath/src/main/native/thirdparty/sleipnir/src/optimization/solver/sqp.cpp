// Copyright (c) Sleipnir contributors

#include "sleipnir/optimization/solver/sqp.hpp"

namespace slp {

template SLEIPNIR_DLLEXPORT ExitStatus
sqp(const SQPMatrixCallbacks<double>& matrix_callbacks,
    std::span<std::function<bool(const IterationInfo<double>& info)>>
        iteration_callbacks,
    const Options& options, Eigen::Vector<double, Eigen::Dynamic>& x);

}  // namespace slp
