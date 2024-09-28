// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <string_view>

#include <gtest/gtest.h>
#include <networktables/DoubleTopic.h>
#include <networktables/NetworkTableInstance.h>

#include "frc/Tracer.h"
#include "frc/simulation/SimHooks.h"

TEST(TracerTest, TraceFunc) {
  frc::Tracer::SetThreadName("TracerTest");

  frc::sim::PauseTiming();

  frc::Tracer::StartTrace("Test1");
  frc::Tracer::TraceFunc("Test2", []() { frc::sim::StepTiming(400_ms); });
  frc::sim::StepTiming(100_ms);
  frc::Tracer::EndTrace();

  auto test1Entry = nt::NetworkTableInstance::GetDefault().GetEntry(
      "/Tracer/TracerTest/Test1");
  auto test2Entry = nt::NetworkTableInstance::GetDefault().GetEntry(
      "/Tracer/TracerTest/Test1/Test2");

  EXPECT_TRUE(test1Entry.GetDouble(0.0) - 500.0 < 1.0);
  EXPECT_TRUE(test2Entry.GetDouble(0.0) - 400.0 < 1.0);

  frc::sim::ResumeTiming();
}

TEST(TracerTest, ScopedTrace) {
  frc::Tracer::SetThreadName("TracerTest");

  frc::sim::PauseTiming();

  frc::Tracer::StartTrace("Test3");
  {
    frc::Tracer::StartScopedTrace("Test4");
    frc::sim::StepTiming(400_ms);
  }
  frc::sim::StepTiming(100_ms);
  frc::Tracer::EndTrace();

  auto test3Entry = nt::NetworkTableInstance::GetDefault().GetEntry(
      "/Tracer/TracerTest/Test3");
  auto test4Entry = nt::NetworkTableInstance::GetDefault().GetEntry(
      "/Tracer/TracerTest/Test3/Test4");

  EXPECT_TRUE(test3Entry.GetDouble(0.0) - 500.0 < 1.0);
  EXPECT_TRUE(test4Entry.GetDouble(0.0) - 400.0 < 1.0);

  frc::sim::ResumeTiming();
}