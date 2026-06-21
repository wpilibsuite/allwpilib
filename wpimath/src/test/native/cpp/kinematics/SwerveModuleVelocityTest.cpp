// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/kinematics/SwerveModuleVelocity.hpp"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "wpi/math/geometry/Rotation2d.hpp"

static constexpr double kEpsilon = 1E-9;

TEST_CASE("SwerveModuleVelocityTest Optimize", "[wpimath]") {
  wpi::math::Rotation2d angleA{45_deg};
  wpi::math::SwerveModuleVelocity refA{-2_mps, 180_deg};
  auto optimizedA = refA.Optimize(angleA);

  CHECK(optimizedA.velocity.value() == Catch::Approx(2.0).margin(kEpsilon));
  CHECK(optimizedA.angle.Degrees().value() ==
        Catch::Approx(0.0).margin(kEpsilon));

  wpi::math::Rotation2d angleB{-50_deg};
  wpi::math::SwerveModuleVelocity refB{4.7_mps, 41_deg};
  auto optimizedB = refB.Optimize(angleB);

  CHECK(optimizedB.velocity.value() == Catch::Approx(-4.7).margin(kEpsilon));
  CHECK(optimizedB.angle.Degrees().value() ==
        Catch::Approx(-139.0).margin(kEpsilon));
}

TEST_CASE("SwerveModuleVelocityTest NoOptimize", "[wpimath]") {
  wpi::math::Rotation2d angleA{0_deg};
  wpi::math::SwerveModuleVelocity refA{2_mps, 89_deg};
  auto optimizedA = refA.Optimize(angleA);

  CHECK(optimizedA.velocity.value() == Catch::Approx(2.0).margin(kEpsilon));
  CHECK(optimizedA.angle.Degrees().value() ==
        Catch::Approx(89.0).margin(kEpsilon));

  wpi::math::Rotation2d angleB{0_deg};
  wpi::math::SwerveModuleVelocity refB{-2_mps, -2_deg};
  auto optimizedB = refB.Optimize(angleB);

  CHECK(optimizedB.velocity.value() == Catch::Approx(-2.0).margin(kEpsilon));
  CHECK(optimizedB.angle.Degrees().value() ==
        Catch::Approx(-2.0).margin(kEpsilon));
}

TEST_CASE("SwerveModuleVelocityTest CosineScaling", "[wpimath]") {
  wpi::math::Rotation2d angleA{0_deg};
  wpi::math::SwerveModuleVelocity refA{2_mps, 45_deg};
  auto optimizedA = refA.CosineScale(angleA);

  CHECK(optimizedA.velocity.value() ==
        Catch::Approx(std::sqrt(2.0)).margin(kEpsilon));
  CHECK(optimizedA.angle.Degrees().value() ==
        Catch::Approx(45.0).margin(kEpsilon));

  wpi::math::Rotation2d angleB{45_deg};
  wpi::math::SwerveModuleVelocity refB{2_mps, 45_deg};
  auto optimizedB = refB.CosineScale(angleB);

  CHECK(optimizedB.velocity.value() == Catch::Approx(2.0).margin(kEpsilon));
  CHECK(optimizedB.angle.Degrees().value() ==
        Catch::Approx(45.0).margin(kEpsilon));

  wpi::math::Rotation2d angleC{-45_deg};
  wpi::math::SwerveModuleVelocity refC{2_mps, 45_deg};
  auto optimizedC = refC.CosineScale(angleC);

  CHECK(optimizedC.velocity.value() == Catch::Approx(0.0).margin(kEpsilon));
  CHECK(optimizedC.angle.Degrees().value() ==
        Catch::Approx(45.0).margin(kEpsilon));

  wpi::math::Rotation2d angleD{135_deg};
  wpi::math::SwerveModuleVelocity refD{2_mps, 45_deg};
  auto optimizedD = refD.CosineScale(angleD);

  CHECK(optimizedD.velocity.value() == Catch::Approx(0.0).margin(kEpsilon));
  CHECK(optimizedD.angle.Degrees().value() ==
        Catch::Approx(45.0).margin(kEpsilon));

  wpi::math::Rotation2d angleE{-135_deg};
  wpi::math::SwerveModuleVelocity refE{2_mps, 45_deg};
  auto optimizedE = refE.CosineScale(angleE);

  CHECK(optimizedE.velocity.value() == Catch::Approx(-2.0).margin(kEpsilon));
  CHECK(optimizedE.angle.Degrees().value() ==
        Catch::Approx(45.0).margin(kEpsilon));

  wpi::math::Rotation2d angleF{180_deg};
  wpi::math::SwerveModuleVelocity refF{2_mps, 45_deg};
  auto optimizedF = refF.CosineScale(angleF);

  CHECK(optimizedF.velocity.value() ==
        Catch::Approx(-std::sqrt(2.0)).margin(kEpsilon));
  CHECK(optimizedF.angle.Degrees().value() ==
        Catch::Approx(45.0).margin(kEpsilon));

  wpi::math::Rotation2d angleG{0_deg};
  wpi::math::SwerveModuleVelocity refG{-2_mps, 45_deg};
  auto optimizedG = refG.CosineScale(angleG);

  CHECK(optimizedG.velocity.value() ==
        Catch::Approx(-std::sqrt(2.0)).margin(kEpsilon));
  CHECK(optimizedG.angle.Degrees().value() ==
        Catch::Approx(45.0).margin(kEpsilon));

  wpi::math::Rotation2d angleH{45_deg};
  wpi::math::SwerveModuleVelocity refH{-2_mps, 45_deg};
  auto optimizedH = refH.CosineScale(angleH);

  CHECK(optimizedH.velocity.value() == Catch::Approx(-2.0).margin(kEpsilon));
  CHECK(optimizedH.angle.Degrees().value() ==
        Catch::Approx(45.0).margin(kEpsilon));

  wpi::math::Rotation2d angleI{-45_deg};
  wpi::math::SwerveModuleVelocity refI{-2_mps, 45_deg};
  auto optimizedI = refI.CosineScale(angleI);

  CHECK(optimizedI.velocity.value() == Catch::Approx(0.0).margin(kEpsilon));
  CHECK(optimizedI.angle.Degrees().value() ==
        Catch::Approx(45.0).margin(kEpsilon));

  wpi::math::Rotation2d angleJ{135_deg};
  wpi::math::SwerveModuleVelocity refJ{-2_mps, 45_deg};
  auto optimizedJ = refJ.CosineScale(angleJ);

  CHECK(optimizedJ.velocity.value() == Catch::Approx(0.0).margin(kEpsilon));
  CHECK(optimizedJ.angle.Degrees().value() ==
        Catch::Approx(45.0).margin(kEpsilon));

  wpi::math::Rotation2d angleK{-135_deg};
  wpi::math::SwerveModuleVelocity refK{-2_mps, 45_deg};
  auto optimizedK = refK.CosineScale(angleK);

  CHECK(optimizedK.velocity.value() == Catch::Approx(2.0).margin(kEpsilon));
  CHECK(optimizedK.angle.Degrees().value() ==
        Catch::Approx(45.0).margin(kEpsilon));

  wpi::math::Rotation2d angleL{180_deg};
  wpi::math::SwerveModuleVelocity refL{-2_mps, 45_deg};
  auto optimizedL = refL.CosineScale(angleL);

  CHECK(optimizedL.velocity.value() ==
        Catch::Approx(std::sqrt(2.0)).margin(kEpsilon));
  CHECK(optimizedL.angle.Degrees().value() ==
        Catch::Approx(45.0).margin(kEpsilon));
}

TEST_CASE("SwerveModuleVelocityTest Equality", "[wpimath]") {
  wpi::math::SwerveModuleVelocity state1{2_mps, 90_deg};
  wpi::math::SwerveModuleVelocity state2{2_mps, 90_deg};

  CHECK(state1 == state2);
}

TEST_CASE("SwerveModuleVelocityTest Inequality", "[wpimath]") {
  wpi::math::SwerveModuleVelocity state1{1_mps, 90_deg};
  wpi::math::SwerveModuleVelocity state2{2_mps, 90_deg};
  wpi::math::SwerveModuleVelocity state3{1_mps, 89_deg};

  CHECK(state1 != state2);
  CHECK(state1 != state3);
}
