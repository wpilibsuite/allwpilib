// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <gtest/gtest.h>
#include <wpi/SmallVector.h>
#include <wpi/protobuf/Protobuf.h>

template <typename T>
class ProtoTest : public testing::Test {};

TYPED_TEST_SUITE_P(ProtoTest);

TYPED_TEST_P(ProtoTest, RoundTrip) {
  wpi::ProtobufMessage<decltype(TypeParam::kTestData)> message;
  wpi::SmallVector<uint8_t, 64> buf;

  ASSERT_TRUE(message.Pack(buf, TypeParam::kTestData));
  auto unpacked_data = message.Unpack(buf);
  ASSERT_TRUE(unpacked_data.has_value());

  TypeParam::CheckEq(TypeParam::kTestData, *unpacked_data);
}

REGISTER_TYPED_TEST_SUITE_P(ProtoTest, RoundTrip);
