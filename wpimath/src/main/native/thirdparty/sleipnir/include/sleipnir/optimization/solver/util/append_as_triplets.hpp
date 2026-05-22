// Copyright (c) Sleipnir contributors

#pragma once

#include <initializer_list>

#include <Eigen/SparseCore>
#include <gch/small_vector.hpp>

/// Appends sparse matrices to list of triplets at the given offset.
///
/// The triplets are appended in column-major order (e.g., first column of mat1,
/// first column of mat2 underneath first column of mat1, second column of mat1,
/// second column of mat2 underneath second column of mat1).
///
/// @tparam Scalar Scalar type.
/// @param triplets List of triplets.
/// @param row_offset Row offset for first matrix.
/// @param col_offset Column offset for first matrix.
/// @param mats Sparse matrices to append.
template <typename Scalar>
void append_as_triplets(
    gch::small_vector<Eigen::Triplet<Scalar>>& triplets, int row_offset,
    int col_offset, std::initializer_list<Eigen::SparseMatrix<Scalar>> mats) {
  // Compute row offset for each matrix
  gch::small_vector<int> mat_row_offsets;
  int mat_row_offset = 0;
  for (const auto& mat : mats) {
    mat_row_offsets.emplace_back(mat_row_offset);
    mat_row_offset += mat.rows();
  }

  // Append elements in column-major order
  for (int col = 0; col < mats.begin()[0].cols(); ++col) {
    for (size_t i = 0; i < mats.size(); ++i) {
      for (typename Eigen::SparseMatrix<Scalar>::InnerIterator it{
               mats.begin()[i], col};
           it; ++it) {
        triplets.emplace_back(row_offset + mat_row_offsets[i] + it.row(),
                              col_offset + it.col(), it.value());
      }
    }
  }
}

/// Append diagonal matrix to list of triplets at the given offset.
///
/// @tparam Scalar Scalar type.
/// @param triplets List of triplets.
/// @param row_offset Row offset for first matrix.
/// @param col_offset Column offset for first matrix.
/// @param diag Diagonal of matrix.
template <typename Scalar>
void append_diagonal_as_triplets(
    gch::small_vector<Eigen::Triplet<Scalar>>& triplets, int row_offset,
    int col_offset, const Eigen::Vector<Scalar, Eigen::Dynamic>& diag) {
  for (int row = 0; row < diag.rows(); ++row) {
    triplets.emplace_back(row_offset + row, col_offset + row, diag[row]);
  }
}
