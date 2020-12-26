// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <string>
#include <thread>

#include <wpi/SmallString.h>
#include <wpi/StringRef.h>
#include <wpi/raw_ostream.h>

#include "frc/ScopedTracer.h"
#include "gtest/gtest.h"

wpi::SmallString<128> buf;
wpi::raw_svector_ostream os(buf);

#ifdef __APPLE__
TEST(ScopedTracerTest, DISABLED_Timing) {
#else
TEST(ScopedTracerTest, Timing) {
#endif
  {
    frc::ScopedTracer tracer("timing_test", os);
    std::this_thread::sleep_for(std::chrono::milliseconds(1500));
  }

  wpi::StringRef out = os.str();
  EXPECT_TRUE(out.startswith("	timing_test: 1.5"));
}
