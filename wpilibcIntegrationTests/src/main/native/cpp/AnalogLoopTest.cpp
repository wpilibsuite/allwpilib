// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <memory>

#include <gtest/gtest.h>
#include <units/time.h>

#include "TestBench.h"
#include "frc/AnalogInput.h"
#include "frc/AnalogOutput.h"
#include "frc/AnalogTrigger.h"
#include "frc/AsynchronousInterrupt.h"
#include "frc/Counter.h"
#include "frc/Timer.h"

static constexpr auto kDelayTime = 10_ms;

/**
 * Test analog inputs and outputs by setting one and making sure the other
 * matches.
 */
TEST(AnalogLoopTest, AnalogInputWorks) {
  frc::AnalogInput input{TestBench::kFakeAnalogOutputChannel};
  frc::AnalogOutput output{TestBench::kAnalogOutputChannel};

  // Set the output voltage and check if the input measures the same voltage
  for (int32_t i = 0; i < 50; i++) {
    output.SetVoltage(i / 10.0);

    frc::Wait(kDelayTime);

    EXPECT_NEAR(output.GetVoltage(), input.GetVoltage(), 0.01);
  }
}

/**
 * Test if we can use an analog trigger to  check if the output is within a
 * range correctly.
 */
TEST(AnalogLoopTest, AnalogTriggerWorks) {
  frc::AnalogInput input{TestBench::kFakeAnalogOutputChannel};
  frc::AnalogOutput output{TestBench::kAnalogOutputChannel};

  frc::AnalogTrigger trigger{&input};
  trigger.SetLimitsVoltage(2.0, 3.0);

  output.SetVoltage(1.0);
  frc::Wait(kDelayTime);

  EXPECT_FALSE(trigger.GetInWindow())
      << "Analog trigger is in the window (2V, 3V)";
  EXPECT_FALSE(trigger.GetTriggerState()) << "Analog trigger is on";

  output.SetVoltage(2.5);
  frc::Wait(kDelayTime);

  EXPECT_TRUE(trigger.GetInWindow())
      << "Analog trigger is not in the window (2V, 3V)";
  EXPECT_FALSE(trigger.GetTriggerState()) << "Analog trigger is on";

  output.SetVoltage(4.0);
  frc::Wait(kDelayTime);

  EXPECT_FALSE(trigger.GetInWindow())
      << "Analog trigger is in the window (2V, 3V)";
  EXPECT_TRUE(trigger.GetTriggerState()) << "Analog trigger is not on";
}

/**
 * Test if we can count the right number of ticks from an analog trigger with
 * a counter.
 */
TEST(AnalogLoopTest, AnalogTriggerCounterWorks) {
  frc::AnalogInput input{TestBench::kFakeAnalogOutputChannel};
  frc::AnalogOutput output{TestBench::kAnalogOutputChannel};

  frc::AnalogTrigger trigger{&input};
  trigger.SetLimitsVoltage(2.0, 3.0);

  frc::Counter counter{trigger};

  // Turn the analog output low and high 50 times
  for (int32_t i = 0; i < 50; i++) {
    output.SetVoltage(1.0);
    frc::Wait(kDelayTime);
    output.SetVoltage(4.0);
    frc::Wait(kDelayTime);
  }

  // The counter should be 50
  EXPECT_EQ(50, counter.Get())
      << "Analog trigger counter did not count 50 ticks";
}

TEST(AnalogLoopTest, AsynchronusInterruptWorks) {
  frc::AnalogInput input{TestBench::kFakeAnalogOutputChannel};
  frc::AnalogOutput output{TestBench::kAnalogOutputChannel};

  int32_t param = 0;
  frc::AnalogTrigger trigger{&input};
  trigger.SetLimitsVoltage(2.0, 3.0);

  // Given an interrupt handler that sets an int32_t to 12345
  std::shared_ptr<frc::AnalogTriggerOutput> triggerOutput =
      trigger.CreateOutput(frc::AnalogTriggerType::kState);

  frc::AsynchronousInterrupt interrupt{triggerOutput,
                                       [&](auto a, auto b) { param = 12345; }};

  interrupt.Enable();

  // If the analog output moves from below to above the window
  output.SetVoltage(0.0);
  frc::Wait(kDelayTime);
  output.SetVoltage(5.0);
  // Then the int32_t should be 12345
  frc::Wait(kDelayTime);
  interrupt.Disable();

  EXPECT_EQ(12345, param) << "The interrupt did not run.";
}
