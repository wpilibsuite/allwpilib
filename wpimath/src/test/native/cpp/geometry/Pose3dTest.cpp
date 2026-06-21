// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/geometry/Pose3d.hpp"

#include <cmath>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "wpi/util/array.hpp"

using namespace wpi::math;

TEST_CASE("Pose3dTest RotateBy", "[wpimath]") {
  constexpr auto x = 1_m;
  constexpr auto y = 2_m;
  const Pose3d initial{x, y, 0_m, Rotation3d{0_deg, 0_deg, 45_deg}};

  constexpr wpi::units::radian_t yaw = 5_deg;
  const Rotation3d rotation{0_deg, 0_deg, yaw};
  const auto rotated = initial.RotateBy(rotation);

  // Translation is rotated by CCW rotation matrix
  double c = std::cos(yaw.value());
  double s = std::sin(yaw.value());
  CHECK(c * x.value() - s * y.value() == Catch::Approx(rotated.X().value()));
  CHECK(s * x.value() + c * y.value() == Catch::Approx(rotated.Y().value()));
  CHECK(0.0 == Catch::Approx(rotated.Z().value()));
  CHECK(0.0 == Catch::Approx(rotated.Rotation().X().value()));
  CHECK(0.0 == Catch::Approx(rotated.Rotation().Y().value()));
  CHECK(initial.Rotation().Z().value() + rotation.Z().value() ==
        Catch::Approx(rotated.Rotation().Z().value()));
}

TEST_CASE("Pose3dTest TestTransformByRotations", "[wpimath]") {
  constexpr double kEpsilon = 1E-9;

  const Pose3d initialPose{0_m, 0_m, 0_m, Rotation3d{0_deg, 0_deg, 0_deg}};
  const Transform3d transform1{Translation3d{0_m, 0_m, 0_m},
                               Rotation3d{90_deg, 45_deg, 0_deg}};
  const Transform3d transform2{Translation3d{0_m, 0_m, 0_m},
                               Rotation3d{-90_deg, 0_deg, 0_deg}};
  const Transform3d transform3{Translation3d{0_m, 0_m, 0_m},
                               Rotation3d{0_deg, -45_deg, 0_deg}};

  Pose3d finalPose = initialPose.TransformBy(transform1)
                         .TransformBy(transform2)
                         .TransformBy(transform3);

  CHECK(finalPose.Rotation().X().value() ==
        Catch::Approx(initialPose.Rotation().X().value()).margin(kEpsilon));
  CHECK(finalPose.Rotation().Y().value() ==
        Catch::Approx(initialPose.Rotation().Y().value()).margin(kEpsilon));
  CHECK(finalPose.Rotation().Z().value() ==
        Catch::Approx(initialPose.Rotation().Z().value()).margin(kEpsilon));
}

TEST_CASE("Pose3dTest TransformBy", "[wpimath]") {
  Eigen::Vector3d zAxis{0.0, 0.0, 1.0};

  const Pose3d initial{1_m, 2_m, 0_m, Rotation3d{zAxis, 45.0_deg}};
  const Transform3d transform{Translation3d{5_m, 0_m, 0_m},
                              Rotation3d{zAxis, 5_deg}};

  const auto transformed = initial + transform;

  CHECK(1.0 + 5.0 / std::sqrt(2.0) == Catch::Approx(transformed.X().value()));
  CHECK(2.0 + 5.0 / std::sqrt(2.0) == Catch::Approx(transformed.Y().value()));
  CHECK(transformed.Rotation().Z().value() ==
        Catch::Approx(wpi::units::radian_t{50_deg}.value()));
}

TEST_CASE("Pose3dTest RelativeTo", "[wpimath]") {
  Eigen::Vector3d zAxis{0.0, 0.0, 1.0};

  const Pose3d initial{0_m, 0_m, 0_m, Rotation3d{zAxis, 45_deg}};
  const Pose3d final{5_m, 5_m, 0_m, Rotation3d{zAxis, 45_deg}};

  const auto finalRelativeToInitial = final.RelativeTo(initial);

  CHECK(5.0 * std::sqrt(2.0) ==
        Catch::Approx(finalRelativeToInitial.X().value()));
  CHECK(0.0 == Catch::Approx(finalRelativeToInitial.Y().value()));
  CHECK(0.0 == Catch::Approx(finalRelativeToInitial.Rotation().Z().value())
                   .margin(1e-9));
}

TEST_CASE("Pose3dTest RotateAround", "[wpimath]") {
  const Pose3d initial{5_m, 0_m, 0_m, Rotation3d{}};
  const Translation3d point{0_m, 0_m, 0_m};

  const auto rotated =
      initial.RotateAround(point, Rotation3d{0_deg, 0_deg, 180_deg});

  CHECK(-5.0 == Catch::Approx(rotated.X().value()).margin(1e-9));
  CHECK(0.0 == Catch::Approx(rotated.Y().value()).margin(1e-9));
  CHECK(wpi::units::radian_t{180_deg}.value() ==
        Catch::Approx(rotated.Rotation().Z().value()).margin(1e-9));
}

TEST_CASE("Pose3dTest Equality", "[wpimath]") {
  Eigen::Vector3d zAxis{0.0, 0.0, 1.0};

  const Pose3d a{0_m, 5_m, 0_m, Rotation3d{zAxis, 43_deg}};
  const Pose3d b{0_m, 5_m, 0_m, Rotation3d{zAxis, 43_deg}};
  CHECK(a == b);
}

TEST_CASE("Pose3dTest Inequality", "[wpimath]") {
  Eigen::Vector3d zAxis{0.0, 0.0, 1.0};

  const Pose3d a{0_m, 5_m, 0_m, Rotation3d{zAxis, 43_deg}};
  const Pose3d b{0_m, 5_ft, 0_m, Rotation3d{zAxis, 43_deg}};
  CHECK(a != b);
}

TEST_CASE("Pose3dTest Minus", "[wpimath]") {
  Eigen::Vector3d zAxis{0.0, 0.0, 1.0};

  const Pose3d initial{0_m, 0_m, 0_m, Rotation3d{zAxis, 45_deg}};
  const Pose3d final{5_m, 5_m, 0_m, Rotation3d{zAxis, 45_deg}};

  const auto transform = final - initial;

  CHECK(5.0 * std::sqrt(2.0) == Catch::Approx(transform.X().value()));
  CHECK(0.0 == Catch::Approx(transform.Y().value()));
  CHECK(0.0 == Catch::Approx(transform.Rotation().Z().value()).margin(1e-9));
}

TEST_CASE("Pose3dTest ToMatrix", "[wpimath]") {
  Pose3d before{1_m, 2_m, 3_m, Rotation3d{10_deg, 20_deg, 30_deg}};
  Pose3d after{before.ToMatrix()};

  CHECK(before == after);
}

TEST_CASE("Pose3dTest ToPose2d", "[wpimath]") {
  Pose3d pose{1_m, 2_m, 3_m, Rotation3d{20_deg, 30_deg, 40_deg}};
  Pose2d expected{1_m, 2_m, 40_deg};

  CHECK(expected == pose.ToPose2d());
}

TEST_CASE("Pose3dTest ComplexTwists", "[wpimath]") {
  wpi::util::array<Pose3d, 5> initial_poses{
      Pose3d{0.698303_m, -0.959096_m, 0.271076_m,
             Rotation3d{Quaternion{0.86403, -0.076866, 0.147234, 0.475254}}},
      Pose3d{0.634892_m, -0.765209_m, 0.117543_m,
             Rotation3d{Quaternion{0.84987, -0.070829, 0.162097, 0.496415}}},
      Pose3d{0.584827_m, -0.590303_m, -0.02557_m,
             Rotation3d{Quaternion{0.832743, -0.041991, 0.202188, 0.513708}}},
      Pose3d{0.505038_m, -0.451479_m, -0.112835_m,
             Rotation3d{Quaternion{0.816515, -0.002673, 0.226182, 0.531166}}},
      Pose3d{0.428178_m, -0.329692_m, -0.189707_m,
             Rotation3d{Quaternion{0.807886, 0.029298, 0.257788, 0.529157}}},
  };

  wpi::util::array<Pose3d, 5> final_poses{
      Pose3d{-0.230448_m, -0.511957_m, 0.198406_m,
             Rotation3d{Quaternion{0.753984, 0.347016, 0.409105, 0.379106}}},
      Pose3d{-0.088932_m, -0.343253_m, 0.095018_m,
             Rotation3d{Quaternion{0.638738, 0.413016, 0.536281, 0.365833}}},
      Pose3d{-0.107908_m, -0.317552_m, 0.133946_m,
             Rotation3d{Quaternion{0.653444, 0.417069, 0.465505, 0.427046}}},
      Pose3d{-0.123383_m, -0.156411_m, -0.047435_m,
             Rotation3d{Quaternion{0.652983, 0.40644, 0.431566, 0.47135}}},
      Pose3d{-0.084654_m, -0.019305_m, -0.030022_m,
             Rotation3d{Quaternion{0.620243, 0.429104, 0.479384, 0.44873}}},
  };

  for (size_t i = 0; i < initial_poses.size(); i++) {
    auto start = initial_poses[i];
    auto end = final_poses[i];

    auto twist = (end - start).Log();
    auto start_exp = start + twist.Exp();

    auto eps = 1E-5;

    CHECK(start_exp.X().value() == Catch::Approx(end.X().value()).margin(eps));
    CHECK(start_exp.Y().value() == Catch::Approx(end.Y().value()).margin(eps));
    CHECK(start_exp.Z().value() == Catch::Approx(end.Z().value()).margin(eps));
    CHECK(start_exp.Rotation().GetQuaternion().W() ==
          Catch::Approx(end.Rotation().GetQuaternion().W()).margin(eps));
    CHECK(start_exp.Rotation().GetQuaternion().X() ==
          Catch::Approx(end.Rotation().GetQuaternion().X()).margin(eps));
    CHECK(start_exp.Rotation().GetQuaternion().Y() ==
          Catch::Approx(end.Rotation().GetQuaternion().Y()).margin(eps));
    CHECK(start_exp.Rotation().GetQuaternion().Z() ==
          Catch::Approx(end.Rotation().GetQuaternion().Z()).margin(eps));
  }
}

TEST_CASE("Pose3dTest TwistNaN", "[wpimath]") {
  wpi::util::array<Pose3d, 2> initial_poses{
      Pose3d{6.32_m, 4.12_m, 0.00_m,
             Rotation3d{Quaternion{-0.9999999999999999, 0.0, 0.0,
                                   1.9208309264993548E-8}}},
      Pose3d{3.75_m, 2.95_m, 0.00_m,
             Rotation3d{Quaternion{0.9999999999999793, 0.0, 0.0,
                                   2.0352360299846772E-7}}},
  };

  wpi::util::array<Pose3d, 2> final_poses{
      Pose3d{6.33_m, 4.15_m, 0.00_m,
             Rotation3d{Quaternion{-0.9999999999999999, 0.0, 0.0,
                                   2.416890209039172E-8}}},
      Pose3d{3.66_m, 2.93_m, 0.00_m,
             Rotation3d{Quaternion{0.9999999999999782, 0.0, 0.0,
                                   2.0859477994905617E-7}}},
  };

  for (size_t i = 0; i < initial_poses.size(); i++) {
    auto start = initial_poses[i];
    auto end = final_poses[i];
    auto twist = (end - start).Log();

    CHECK_FALSE(std::isnan(twist.dx.value()));
    CHECK_FALSE(std::isnan(twist.dy.value()));
    CHECK_FALSE(std::isnan(twist.dz.value()));
    CHECK_FALSE(std::isnan(twist.rx.value()));
    CHECK_FALSE(std::isnan(twist.ry.value()));
    CHECK_FALSE(std::isnan(twist.rz.value()));
  }
}

TEST_CASE("Pose3dTest Nearest", "[wpimath]") {
  const Pose3d origin{0_m, 0_m, 0_m, Rotation3d{}};

  // Distance sort
  // poses are in order of closest to farthest away from the origin at
  // various positions in 3D space.
  const Pose3d pose1{1_m, 0_m, 0_m, Rotation3d{}};
  const Pose3d pose2{0_m, 2_m, 0_m, Rotation3d{}};
  const Pose3d pose3{0_m, 0_m, 3_m, Rotation3d{}};
  const Pose3d pose4{2_m, 2_m, 2_m, Rotation3d{}};
  const Pose3d pose5{3_m, 3_m, 3_m, Rotation3d{}};

  CHECK(pose3.X().value() ==
        Catch::Approx(origin.Nearest({pose5, pose3, pose4}).X().value()));
  CHECK(pose3.Y().value() ==
        Catch::Approx(origin.Nearest({pose5, pose3, pose4}).Y().value()));
  CHECK(pose3.Z().value() ==
        Catch::Approx(origin.Nearest({pose5, pose3, pose4}).Z().value()));

  CHECK(pose1.X().value() ==
        Catch::Approx(origin.Nearest({pose1, pose2, pose3}).X().value()));
  CHECK(pose1.Y().value() ==
        Catch::Approx(origin.Nearest({pose1, pose2, pose3}).Y().value()));
  CHECK(pose1.Z().value() ==
        Catch::Approx(origin.Nearest({pose1, pose2, pose3}).Z().value()));

  CHECK(pose2.X().value() ==
        Catch::Approx(origin.Nearest({pose4, pose2, pose3}).X().value()));
  CHECK(pose2.Y().value() ==
        Catch::Approx(origin.Nearest({pose4, pose2, pose3}).Y().value()));
  CHECK(pose2.Z().value() ==
        Catch::Approx(origin.Nearest({pose4, pose2, pose3}).Z().value()));

  // Rotation component sort (when distance is the same)
  // Use the same translation to avoid distance differences
  const Translation3d translation{1_m, 0_m, 0_m};

  const Pose3d poseA{translation, Rotation3d{}};  // No rotation
  const Pose3d poseB{translation, Rotation3d{30_deg, 0_deg, 0_deg}};
  const Pose3d poseC{translation, Rotation3d{0_deg, 45_deg, 0_deg}};
  const Pose3d poseD{translation, Rotation3d{0_deg, 0_deg, 90_deg}};
  const Pose3d poseE{translation, Rotation3d{180_deg, 0_deg, 0_deg}};

  auto result1 =
      Pose3d{0_m, 0_m, 0_m, Rotation3d{}}.Nearest({poseA, poseB, poseD});
  CHECK(poseA.Rotation().X().value() ==
        Catch::Approx(result1.Rotation().X().value()));
  CHECK(poseA.Rotation().Y().value() ==
        Catch::Approx(result1.Rotation().Y().value()));
  CHECK(poseA.Rotation().Z().value() ==
        Catch::Approx(result1.Rotation().Z().value()));

  auto result2 =
      Pose3d{0_m, 0_m, 0_m, Rotation3d{25_deg, 0_deg, 0_deg}}.Nearest(
          {poseB, poseC, poseD});
  CHECK(poseB.Rotation().X().value() ==
        Catch::Approx(result2.Rotation().X().value()));
  CHECK(poseB.Rotation().Y().value() ==
        Catch::Approx(result2.Rotation().Y().value()));
  CHECK(poseB.Rotation().Z().value() ==
        Catch::Approx(result2.Rotation().Z().value()));

  auto result3 =
      Pose3d{0_m, 0_m, 0_m, Rotation3d{0_deg, 50_deg, 0_deg}}.Nearest(
          {poseB, poseC, poseD});
  CHECK(poseC.Rotation().X().value() ==
        Catch::Approx(result3.Rotation().X().value()));
  CHECK(poseC.Rotation().Y().value() ==
        Catch::Approx(result3.Rotation().Y().value()));
  CHECK(poseC.Rotation().Z().value() ==
        Catch::Approx(result3.Rotation().Z().value()));

  auto result4 =
      Pose3d{0_m, 0_m, 0_m, Rotation3d{0_deg, 0_deg, 85_deg}}.Nearest(
          {poseA, poseC, poseD});
  CHECK(poseD.Rotation().X().value() ==
        Catch::Approx(result4.Rotation().X().value()));
  CHECK(poseD.Rotation().Y().value() ==
        Catch::Approx(result4.Rotation().Y().value()));
  CHECK(poseD.Rotation().Z().value() ==
        Catch::Approx(result4.Rotation().Z().value()));

  auto result5 =
      Pose3d{0_m, 0_m, 0_m, Rotation3d{170_deg, 0_deg, 0_deg}}.Nearest(
          {poseA, poseD, poseE});
  CHECK(poseE.Rotation().X().value() ==
        Catch::Approx(result5.Rotation().X().value()));
  CHECK(poseE.Rotation().Y().value() ==
        Catch::Approx(result5.Rotation().Y().value()));
  CHECK(poseE.Rotation().Z().value() ==
        Catch::Approx(result5.Rotation().Z().value()));

  // Test with complex 3D rotations (combining roll, pitch, yaw)
  const Pose3d complexPose1{translation, Rotation3d{45_deg, 30_deg, 60_deg}};
  const Pose3d complexPose2{translation, Rotation3d{90_deg, 45_deg, 90_deg}};
  const Pose3d complexPose3{translation, Rotation3d{10_deg, 15_deg, 20_deg}};

  auto complexResult =
      Pose3d{0_m, 0_m, 0_m, Rotation3d{5_deg, 10_deg, 15_deg}}.Nearest(
          {complexPose1, complexPose2, complexPose3});
  CHECK(complexPose3.Rotation().X().value() ==
        Catch::Approx(complexResult.Rotation().X().value()));
  CHECK(complexPose3.Rotation().Y().value() ==
        Catch::Approx(complexResult.Rotation().Y().value()));
  CHECK(complexPose3.Rotation().Z().value() ==
        Catch::Approx(complexResult.Rotation().Z().value()));
}
