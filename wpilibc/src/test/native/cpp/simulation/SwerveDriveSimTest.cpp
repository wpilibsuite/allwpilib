/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/simulation/SwerveDriveSim.h"
#include "frc/system/plant/LinearSystemId.h"
#include "gtest/gtest.h"

frc::sim::SwerveDriveSim<4> sim{
    50_kg,
    frc::sim::SimSwerveModule(
        frc::DCMotor::NEO(1), 8.0, 2_in, frc::Translation2d(1_m, 1_m),
        frc::LinearSystemId::SingleJointedArmSystem(
            frc::DCMotor::NEO(1), units::kilogram_square_meter_t(0.01), 10.0)),
    frc::sim::SimSwerveModule(
        frc::DCMotor::NEO(1), 8.0, 2_in, frc::Translation2d(1_m, -1_m),
        frc::LinearSystemId::SingleJointedArmSystem(
            frc::DCMotor::NEO(1), units::kilogram_square_meter_t(0.01), 10.0)),
    frc::sim::SimSwerveModule(
        frc::DCMotor::NEO(1), 8.0, 2_in, frc::Translation2d(-1_m, 1_m),
        frc::LinearSystemId::SingleJointedArmSystem(
            frc::DCMotor::NEO(1), units::kilogram_square_meter_t(0.01), 10.0)),
    frc::sim::SimSwerveModule(
        frc::DCMotor::NEO(1), 8.0, 2_in, frc::Translation2d(-1_m, -1_m),
        frc::LinearSystemId::SingleJointedArmSystem(
            frc::DCMotor::NEO(1), units::kilogram_square_meter_t(0.01), 10.0))};

TEST(SwerveDriveSim, ForceCalculation) {
  const auto& module = sim.GetModules()[0];
  units::newton_t force = module.EstimateModuleForce(0_mps, units::volt_t(12));

  // F = a when mass = 1kg
  // xdot = [vel, accel] = 0x + B u
  auto system =
      frc::LinearSystemId::ElevatorSystem(frc::DCMotor::NEO(1), 1_kg, 2_in, 8);
  auto refForce = system.B() * frc::MakeMatrix<1, 1>(12.0);

  EXPECT_NEAR(refForce(1, 0), force.to<double>(), 0.02);
}

TEST(SwerveDriveSim, SimulationDrivingForward) {
  auto desiredSpeeds =
      sim.GetKinematics().ToSwerveModuleStates({1.0_mps, 2_mps, 0.5_rad_per_s});

  // Seems like ~3.25 volts per meter per second.
  for (size_t i = 0; i < 200; ++i) {
    sim.SetModuleDriveVoltages(7.5_V, 7.5_V, 7.5_V, 7.5_V);

    // Simple P loop on heading error.
    std::array<units::volt_t, 4> moduleVoltages;
    for (size_t m = 0; m < 4; ++m) {
      const auto& module = sim.GetModules()[m];
      const auto& ref = desiredSpeeds[m].angle;
      const auto& angle = module.GetAzimuthAngle();
      auto delta = ref - angle;
      moduleVoltages[m] = units::volt_t(delta.Radians().to<double>() * 10.0);
    }
    sim.SetModuleAzimuthVoltages(moduleVoltages);
    sim.Update(20_ms);
  }

  EXPECT_NEAR(1.0, sim.GetEstimatedSpeed().vx.to<double>(), 0.1);
  EXPECT_NEAR(2.0, sim.GetEstimatedSpeed().vy.to<double>(), 0.1);
  EXPECT_NEAR(0.5, sim.GetEstimatedSpeed().omega.to<double>(), 0.1);
}
