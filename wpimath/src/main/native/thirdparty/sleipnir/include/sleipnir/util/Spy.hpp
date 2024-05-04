// Copyright (c) Sleipnir contributors

#pragma once

#include <fstream>
#include <string>
#include <string_view>

#include <Eigen/SparseCore>

#include "sleipnir/util/SmallVector.hpp"
#include "sleipnir/util/SymbolExports.hpp"

namespace sleipnir {

/**
 * Write the sparsity pattern of a sparse matrix to a file.
 *
 * Each character represents an element with '.' representing zero, '+'
 * representing positive, and '-' representing negative. Here's an example for a
 * 3x3 identity matrix.
 *
 * "+.."
 * ".+."
 * "..+"
 *
 * @param[out] file A file stream.
 * @param[in] mat The sparse matrix.
 */
SLEIPNIR_DLLEXPORT inline void Spy(std::ostream& file,
                                   const Eigen::SparseMatrix<double>& mat) {
  const int cells_width = mat.cols() + 1;
  const int cells_height = mat.rows();

  small_vector<uint8_t> cells;

  // Allocate space for matrix of characters plus trailing newlines
  cells.reserve(cells_width * cells_height);

  // Initialize cell array
  for (int row = 0; row < mat.rows(); ++row) {
    for (int col = 0; col < mat.cols(); ++col) {
      cells.emplace_back('.');
    }
    cells.emplace_back('\n');
  }

  // Fill in non-sparse entries
  for (int k = 0; k < mat.outerSize(); ++k) {
    for (Eigen::SparseMatrix<double>::InnerIterator it{mat, k}; it; ++it) {
      if (it.value() < 0.0) {
        cells[it.row() * cells_width + it.col()] = '-';
      } else if (it.value() > 0.0) {
        cells[it.row() * cells_width + it.col()] = '+';
      }
    }
  }

  // Write cell array to file
  for (const auto& c : cells) {
    file << c;
  }
}

/**
 * Write the sparsity pattern of a sparse matrix to a file.
 *
 * Each character represents an element with "." representing zero, "+"
 * representing positive, and "-" representing negative. Here's an example for a
 * 3x3 identity matrix.
 *
 * "+.."
 * ".+."
 * "..+"
 *
 * @param[in] filename The filename.
 * @param[in] mat The sparse matrix.
 */
SLEIPNIR_DLLEXPORT inline void Spy(std::string_view filename,
                                   const Eigen::SparseMatrix<double>& mat) {
  std::ofstream file{std::string{filename}};
  if (!file.is_open()) {
    return;
  }

  Spy(file, mat);
}

}  // namespace sleipnir
