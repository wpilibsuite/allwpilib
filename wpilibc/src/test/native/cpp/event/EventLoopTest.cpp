// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "frc/Errors.h"
#include "frc/event/EventLoop.h"

using namespace frc;

TEST(EventLoopTest, ConcurrentModification) {
  EventLoop loop;

  loop.Bind([&loop] { ASSERT_THROW(loop.Bind([] {}), frc::RuntimeError); });

  loop.Poll();

  loop.Clear();

  loop.Bind([&loop] { ASSERT_THROW(loop.Clear(), frc::RuntimeError); });

  loop.Poll();
}
