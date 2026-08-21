// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <catch2/catch_test_macros.hpp>

#include "wpi/util/SmallVector.hpp"
#include "wpi/util/protobuf/Protobuf.hpp"

template <typename T>
class ProtoTest {};

#define CATCH_TYPED_TEST_SUITE_P(Suite)

#define CATCH_TYPED_TEST_P(Suite, Name) \
  template <typename TypeParam>         \
  void Suite##_##Name()

#define REGISTER_CATCH_TYPED_TEST_SUITE_P(Suite, ...)

#define INSTANTIATE_CATCH_TYPED_TEST_SUITE_P(Prefix, Suite, TypeParam) \
  TEST_CASE(#Suite " " #Prefix " RoundTrip", "[wpimath]") {            \
    Suite##_RoundTrip<TypeParam>();                                    \
  }

CATCH_TYPED_TEST_P(ProtoTest, RoundTrip) {
  wpi::util::ProtobufMessage<decltype(TypeParam::TEST_DATA)> message;
  wpi::util::SmallVector<uint8_t, 64> buf;

  REQUIRE(message.Pack(buf, TypeParam::TEST_DATA));
  auto unpacked_data = message.Unpack(buf);
  REQUIRE(unpacked_data.has_value());

  TypeParam::CheckEq(TypeParam::TEST_DATA, *unpacked_data);
}

REGISTER_CATCH_TYPED_TEST_SUITE_P(ProtoTest, RoundTrip);
