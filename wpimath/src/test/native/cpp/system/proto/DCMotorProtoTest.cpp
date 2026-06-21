// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "wpi/math/system/DCMotor.hpp"
#include "wpi/util/SmallVector.hpp"

using namespace wpi::math;

using ProtoType = wpi::util::Protobuf<wpi::math::DCMotor>;

inline constexpr DCMotor kExpectedData =
    DCMotor{1.91_V, 19.1_Nm, 1.74_A, 2.29_A, 2.2_rad_per_s, 2};

TEST_CASE("DCMotorProtoTest Roundtrip", "[wpimath]") {
  wpi::util::ProtobufMessage<decltype(kExpectedData)> message;
  wpi::util::SmallVector<uint8_t, 64> buf;

  REQUIRE(message.Pack(buf, kExpectedData));
  auto unpacked_data = message.Unpack(buf);
  REQUIRE(unpacked_data.has_value());

  CHECK(kExpectedData.nominalVoltage.value() ==
        unpacked_data->nominalVoltage.value());
  CHECK(kExpectedData.stallTorque.value() ==
        unpacked_data->stallTorque.value());
  CHECK(kExpectedData.stallCurrent.value() ==
        unpacked_data->stallCurrent.value());
  CHECK(kExpectedData.freeCurrent.value() ==
        unpacked_data->freeCurrent.value());
  CHECK(kExpectedData.freeSpeed.value() == unpacked_data->freeSpeed.value());
  CHECK(kExpectedData.R.value() == unpacked_data->R.value());
  CHECK(kExpectedData.Kv.value() == unpacked_data->Kv.value());
  CHECK(kExpectedData.Kt.value() == unpacked_data->Kt.value());
}
