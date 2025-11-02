// Copyright (c) Sleipnir contributors

#include "sleipnir/optimization/solver/interior_point.hpp"

namespace slp {

template SLEIPNIR_DLLEXPORT ExitStatus
interior_point(const InteriorPointMatrixCallbacks<double>& matrix_callbacks,
               std::span<std::function<bool(const IterationInfo<double>& info)>>
                   iteration_callbacks,
               const Options& options,
#ifdef SLEIPNIR_ENABLE_BOUND_PROJECTION
               const Eigen::ArrayX<bool>& bound_constraint_mask,
#endif
               Eigen::Vector<double, Eigen::Dynamic>& x);

}  // namespace slp
