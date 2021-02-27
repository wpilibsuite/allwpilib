// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <wpi/SmallString.h>
#include <wpi/StringRef.h>
#include <wpi/raw_ostream.h>

#include "frc/ScopedTracer.h"
#include "frc/simulation/SimHooks.h"
#include "gtest/gtest.h"

wpi::SmallString<128> buf;
wpi::raw_svector_ostream os(buf);

TEST(ScopedTracerTest, Timing) {
  frc::sim::PauseTiming();
  {
    frc::ScopedTracer tracer("timing_test", os);
    frc::sim::StepTiming(1.5_s);
  }
  frc::sim::ResumeTiming();

  wpi::StringRef out = os.str();
  EXPECT_TRUE(out.startswith("	timing_test: 1.5"));
}
