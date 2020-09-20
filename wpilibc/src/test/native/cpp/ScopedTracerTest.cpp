/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

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
