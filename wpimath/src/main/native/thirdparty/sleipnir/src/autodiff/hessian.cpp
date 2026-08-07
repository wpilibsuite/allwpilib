// Copyright (c) Sleipnir contributors

#include "sleipnir/autodiff/hessian.hpp"

#include <Eigen/Core>

#include "sleipnir/util/symbol_exports.hpp"

template class EXPORT_TEMPLATE_DEFINE(SLEIPNIR_DLLEXPORT)
slp::Hessian<double, Eigen::Lower | Eigen::Upper>;
