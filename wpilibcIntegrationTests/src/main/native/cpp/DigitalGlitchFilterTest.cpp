// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/DigitalGlitchFilter.h"  // NOLINT(build/include_order)

#include "frc/Counter.h"
#include "frc/DigitalInput.h"
#include "frc/Encoder.h"
#include "gtest/gtest.h"

using namespace frc;

/**
 * Tests that configuring inputs to be filtered succeeds.
 *
 * This test actually tests everything except that the actual FPGA
 * implementation works as intended.  We configure the FPGA and then query it to
 * make sure that the acutal configuration matches.
 */
TEST(DigitalGlitchFilterTest, BasicTest) {
  DigitalInput input1(1);
  DigitalInput input2(2);
  DigitalInput input3(3);
  DigitalInput input4(4);
  Encoder encoder5(5, 6);
  Counter counter7(7);

  // Check that we can make a single filter and set the period.
  DigitalGlitchFilter filter1;
  filter1.Add(&input1);
  filter1.SetPeriodNanoSeconds(4200);

  // Check that we can make a second filter with 2 inputs.
  DigitalGlitchFilter filter2;
  filter2.Add(&input2);
  filter2.Add(&input3);
  filter2.SetPeriodNanoSeconds(97100);

  // Check that we can make a third filter with an input, an encoder, and a
  // counter.
  DigitalGlitchFilter filter3;
  filter3.Add(&input4);
  filter3.Add(&encoder5);
  filter3.Add(&counter7);
  filter3.SetPeriodNanoSeconds(167800);

  // Verify that the period was properly set for all 3 filters.
  EXPECT_EQ(4200u, filter1.GetPeriodNanoSeconds());
  EXPECT_EQ(97100u, filter2.GetPeriodNanoSeconds());
  EXPECT_EQ(167800u, filter3.GetPeriodNanoSeconds());

  // Clean up.
  filter1.Remove(&input1);
  filter2.Remove(&input2);
  filter2.Remove(&input3);
  filter3.Remove(&input4);
  filter3.Remove(&encoder5);
  filter3.Remove(&counter7);
}
