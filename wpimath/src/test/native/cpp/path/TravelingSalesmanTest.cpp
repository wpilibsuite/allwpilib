// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <cassert>
#include <span>
#include <vector>

#include <gtest/gtest.h>
#include <wpi/array.h>
#include <wpi/circular_buffer.h>
#include <wpi/math/EigenCore.hpp>
#include <wpi/math/geometry/Pose2d.hpp>
#include <wpi/math/geometry/Rotation2d.hpp>
#include <wpi/math/path/TravelingSalesman.hpp>

/**
 * Returns true if the cycles represented by the two lists match.
 *
 * @param expected The expected cycle.
 * @param actual The actual cycle.
 */
bool IsMatchingCycle(std::span<const wpi::math::Pose2d> expected,
                     std::span<const wpi::math::Pose2d> actual) {
  assert(expected.size() == actual.size());

  // Check actual has expected cycle (forward)
  wpi::circular_buffer<wpi::math::Pose2d> actualBufferForward{expected.size()};
  for (size_t i = 0; i < actual.size(); ++i) {
    actualBufferForward.push_back(actual[i % actual.size()]);
  }
  bool matchesExpectedForward = true;
  for (size_t i = 0; i < expected.size(); ++i) {
    matchesExpectedForward &= (expected[i] == actualBufferForward[i]);
  }

  // Check actual has expected cycle (reverse)
  wpi::circular_buffer<wpi::math::Pose2d> actualBufferReverse{expected.size()};
  for (size_t i = 0; i < actual.size(); ++i) {
    actualBufferReverse.push_front(actual[(1 + i) % actual.size()]);
  }
  bool matchesExpectedReverse = true;
  for (size_t i = 0; i < expected.size(); ++i) {
    matchesExpectedReverse &= (expected[i] == actualBufferReverse[i]);
  }

  // Actual may be reversed from expected, but that's still valid
  return matchesExpectedForward || matchesExpectedReverse;
}

TEST(TravelingSalesmanTest, FiveLengthStaticPathWithDistanceCost) {
  // ...................
  // ........2..........
  // ..0..........4.....
  // ...................
  // ....3.....1........
  // ...................
  wpi::array<wpi::math::Pose2d, 5> poses{
      wpi::math::Pose2d{3_m, 3_m, 0_rad}, wpi::math::Pose2d{11_m, 5_m, 0_rad},
      wpi::math::Pose2d{9_m, 2_m, 0_rad}, wpi::math::Pose2d{5_m, 5_m, 0_rad},
      wpi::math::Pose2d{14_m, 3_m, 0_rad}};

  wpi::math::TravelingSalesman traveler;
  wpi::array<wpi::math::Pose2d, 5> solution = traveler.Solve(poses, 500);

  wpi::array<wpi::math::Pose2d, 5> expected{poses[0], poses[2], poses[4],
                                            poses[1], poses[3]};

  EXPECT_TRUE(IsMatchingCycle(expected, solution));
}

TEST(TravelingSalesmanTest, FiveLengthDynamicPathWithDistanceCost) {
  // ...................
  // ........2..........
  // ..0..........4.....
  // ...................
  // ....3.....1........
  // ...................
  wpi::array<wpi::math::Pose2d, 5> poses{
      wpi::math::Pose2d{3_m, 3_m, 0_rad}, wpi::math::Pose2d{11_m, 5_m, 0_rad},
      wpi::math::Pose2d{9_m, 2_m, 0_rad}, wpi::math::Pose2d{5_m, 5_m, 0_rad},
      wpi::math::Pose2d{14_m, 3_m, 0_rad}};

  wpi::math::TravelingSalesman traveler;
  std::vector<wpi::math::Pose2d> solution =
      traveler.Solve(std::span<const wpi::math::Pose2d>{poses}, 500);

  ASSERT_EQ(5u, solution.size());
  wpi::array<wpi::math::Pose2d, 5> expected{poses[0], poses[2], poses[4],
                                            poses[1], poses[3]};

  EXPECT_TRUE(IsMatchingCycle(expected, solution));
}

TEST(TravelingSalesmanTest, TenLengthStaticPathWithDistanceCost) {
  // ....6.3..1.2.......
  // ..4................
  // .............9.....
  // .0.................
  // .....7..5...8......
  // ...................
  wpi::array<wpi::math::Pose2d, 10> poses{
      wpi::math::Pose2d{2_m, 4_m, 0_rad},  wpi::math::Pose2d{10_m, 1_m, 0_rad},
      wpi::math::Pose2d{12_m, 1_m, 0_rad}, wpi::math::Pose2d{7_m, 1_m, 0_rad},
      wpi::math::Pose2d{3_m, 2_m, 0_rad},  wpi::math::Pose2d{9_m, 5_m, 0_rad},
      wpi::math::Pose2d{5_m, 1_m, 0_rad},  wpi::math::Pose2d{6_m, 5_m, 0_rad},
      wpi::math::Pose2d{13_m, 5_m, 0_rad}, wpi::math::Pose2d{14_m, 3_m, 0_rad}};

  wpi::math::TravelingSalesman traveler;
  wpi::array<wpi::math::Pose2d, 10> solution = traveler.Solve(poses, 500);

  wpi::array<wpi::math::Pose2d, 10> expected{
      poses[0], poses[4], poses[6], poses[3], poses[1],
      poses[2], poses[9], poses[8], poses[5], poses[7]};

  EXPECT_TRUE(IsMatchingCycle(expected, solution));
}

TEST(TravelingSalesmanTest, TenLengthDynamicPathWithDistanceCost) {
  // ....6.3..1.2.......
  // ..4................
  // .............9.....
  // .0.................
  // .....7..5...8......
  // ...................
  wpi::array<wpi::math::Pose2d, 10> poses{
      wpi::math::Pose2d{2_m, 4_m, 0_rad},  wpi::math::Pose2d{10_m, 1_m, 0_rad},
      wpi::math::Pose2d{12_m, 1_m, 0_rad}, wpi::math::Pose2d{7_m, 1_m, 0_rad},
      wpi::math::Pose2d{3_m, 2_m, 0_rad},  wpi::math::Pose2d{9_m, 5_m, 0_rad},
      wpi::math::Pose2d{5_m, 1_m, 0_rad},  wpi::math::Pose2d{6_m, 5_m, 0_rad},
      wpi::math::Pose2d{13_m, 5_m, 0_rad}, wpi::math::Pose2d{14_m, 3_m, 0_rad}};

  wpi::math::TravelingSalesman traveler;
  std::vector<wpi::math::Pose2d> solution =
      traveler.Solve(std::span<const wpi::math::Pose2d>{poses}, 500);

  ASSERT_EQ(10u, solution.size());
  wpi::array<wpi::math::Pose2d, 10> expected{
      poses[0], poses[4], poses[6], poses[3], poses[1],
      poses[2], poses[9], poses[8], poses[5], poses[7]};

  EXPECT_TRUE(IsMatchingCycle(expected, solution));
}
