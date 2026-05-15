/*
 * Copyright (C) Photon Vision.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "mrcal-uncertainty.hpp"
#include <Eigen/SparseLU>
#include <chrono>
#include <memory>
#include <utility>
#include <vector>

using namespace cv;

using EigenPoint2 = Eigen::Matrix<double, 2, 1>;
using EigenPoint3 = Eigen::Matrix<double, 3, 1>;

// 10.5 seconds. definitely still CPU bound
// using SolverType = Eigen::CholmodSupernodalLLT<Eigen::SparseMatrix<double,
// Eigen::ColMajor>>;

// 10.5 seconds
using SolverType =
    Eigen::SparseLU<Eigen::SparseMatrix<double, Eigen::ColMajor>>;

// 11 seconds
// using SolverType = Eigen::SimplicialLLT<Eigen::SparseMatrix<double,
// Eigen::ColMajor>>;

struct CalibrationUncertaintyContext {
  std::unique_ptr<SolverType> solver;
  Eigen::SparseMatrix<double, Eigen::ColMajor>
      J_observations; // J matrix for board observations only
  double observed_pixel_uncertainty;
  int Nstate;
  int Nmeasurements_boards;
};

// Helper to create CHOLMOD factorization from sparse Jt
cholmod_factor *create_factorization(cholmod_sparse *Jt,
                                     cholmod_common *common) {
  // Compute JtJ (which is Jt * Jt^T since Jt is already transposed)
  cholmod_sparse *JtJ = cholmod_aat(Jt, nullptr, 0, 1, common);
  if (!JtJ) {
    throw std::runtime_error("cholmod_aat failed");
  }

  // Factorize JtJ
  cholmod_factor *factorization = cholmod_analyze(JtJ, common);
  if (!factorization) {
    cholmod_free_sparse(&JtJ, common);
    throw std::runtime_error("cholmod_analyze failed");
  }

  if (!cholmod_factorize(JtJ, factorization, common)) {
    cholmod_free_factor(&factorization, common);
    cholmod_free_sparse(&JtJ, common);
    throw std::runtime_error("cholmod_factorize failed");
  }

  cholmod_free_sparse(&JtJ, common);
  return factorization;
}

inline double worst_direction_stdev(const Eigen::Matrix2d &cov) {
  // Direct formula for 2x2: sqrt((a+c)/2 + sqrt((a-c)^2/4 + b^2))
  double a = cov(0, 0);
  double b = cov(1, 0);
  double c = cov(1, 1);

  return std::sqrt((a + c) / 2.0 + std::sqrt((a - c) * (a - c) / 4.0 + b * b));
}

Eigen::MatrixXd solve_xt_JtJ_bt(
    cholmod_factor *factorization,
    const Eigen::Matrix<double, 2, Eigen::Dynamic> &bt, // shape (2, Nstate)
    cholmod_common *common) {
  int Nstate = bt.cols();
  int Nrhs = bt.rows();

  if (factorization->n != static_cast<size_t>(Nstate)) {
    throw std::runtime_error("Dimension mismatch in solve");
  }

  // Transpose bt to column-major for CHOLMOD
  Eigen::MatrixXd bt_col_major = bt.transpose(); // (Nstate, Nrhs)

  cholmod_dense b = {.nrow = static_cast<size_t>(Nstate),
                     .ncol = static_cast<size_t>(Nrhs),
                     .nzmax = static_cast<size_t>(Nrhs * Nstate),
                     .d = static_cast<size_t>(Nstate),
                     .x = bt_col_major.data(),
                     .z = nullptr,
                     .xtype = CHOLMOD_REAL,
                     .dtype = CHOLMOD_DOUBLE};

  Eigen::MatrixXd result(Nstate, Nrhs);
  cholmod_dense out = {.nrow = static_cast<size_t>(Nstate),
                       .ncol = static_cast<size_t>(Nrhs),
                       .nzmax = static_cast<size_t>(Nrhs * Nstate),
                       .d = static_cast<size_t>(Nstate),
                       .x = result.data(),
                       .z = nullptr,
                       .xtype = CHOLMOD_REAL,
                       .dtype = CHOLMOD_DOUBLE};

  cholmod_dense *M = &out;
  cholmod_dense *Y = nullptr;
  cholmod_dense *E = nullptr;

  if (!cholmod_solve2(CHOLMOD_A, factorization, &b, nullptr, &M, nullptr, &Y,
                      &E, common)) {
    throw std::runtime_error("cholmod_solve2 failed");
  }

  if (M != &out) {
    throw std::runtime_error("cholmod_solve2 reallocated output");
  }

  return result;
}

std::vector<mrcal_point2_t> sample_imager(Size numSamples, Size imagerSize) {
  std::vector<mrcal_point2_t> samples;
  samples.reserve(numSamples.width * numSamples.height);

  for (int j = 0; j < numSamples.height; j++) {
    for (int i = 0; i < numSamples.width; i++) {
      double x, y;

      // linspace formula: start + (stop - start) * i / (n - 1)
      if (numSamples.width == 1) {
        x = 0;
      } else {
        x = (imagerSize.width - 1) * i / (numSamples.width - 1.0);
      }

      if (numSamples.height == 1) {
        y = 0;
      } else {
        y = (imagerSize.height - 1) * j / (numSamples.height - 1.0);
      }

      samples.push_back({.x = x, .y = y});
    }
  }
  return samples;
}

// The derivative of q (pixel space location/s) wrt b (state vector)
Eigen::Matrix<double, 2, Eigen::Dynamic, Eigen::RowMajor>
_dq_db_projection_uncertainty(mrcal_point3_t pcam, mrcal_lensmodel_t lensmodel,
                              std::span<mrcal_pose_t> rt_ref_frame, int Nstate,
                              int istate_frames0,
                              std::span<double> intrinsics) {
  // project with gradients
  // model_analysis.py:1067
  mrcal_point2_t q{};
  Eigen::Matrix<double, 2, 3, Eigen::RowMajor> dq_dpcam;
  std::vector<double> dq_dintrinsics(2 * 1ull * intrinsics.size());
  bool ret = mrcal_project(
      // out
      &q, reinterpret_cast<mrcal_point3_t *>(dq_dpcam.data()),
      dq_dintrinsics.data(),
      // in
      &pcam, 1, &lensmodel, intrinsics.data());

  if (!ret) {
    throw std::runtime_error("mrcal_project_with_gradients failed");
  }

  // number of boards
  const size_t Nboards{rt_ref_frame.size()};

  // p_ref = pcam rotated by r (always zero1)
  Eigen::Matrix<double, 1, 3> p_ref{pcam.x, pcam.y, pcam.z};

  // prepare dq_db. Mrcal does this as a 40x60x2xNstate tensor, but we
  // are only projecting one point
  Eigen::Matrix<double, 2, Eigen::Dynamic> dq_db(2, Nstate);
  dq_db.setZero();

  // set dq_db[0:num intrinsics] = [dq_dintrinsics]
  Eigen::Map<Eigen::Matrix<double, 2, Eigen::Dynamic, Eigen::RowMajor>>
      dq_dintrinsics_eigen(dq_dintrinsics.data(), 2, intrinsics.size());
  dq_db.leftCols(intrinsics.size()) = dq_dintrinsics_eigen;

  // determine dpcam_dr and dpcamp_dpref
  Eigen::Matrix<double, 3, 3> dpcam_dpref; // dxout/dxin
  Eigen::Matrix<double, 3, 3> dpcam_dr;    // dx_out/dr
  {
    // HACK -- hard-coded to origin
    Eigen::Matrix<double, 1, 6> rt_cam_ref;
    rt_cam_ref.setZero();

    // output arrays
    mrcal_point3_t rotated_p_ref;

    mrcal_rotate_point_r(
        // out
        rotated_p_ref.xyz, dpcam_dr.data(), dpcam_dpref.data(),
        // in
        rt_cam_ref.data(), p_ref.data());
  }

  // method is always mean-pcam
  Eigen::Matrix<double, 2, 3> dq_dpref = dq_dpcam * dpcam_dpref;

  // calculate p_frames
  Eigen::Matrix<double, Eigen::Dynamic, 3, Eigen::RowMajor> p_frames(Nboards,
                                                                     3);
  p_frames.setZero();
  {
    // output arrays

    for (size_t pose = 0; pose < Nboards; pose++) {
      mrcal_rotate_point_r_inverted(
          // out
          p_frames.row(pose).data(), NULL, NULL,
          // in
          rt_ref_frame[pose].r.xyz, p_ref.data());
    }
  }

  // and rotate to yield dpref_dframes
  std::vector<Eigen::Matrix<double, 3, 3, Eigen::RowMajor>> dpref_dframes;
  dpref_dframes.resize(Nboards);
  for (size_t pose = 0; pose < Nboards; pose++) {
    mrcal_point3_t dummy;
    mrcal_rotate_point_r(
        // out
        dummy.xyz, dpref_dframes[pose].data(), NULL,
        // in
        rt_ref_frame[pose].r.xyz, p_frames.row(pose).data());
  }

  // Calculate dq_dframes
  std::vector<Eigen::Matrix<double, 2, 3>> dq_dframes;
  dq_dframes.resize(Nboards);
  for (size_t pose = 0; pose < Nboards; pose++) {
    dq_dframes[pose] = dq_dpref * dpref_dframes[pose];
  }

  // Populate dq_db_slice_frames
  // Shape after mean and xchg: (2, 3) for at_infinity
  // Each frame gets 3 DOF (translation only)
  for (size_t frame = 0; frame < Nboards; frame++) {
    int frame_start = istate_frames0 + frame * 6;
    // Populate first 3 columns of each frame's 6 DOF block with the mean
    dq_db.block(0, frame_start, 2, 3) = dq_dframes[frame] / Nboards;
  }

  return dq_db;
}

double _observed_pixel_uncertainty_from_inputs(std::vector<double> &x,
                                               int num_measurements_board,
                                               int measurement_index_board) {
  // Compute variance from residuals
  double sum = 0.0, sum_sq = 0.0;
  for (size_t i = measurement_index_board;
       i < measurement_index_board + num_measurements_board; i++) {
    double val = x[i];
    sum += val;
    sum_sq += val * val;
  }
  double mean = sum / x.size();
  double variance = (sum_sq / x.size()) - (mean * mean);

  return std::sqrt(variance);
}

CalibrationUncertaintyContext create_calibration_uncertainty_context(
    mrcal_problem_selections_t &problem_selections,
    mrcal_lensmodel_t &lensmodel, const std::span<double> intrinsics,
    const std::span<mrcal_pose_t> rt_ref_frame,
    const mrcal_observation_board_t *observations_board,
    const mrcal_point3_t *observations_board_pool, int Nobservations_board,
    int calibration_object_width_n, int calibration_object_height_n,
    double calibration_object_spacing, cv::Size imagerSize,
    mrcal_calobject_warp_t warp) {
  int Nstate =
      mrcal_num_states(1, 0, rt_ref_frame.size(), 0, 0, Nobservations_board,
                       problem_selections, &lensmodel);

  int Nmeasurements = mrcal_num_measurements(
      Nobservations_board, 0, NULL, 0, calibration_object_width_n,
      calibration_object_height_n, 1, 0, 6, 0, 0, problem_selections,
      &lensmodel);
  int Nmeasurements_boards = mrcal_num_measurements_boards(
      Nobservations_board, calibration_object_width_n,
      calibration_object_height_n);
  int imeas0 = mrcal_measurement_index_boards(0, Nobservations_board, 0,
                                              calibration_object_width_n,
                                              calibration_object_height_n);

  // Allocate buffers for Jt sparse matrix
  int N_j_nonzero = Nstate * Nmeasurements; // Upper bound, actual will be less
  std::vector<int32_t> Jt_p(Nmeasurements + 1);
  std::vector<int32_t> Jt_i(N_j_nonzero);
  std::vector<double_t> Jt_x(N_j_nonzero);

  cholmod_sparse Jt = {.nrow = static_cast<size_t>(Nstate),
                       .ncol = static_cast<size_t>(Nmeasurements),
                       .nzmax = static_cast<size_t>(N_j_nonzero),
                       .p = Jt_p.data(),
                       .i = Jt_i.data(),
                       .nz = nullptr,
                       .x = Jt_x.data(),
                       .z = nullptr,
                       .stype = 0,
                       .itype = CHOLMOD_INT,
                       .xtype = CHOLMOD_REAL,
                       .dtype = CHOLMOD_DOUBLE,
                       .sorted = 1,
                       .packed = 1};

  std::vector<double> b_packed(Nstate);
  std::vector<double> x(Nmeasurements);

  double point_min_range = -1.0, point_max_range = -1.0;
  mrcal_problem_constants_t problem_constants = {
      .point_min_range = point_min_range, .point_max_range = point_max_range};

  int imagersize[2]{imagerSize.width, imagerSize.height};

  bool ret = mrcal_optimizer_callback(
      b_packed.data(), b_packed.size() * sizeof(double), // out
      x.data(), x.size() * sizeof(double),               // out
      &Jt,                                               // Get the Jacobian
      // IN parameters
      intrinsics.data(),
      nullptr, // no extrinsics
      rt_ref_frame.data(),
      nullptr, // no points
      &warp, 1, 0, static_cast<int>(rt_ref_frame.size()), 0, 0,
      observations_board, nullptr, Nobservations_board, 0, nullptr, 0,
      observations_board_pool, nullptr, &lensmodel, imagersize,
      problem_selections, &problem_constants, calibration_object_spacing,
      calibration_object_width_n, calibration_object_height_n, false);

  if (!ret) {
    throw std::runtime_error("mrcal_optimizer_callback failed");
  }

  double observed_pixel_uncertainty =
      _observed_pixel_uncertainty_from_inputs(x, Nmeasurements_boards, imeas0);

  // Convert CHOLMOD sparse Jt to Eigen sparse matrix
  using SpMat = Eigen::SparseMatrix<double, Eigen::ColMajor>;
  SpMat Jt_eigen(Nstate, Nmeasurements);

  {
    std::vector<Eigen::Triplet<double>> triplets;
    triplets.reserve(N_j_nonzero);

    for (int col = 0; col < Nmeasurements; col++) {
      for (int p = Jt_p[col]; p < Jt_p[col + 1]; p++) {
        int row = Jt_i[p];
        double val = Jt_x[p];
        triplets.emplace_back(row, col, val);
      }
    }

    Jt_eigen.setFromTriplets(triplets.begin(), triplets.end());
  }

  // J = Jt^T has shape (Nmeasurements, Nstate)
  SpMat J = Jt_eigen.transpose();

  // Compute JtJ = Jt * J (shape Nstate x Nstate)
  SpMat JtJ = Jt_eigen * J;

  // check positive definite happens for free during compute/info

  // Pre-compute the factorization
  auto solver = std::make_unique<SolverType>();
  solver->compute(JtJ);

  if (solver->info() != Eigen::Success) {
    throw std::runtime_error("Eigen factorization failed");
  }

  // Store the observation rows of J for fast uncertainty propagation
  SpMat J_observations = J.topRows(Nmeasurements_boards);

  return CalibrationUncertaintyContext{
      .solver = std::move(solver),
      .J_observations = std::move(J_observations),
      .observed_pixel_uncertainty = observed_pixel_uncertainty,
      .Nstate = Nstate,
      .Nmeasurements_boards = Nmeasurements_boards};
}

double _propagate_calibration_uncertainty_fast(
    const CalibrationUncertaintyContext &context,
    Eigen::Matrix<double, 2, Eigen::Dynamic, Eigen::RowMajor> dF_dbunpacked,
    mrcal_problem_selections_t &problem_selections,
    mrcal_lensmodel_t &lensmodel, const std::span<double> intrinsics,
    const std::span<mrcal_pose_t> rt_ref_frame, int Nobservations_board) {
  // Pack the gradient
  Eigen::Matrix<double, 2, Eigen::Dynamic, Eigen::RowMajor> dF_dbpacked =
      dF_dbunpacked;

  // called for each Nstate elements of dF_dbunpacked
  for (int i = 0; i < dF_dbunpacked.rows(); i++) {
    size_t offset = i * dF_dbunpacked.cols();
    mrcal_unpack_solver_state_vector(
        dF_dbpacked.data() + offset, 1, 0, rt_ref_frame.size(), 0, 0,
        Nobservations_board, problem_selections, &lensmodel);
  }

  // Solve JtJ * A = dF_dbpacked^T using pre-computed factorization
  Eigen::MatrixXd rhs = dF_dbpacked.transpose();  // (Nstate, 2)
  Eigen::MatrixXd A = context.solver->solve(rhs); // (Nstate, 2)

  if (context.solver->info() != Eigen::Success) {
    throw std::runtime_error("Eigen solve failed");
  }

  // Compute J_obs * A using the stored observation Jacobian
  Eigen::MatrixXd JA = context.J_observations * A; // (Nmeas_obs, 2)

  // Compute Var(F) = JA^T * JA
  Eigen::Matrix2d Var_dF = JA.transpose() * JA;

  return worst_direction_stdev(Var_dF) * context.observed_pixel_uncertainty;
}

double projection_uncertainty_fast(const CalibrationUncertaintyContext &context,
                                   mrcal_point3_t pcam,
                                   mrcal_lensmodel_t lensmodel,
                                   std::span<mrcal_pose_t> rt_ref_frames,
                                   std::span<double> intrinsics) {
  // Prepare inputs
  mrcal_problem_selections_t problem_selections{0};
  problem_selections.do_optimize_intrinsics_core = true;
  problem_selections.do_optimize_intrinsics_distortions = true;
  problem_selections.do_optimize_extrinsics = true;
  problem_selections.do_optimize_frames = true;
  problem_selections.do_optimize_calobject_warp = true;
  problem_selections.do_apply_regularization = true;
  problem_selections.do_apply_outlier_rejection = true;
  problem_selections.do_apply_regularization_unity_cam01 = false;

  int istate_frames0 = mrcal_state_index_frames(0, 1, 0, 6, 0, 0, 6,
                                                problem_selections, &lensmodel);

  int Nobservations_board = rt_ref_frames.size();

  auto dq_db{_dq_db_projection_uncertainty(pcam, lensmodel, rt_ref_frames,
                                           context.Nstate, istate_frames0,
                                           intrinsics)};

  return _propagate_calibration_uncertainty_fast(
      context, dq_db, problem_selections, lensmodel, intrinsics, rt_ref_frames,
      Nobservations_board);
}

std::vector<mrcal_point3_t> compute_uncertainty(
    std::span<mrcal_point3_t> observations_board, std::span<double> intrinsics,
    std::span<mrcal_pose_t> rt_ref_frames, mrcal_calobject_warp_t warp,
    cv::Size imagerSize, cv::Size calobjectSize, double calobjectSpacing,
    cv::Size sampleResolution) {

  mrcal_lensmodel_t lensmodel;
  lensmodel.type = MRCAL_LENSMODEL_OPENCV8;

  // Create calibration uncertainty context once
  mrcal_problem_selections_t problem_selections{0};
  problem_selections.do_optimize_intrinsics_core = true;
  problem_selections.do_optimize_intrinsics_distortions = true;
  problem_selections.do_optimize_extrinsics = true;
  problem_selections.do_optimize_frames = true;
  problem_selections.do_optimize_calobject_warp = true;
  problem_selections.do_apply_regularization = true;
  problem_selections.do_apply_outlier_rejection = true;
  problem_selections.do_apply_regularization_unity_cam01 = false;

  std::vector<mrcal_point3_t> indices_frame_camintrinsics_camextrinsics;
  for (int i = 0; i < rt_ref_frames.size(); i++) {
    indices_frame_camintrinsics_camextrinsics.push_back(
        {.x = static_cast<double>(i), .y = 0, .z = -1});
  }

  std::vector<mrcal_observation_board_t> observations_board_data(
      rt_ref_frames.size());
  auto c_observations_board = observations_board_data.data();

  for (int i_observation = 0; i_observation < rt_ref_frames.size();
       i_observation++) {
    int32_t iframe =
        indices_frame_camintrinsics_camextrinsics.at(i_observation).x;
    int32_t icam_intrinsics =
        indices_frame_camintrinsics_camextrinsics.at(i_observation).y;
    int32_t icam_extrinsics =
        indices_frame_camintrinsics_camextrinsics.at(i_observation).z;

    c_observations_board[i_observation].icam.intrinsics = icam_intrinsics;
    c_observations_board[i_observation].icam.extrinsics = icam_extrinsics;
    c_observations_board[i_observation].iframe = iframe;
  }

  auto context = create_calibration_uncertainty_context(
      problem_selections, lensmodel, intrinsics, rt_ref_frames,
      c_observations_board, observations_board.data(), rt_ref_frames.size(),
      calobjectSize.width, calobjectSize.height, calobjectSpacing, imagerSize,
      warp);

  // generate grid of samples in (u, v) pixels
  auto q = sample_imager(sampleResolution, imagerSize);

  // unproject
  std::vector<mrcal_point3_t> pcam;
  pcam.resize(q.size());
  mrcal_unproject(pcam.data(), q.data(), q.size(), &lensmodel,
                  intrinsics.data());

  // normalize, setting rows with any non-finite elements to zero
  for (auto &p : pcam) {
    double nrm = std::sqrt(p.x * p.x + p.y * p.y + p.z * p.z);
    if (std::isfinite(nrm) && nrm > 0) {
      p.x /= nrm;
      p.y /= nrm;
      p.z /= nrm;
    } else {
      p.x = 0;
      p.y = 0;
      p.z = 0;
    }
  }

  std::vector<mrcal_point3_t> ret;
  ret.reserve(pcam.size());

  // iterate over pcam and q simultaneously - now much faster!
  for (size_t i = 0; i < pcam.size(); i++) {
    auto &pi = pcam[i];
    auto &qi = q[i];

    auto start = std::chrono::high_resolution_clock::now();

    double uncertainty = projection_uncertainty_fast(context, pi, lensmodel,
                                                     rt_ref_frames, intrinsics);

    auto end = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    ret.push_back(mrcal_point3_t{qi.x, qi.y, uncertainty});
  }

  return ret;
}
