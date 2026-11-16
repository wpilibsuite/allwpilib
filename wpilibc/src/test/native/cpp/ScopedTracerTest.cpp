// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/system/ScopedTracer.hpp"

#include <string_view>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "wpi/simulation/SimHooks.hpp"
#include "wpi/util/SmallString.hpp"
#include "wpi/util/StringExtras.hpp"
#include "wpi/util/raw_ostream.hpp"

TEST_CASE("ScopedTracerTest Timing", "[wpilibc]") {
  wpi::util::SmallString<128> buf;
  wpi::util::raw_svector_ostream os(buf);

  wpi::sim::PauseTiming();
  {
    wpi::ScopedTracer tracer("timing_test", os);
    wpi::sim::StepTiming(1.5_s);
  }
  wpi::sim::ResumeTiming();

  std::string_view out = os.str();
  CHECK(wpi::util::starts_with(out, "\ttiming_test: 1.5"));
}
