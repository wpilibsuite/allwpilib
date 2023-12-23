// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/DataLog.h"

#include <gtest/gtest.h>

TEST(DataLog, SimpleInt) {
  std::vector<uint8_t> data;
  {
    wpi::log::DataLog log{
        [&](auto out) { data.insert(data.end(), out.begin(), out.end()); }};
    int entry = log.Start("test", "int64");
    log.AppendInteger(entry, 1, 0);
  }
  ASSERT_EQ(data.size(), 66u);
}
