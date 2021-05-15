// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <cstdlib>

#include <hal/HAL.h>
#include <wpi/raw_ostream.h>

#include "frc/DriverStation.h"
#include "frc/Timer.h"
#include "frc/livewindow/LiveWindow.h"
#include "gtest/gtest.h"
#include "mockds/MockDS.h"

using namespace frc;

class TestEnvironment : public testing::Environment {
  bool m_alreadySetUp = false;
  MockDS m_mockDS;

 public:
  void SetUp() override {
    /* Only set up once.  This allows gtest_repeat to be used to
            automatically repeat tests. */
    if (m_alreadySetUp) {
      return;
    }
    m_alreadySetUp = true;

    if (!HAL_Initialize(500, 0)) {
      wpi::errs() << "FATAL ERROR: HAL could not be initialized\n";
      std::exit(-1);
    }

    m_mockDS.start();

    /* This sets up the network communications library to enable the driver
            station. After starting network coms, it will loop until the driver
            station returns that the robot is enabled, to ensure that tests
            will be able to run on the hardware. */
    HAL_ObserveUserProgramStarting();
    LiveWindow::GetInstance()->SetEnabled(false);

    wpi::outs() << "Started coms\n";

    int enableCounter = 0;
    while (!DriverStation::GetInstance().IsEnabled()) {
      if (enableCounter > 50) {
        // Robot did not enable properly after 5 seconds.
        // Force exit
        wpi::errs() << " Failed to enable. Aborting\n";
        std::terminate();
      }

      Wait(0.1);

      wpi::outs() << "Waiting for enable: " << enableCounter++ << "\n";
    }
  }

  void TearDown() override { m_mockDS.stop(); }
};

testing::Environment* const environment =
    testing::AddGlobalTestEnvironment(new TestEnvironment);
