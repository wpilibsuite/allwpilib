// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <atomic>

#include <gtest/gtest.h>
#include <hal/HAL.h>

#include "frc/AsynchronousInterrupt.h"
#include "frc/DigitalInput.h"
#include "frc/Timer.h"
#include "frc/simulation/DIOSim.h"

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
  frc::Wait(20_ms);

  int count = 0;
  while (!hasFired) {
    frc::Wait(5_ms);
    count++;
    ASSERT_TRUE(count < 1000);
  }
  ASSERT_EQ(1, counter.load());
}

TEST(InterruptTest, RisingEdge) {
  HAL_Initialize(500, 0);

  std::atomic_bool hasFiredFallingEdge{false};
  std::atomic_bool hasFiredRisingEdge{false};

  DigitalInput di{0};
  AsynchronousInterrupt interrupt{di, [&](bool rising, bool falling) {
                                    hasFiredFallingEdge = falling;
                                    hasFiredRisingEdge = rising;
                                  }};
  interrupt.SetInterruptEdges(true, true);
  DIOSim digitalSim{di};
  digitalSim.SetValue(false);
  interrupt.Enable();
  frc::Wait(0.5_s);
  digitalSim.SetValue(true);
  frc::Wait(20_ms);

  int count = 0;
  while (!hasFiredRisingEdge) {
    frc::Wait(5_ms);
    count++;
    ASSERT_TRUE(count < 1000);
  }
  EXPECT_FALSE(hasFiredFallingEdge);
  EXPECT_TRUE(hasFiredRisingEdge);
}

TEST(InterruptTest, FallingEdge) {
  HAL_Initialize(500, 0);

  std::atomic_bool hasFiredFallingEdge{false};
  std::atomic_bool hasFiredRisingEdge{false};

  DigitalInput di{0};
  AsynchronousInterrupt interrupt{di, [&](bool rising, bool falling) {
                                    hasFiredFallingEdge = falling;
                                    hasFiredRisingEdge = rising;
                                  }};
  interrupt.SetInterruptEdges(true, true);
  DIOSim digitalSim{di};
  digitalSim.SetValue(true);
  interrupt.Enable();
  frc::Wait(0.5_s);
  digitalSim.SetValue(false);
  frc::Wait(20_ms);

  int count = 0;
  while (!hasFiredFallingEdge) {
    frc::Wait(5_ms);
    count++;
    ASSERT_TRUE(count < 1000);
  }
  EXPECT_TRUE(hasFiredFallingEdge);
  EXPECT_FALSE(hasFiredRisingEdge);
}
}  // namespace frc
