// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <atomic>

#include <hal/HAL.h>

#include "frc/AsynchronousInterrupt.h"
#include "frc/DigitalInput.h"
#include "frc/Timer.h"
#include "frc/simulation/DIOSim.h"
#include "gtest/gtest.h"

namespace frc {
using namespace frc::sim;
TEST(InterruptTest, AsynchronousInterrupt) {
  HAL_Initialize(500, 0);

  std::atomic_int counter{0};
  std::atomic_bool hasFired{false};

  DigitalInput di{0};
  AsynchronousInterrupt interrupt{di, [&](bool rising, bool falling) {
                                    counter++;
                                    hasFired = true;
                                  }};

  interrupt.Enable();
  frc::Wait(0.5_s);
  DIOSim digitalSim{di};
  digitalSim.SetValue(false);
  frc::Wait(20_ms);
  digitalSim.SetValue(true);
  frc::Wait(10_ms);

  int count = 0;
  while (!hasFired) {
    frc::Wait(5_ms);
    count++;
    ASSERT_TRUE(count < 1000);
  }
  ASSERT_EQ(1, counter.load());
}
}  // namespace frc
