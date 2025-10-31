// Copyright (c) Sleipnir contributors

#include "sleipnir/autodiff/variable_matrix.hpp"

namespace slp {

template VariableMatrix<double> solve(const VariableMatrix<double>& A,
                                      const VariableMatrix<double>& B);

}  // namespace slp
