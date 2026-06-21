// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <catch2/catch_test_macros.hpp>

#include "wpi/util/SmallVector.hpp"
#include "wpi/util/protobuf/Protobuf.hpp"

template <typename TypeParam>
void CheckProtoRoundTrip() {
  wpi::util::ProtobufMessage<decltype(TypeParam::kTestData)> message;
  wpi::util::SmallVector<uint8_t, 64> buf;

  REQUIRE(message.Pack(buf, TypeParam::kTestData));
  auto unpacked_data = message.Unpack(buf);
  REQUIRE(unpacked_data.has_value());

  TypeParam::CheckEq(TypeParam::kTestData, *unpacked_data);
}

#define INSTANTIATE_TYPED_TEST_SUITE_P(Prefix, Suite, TypeParam) \
  TEST_CASE(#Prefix " " #Suite " RoundTrip", "[wpimath]") {      \
    CheckProtoRoundTrip<TypeParam>();                            \
  }
