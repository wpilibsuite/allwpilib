// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>
#include <wpi/SmallVector.h>

#include "frc/controller/ArmFeedforward.h"

using namespace frc;

namespace {

static constexpr auto S = 1.91_V;
static constexpr auto G = 2.29_V;
static constexpr auto V = 35.04_V * 1_s / 1_rad;
static constexpr auto A = 1.74_V * 1_s * 1_s / 1_rad;
const ArmFeedforward EXPECTED_DATA{S, G, V, A};
}  // namespace

TEST(ArmFeedforwardProtoTest, Roundtrip) {
  wpi::ProtobufMessage<decltype(EXPECTED_DATA)> message;
  wpi::SmallVector<uint8_t, 64> buf;

  ASSERT_TRUE(message.Pack(buf, EXPECTED_DATA));
  auto unpacked_data = message.Unpack(buf);
  ASSERT_TRUE(unpacked_data.has_value());

  EXPECT_EQ(EXPECTED_DATA.GetKs().value(), unpacked_data->GetKs().value());
  EXPECT_EQ(EXPECTED_DATA.GetKg().value(), unpacked_data->GetKg().value());
  EXPECT_EQ(EXPECTED_DATA.GetKv().value(), unpacked_data->GetKv().value());
  EXPECT_EQ(EXPECTED_DATA.GetKa().value(), unpacked_data->GetKa().value());
}
