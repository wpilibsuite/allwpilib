// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <catch2/catch_test_macros.hpp>

#include "wpi/math/controller/ElevatorFeedforward.hpp"
#include "wpi/util/SmallVector.hpp"

using namespace wpi::math;

namespace {

static constexpr auto Ks = 1.91_V;
static constexpr auto Kg = 2.29_V;
static constexpr auto Kv = 35.04_V * 1_s / 1_m;
static constexpr auto Ka = 1.74_V * 1_s * 1_s / 1_m;

constexpr ElevatorFeedforward EXPECTED_DATA{Ks, Kg, Kv, Ka};
}  // namespace

TEST_CASE("ElevatorFeedforwardProtoTest Roundtrip", "[wpimath]") {
  wpi::util::ProtobufMessage<decltype(EXPECTED_DATA)> message;
  wpi::util::SmallVector<uint8_t, 64> buf;

  REQUIRE(message.Pack(buf, EXPECTED_DATA));
  auto unpacked_data = message.Unpack(buf);
  REQUIRE(unpacked_data.has_value());

  CHECK(EXPECTED_DATA.GetKs().value() == unpacked_data->GetKs().value());
  CHECK(EXPECTED_DATA.GetKg().value() == unpacked_data->GetKg().value());
  CHECK(EXPECTED_DATA.GetKv().value() == unpacked_data->GetKv().value());
  CHECK(EXPECTED_DATA.GetKa().value() == unpacked_data->GetKa().value());
}
