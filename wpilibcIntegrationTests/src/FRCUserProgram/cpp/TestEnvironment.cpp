/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <cstdlib>

#include "DriverStation.h"
#include "HAL/HAL.h"
#include "LiveWindow/LiveWindow.h"
#include "Timer.h"
#include "gtest/gtest.h"
#include "llvm/raw_ostream.h"

using namespace frc;

class TestEnvironment : public testing::Environment {
  bool m_alreadySetUp = false;

 public:
  void SetUp() override {
    /* Only set up once.  This allows gtest_repeat to be used to
            automatically repeat tests. */
    if (m_alreadySetUp) return;
    m_alreadySetUp = true;

    if (!HAL_Initialize(500, 0)) {
      llvm::errs() << "FATAL ERROR: HAL could not be initialized\n";
      std::exit(-1);
    }

    /* This sets up the network communications library to enable the driver
            station. After starting network coms, it will loop until the driver
            station returns that the robot is enabled, to ensure that tests
            will be able to run on the hardware. */
    HAL_ObserveUserProgramStarting();
    LiveWindow::GetInstance()->SetEnabled(false);

    llvm::outs() << "Waiting for enable\n";

    while (!DriverStation::GetInstance().IsEnabled()) {
      Wait(0.1);
    }
  }

  void TearDown() override {}
};

testing::Environment* const environment =
    testing::AddGlobalTestEnvironment(new TestEnvironment);
