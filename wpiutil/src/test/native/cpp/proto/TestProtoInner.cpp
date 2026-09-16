// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "TestProtoInner.hpp"

#include <string>
#include <utility>

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>

#include "wpi/util/protobuf/ProtobufCallbacks.hpp"
#include "wpiutil_test.npb.h"

std::optional<TestProtoInner> wpi::util::Protobuf<TestProtoInner>::Unpack(
    wpi::util::ProtoInputStream<TestProtoInner>& stream) {
  wpi::util::UnpackCallback<std::string> str;
  wpi_proto_TestProtoInner msg{
      .msg = str.Callback(),
  };
  if (!stream.Decode(msg)) {
    return {};
  }

  auto istr = str.Items();

  if (istr.empty()) {
    return {};
  }

  return TestProtoInner{std::move(istr[0])};
}

bool wpi::util::Protobuf<TestProtoInner>::Pack(
    wpi::util::ProtoOutputStream<TestProtoInner>& stream,
    const TestProtoInner& value) {
  wpi::util::PackCallback str{&value.msg};
  wpi_proto_TestProtoInner msg{
      .msg = str.Callback(),
  };
  return stream.Encode(msg);
}

namespace {
using ProtoType = wpi::util::Protobuf<TestProtoInner>;
}  // namespace

TEST_CASE("TestProtoInnerTest RoundtripNanopb", "[wpiutil][proto]") {
  const TestProtoInner EXPECTED_DATA = TestProtoInner{"Hello!"};

  wpi::util::ProtobufMessage<TestProtoInner> message;
  wpi::util::SmallVector<uint8_t, 64> buf;

  REQUIRE(message.Pack(buf, EXPECTED_DATA));
  std::optional<TestProtoInner> unpacked_data = message.Unpack(buf);
  REQUIRE(unpacked_data.has_value());
  CHECK(EXPECTED_DATA.msg == unpacked_data->msg);
}

TEST_CASE("TestProtoInnerTest RoundtripNanopbEmpty", "[wpiutil][proto]") {
  const TestProtoInner EXPECTED_DATA = TestProtoInner{"Hello!"};

  wpi::util::ProtobufMessage<decltype(EXPECTED_DATA)> message;
  wpi::util::SmallVector<uint8_t, 64> buf;

  REQUIRE(message.Pack(buf, EXPECTED_DATA));
  auto unpacked_data = message.Unpack(buf);
  REQUIRE(unpacked_data.has_value());
  CHECK(EXPECTED_DATA.msg == unpacked_data->msg);
}
