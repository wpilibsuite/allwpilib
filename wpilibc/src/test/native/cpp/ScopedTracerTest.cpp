// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/system/ScopedTracer.hpp"

#include <string_view>

#include <gtest/gtest.h>

#include "wpi/simulation/SimHooks.hpp"
#include "wpi/util/StringExtras.hpp"
#include "wpi/util/raw_ostream.hpp"

TEST(ScopedTracerTest, Timing) {
  std::string buf;
  wpi::util::raw_string_ostream os(buf);

  wpi::sim::PauseTiming();
  {
    wpi::ScopedTracer tracer("timing_test", os);
    wpi::sim::StepTiming(1.5_s);
  }
  wpi::sim::ResumeTiming();

  EXPECT_TRUE(wpi::util::starts_with(buf, "\ttiming_test: 1.5"));
}
