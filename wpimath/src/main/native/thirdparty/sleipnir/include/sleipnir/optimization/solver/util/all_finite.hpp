// Copyright (c) Sleipnir contributors

#pragma once

#include <cmath>

#include <Eigen/SparseCore>

/// Returns true if elements of sparse matrix are all finite.
///
/// @param mat Sparse matrix.
/// @return True if elements of sparse matrix are all finite.
template <typename Derived>
bool all_finite(const Eigen::SparseCompressedBase<Derived>& mat) {
  using std::isfinite;

  for (int k = 0; k < mat.outerSize(); ++k) {
    for (typename Derived::InnerIterator it{mat, k}; it; ++it) {
      if (!isfinite(it.value())) {
        return false;
      }
    }
  }

  return true;
}
