// Copyright (c) Sleipnir contributors

#include "sleipnir/optimization/solver/newton.hpp"

namespace slp {

template SLEIPNIR_DLLEXPORT ExitStatus
newton(const NewtonMatrixCallbacks<double>& matrix_callbacks,
       std::span<std::function<bool(const IterationInfo<double>& info)>>
           iteration_callbacks,
       const Options& options, Eigen::Vector<double, Eigen::Dynamic>& x);

}  // namespace slp
