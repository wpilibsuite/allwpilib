// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <vector>

#include <catch2/catch_test_macros.hpp>
#include <dogleg.h>
#include <mrcal_wrapper.h>

namespace {

void Evaluate(const double* state, double* residuals, cholmod_sparse* jacobian,
              void*) {
  auto* columnStarts = static_cast<int*>(jacobian->p);
  auto* rowIndices = static_cast<int*>(jacobian->i);
  auto* values = static_cast<double*>(jacobian->x);

  residuals[0] = state[0] * state[0] - 4.0;
  columnStarts[0] = 0;
  columnStarts[1] = 1;
  rowIndices[0] = 0;
  values[0] = 2.0 * state[0];
}

struct CancellationState {
  int polls = 0;
};

bool CancelAfterSolverStarts(void* cookie) {
  auto* state = static_cast<CancellationState*>(cookie);
  return ++state->polls == 4;
}

bool AlwaysCancel(void*) {
  return true;
}

}  // namespace

TEST_CASE("Libdogleg optimization can be canceled", "[wpical]") {
  double state = 10.0;
  dogleg_parameters2_t parameters;
  dogleg_getDefaultParameters(&parameters);

  CancellationState cancellationState;
  bool canceled = false;
  double result = dogleg_optimize2(&state, 1, 1, 1, Evaluate, nullptr,
                                   &parameters, CancelAfterSolverStarts,
                                   &cancellationState, &canceled, nullptr);

  CHECK(result >= 0.0);
  CHECK(canceled);
  CHECK(cancellationState.polls == 4);
}

TEST_CASE("Mrcal optimization can be canceled", "[wpical]") {
  bool canceled = false;
  auto stats = mrcal_optimize(
      nullptr, -1, nullptr, -1, nullptr, nullptr, nullptr, nullptr, nullptr, 0,
      0, 0, 0, 0, nullptr, nullptr, 0, 0, nullptr, 0, nullptr, nullptr, nullptr,
      nullptr, mrcal_problem_selections_t{}, nullptr, 0.0, 0, 0, false, false,
      AlwaysCancel, nullptr, &canceled);

  CHECK(canceled);
  CHECK(stats.rms_reproj_error__pixels < 0.0);
}

TEST_CASE("Mrcal calibration can be canceled before it starts", "[wpical]") {
  mrcal_cancellation_source stopSource;
  stopSource.request_stop();
  std::vector<mrcal_point3_t> observations;
  std::vector<mrcal_pose_t> frames;

  auto result = mrcal_main(observations, frames, cv::Size{10, 10}, 0.03,
                           cv::Size{1280, 720}, 1200, stopSource.get_token());

  CHECK(result == nullptr);
}
