// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <string_view>

#include <gtest/gtest.h>
#include <wpi/SmallString.h>
#include <wpi/StringExtras.h>
#include <wpi/raw_ostream.h>

#include "frc/ScopedTracer.h"
#include "frc/simulation/SimHooks.h"

TEST(ScopedTracerTest, Timing) {
  wpi::SmallString<128> buf;
  wpi::raw_svector_ostream os(buf);

  frc::sim::PauseTiming();
  {
    frc::ScopedTracer tracer("timing_test", os);
    frc::sim::StepTiming(1.5_s);
  }
  frc::sim::ResumeTiming();

  std::string_view out = os.str();
  EXPECT_TRUE(wpi::starts_with(out, "\ttiming_test: 1.5"));
}
