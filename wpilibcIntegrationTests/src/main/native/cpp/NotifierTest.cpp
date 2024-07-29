// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/Notifier.h"  // NOLINT(build/include_order)

#include <gtest/gtest.h>
#include <wpi/print.h>

#include "frc/Timer.h"

TEST(NotifierTest, StartPeriodicAndStop) {
  uint32_t counter = 0;

  frc::Notifier notifier{[&] { ++counter; }};
  notifier.StartPeriodic(1_s);

  frc::Wait(10.5_s);

  notifier.Stop();
  EXPECT_EQ(10u, counter) << "Received " << counter
                          << " notifications in 10.5 seconds\n";
  wpi::print("Received {} notifications in 10.5 seconds\n", counter);

  frc::Wait(3_s);

  EXPECT_EQ(10u, counter) << "Received " << counter - 10
                          << " notifications in 3 seconds\n";
  wpi::print("Received {} notifications in 3 seconds\n", counter - 10);
}

TEST(NotifierTest, StartSingle) {
  uint32_t counter = 0;

  frc::Notifier notifier{[&] { ++counter; }};
  notifier.StartSingle(1_s);

  frc::Wait(10.5_s);

  EXPECT_EQ(1u, counter) << "Received " << counter
                         << " notifications in 10.5 seconds\n";
  wpi::print("Received {} notifications in 10.5 seconds\n", counter);
}
