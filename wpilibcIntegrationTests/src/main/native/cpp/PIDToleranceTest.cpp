/*----------------------------------------------------------------------------*/
/* Copyright (c) 2014-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "TestBench.h"
#include "frc/Timer.h"
#include "frc/experimental/controller/ControllerRunner.h"
#include "frc/experimental/controller/PIDController.h"
#include "gtest/gtest.h"

using namespace frc;

class PIDToleranceTest : public testing::Test {
 protected:
  const double reference = 50.0;
  const double range = 200;
  const double tolerance = 10.0;

  class FakeInput {
   public:
    double val = 0;

    double Get() { return val; }
  };

  class FakeOutput {
   public:
    void Set(double output) {}
  };

  FakeInput inp;
  FakeOutput out;
  frc::experimental::PIDController* pidController;
  frc::experimental::ControllerRunner* pidRunner;

  void SetUp() override {
    pidController = new frc::experimental::PIDController(
        0.5, 0.0, 0.0, [&] { return inp.Get(); });
    pidController->SetInputRange(-range / 2, range / 2);
    pidRunner = new frc::experimental::ControllerRunner(
        *pidController, [&](double output) { out.Set(output); });
  }

  void TearDown() override {
    delete pidRunner;
    delete pidController;
  }

  void Reset() { inp.val = 0; }
};

TEST_F(PIDToleranceTest, Absolute) {
  Reset();

  pidController->SetAbsoluteTolerance(tolerance);
  pidController->SetReference(reference);
  pidRunner->Enable();

  EXPECT_FALSE(pidController->AtReference())
      << "Error was in tolerance when it should not have been. Error was "
      << pidController->GetError();

  inp.val = reference + tolerance / 2;
  Wait(1.0);

  EXPECT_TRUE(pidController->AtReference())
      << "Error was not in tolerance when it should have been. Error was "
      << pidController->GetError();

  inp.val = reference + 10 * tolerance;
  Wait(1.0);

  EXPECT_FALSE(pidController->AtReference())
      << "Error was in tolerance when it should not have been. Error was "
      << pidController->GetError();
}

TEST_F(PIDToleranceTest, Percent) {
  Reset();

  pidController->SetPercentTolerance(tolerance);
  pidController->SetReference(reference);
  pidRunner->Enable();

  EXPECT_FALSE(pidController->AtReference())
      << "Error was in tolerance when it should not have been. Error was "
      << pidController->GetError();

  inp.val =
      reference + (tolerance) / 200 *
                      range;  // half of percent tolerance away from reference
  Wait(1.0);

  EXPECT_TRUE(pidController->AtReference())
      << "Error was not in tolerance when it should have been. Error was "
      << pidController->GetError();

  inp.val =
      reference +
      (tolerance) / 50 * range;  // double percent tolerance away from setPoint

  Wait(1.0);

  EXPECT_FALSE(pidController->AtReference())
      << "Error was in tolerance when it should not have been. Error was "
      << pidController->GetError();
}
