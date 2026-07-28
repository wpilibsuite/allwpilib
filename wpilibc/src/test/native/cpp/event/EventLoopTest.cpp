// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/event/EventLoop.hpp"

#include <gtest/gtest.h>

#include "wpi/system/Errors.hpp"

using namespace wpi;

TEST(EventLoopTest, ConcurrentModification) {
  EventLoop loop;

  loop.Bind([&loop] { ASSERT_THROW(loop.Bind([] {}), wpi::RuntimeError); });

  loop.Poll();

  loop.Clear();

  loop.Bind([&loop] { ASSERT_THROW(loop.Clear(), wpi::RuntimeError); });

  loop.Poll();
}
