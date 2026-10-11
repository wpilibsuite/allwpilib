// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <memory>
#include <numbers>
#include <string>
#include <vector>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "wpi/hal/HAL.h"
#include "wpi/hal/IMU.h"
#include "wpi/hal/simulation/IMUData.h"
#include "wpi/halsim/ws_core/WSProviderContainer.hpp"
#include "wpi/halsim/ws_core/WSProvider_IMU.hpp"

using namespace wpilibws;
using wpi::util::json;

namespace {
class Connection : public HALSimBaseWebSocketConnection {
 public:
  void OnSimValueChanged(const json& msg) override { messages.push_back(msg); }
  std::vector<json> messages;
};

struct IMUProviderTest {
  IMUProviderTest() {
    HAL_Initialize();
    ResetData();
  }
  ~IMUProviderTest() { ResetData(); }
  void ResetData() {
    HALSIM_SetIMUAngleX(0);
    HALSIM_SetIMUAngleY(0);
    HALSIM_SetIMUAngleZ(0);
    HALSIM_SetIMUGyroRateX(0);
    HALSIM_SetIMUGyroRateY(0);
    HALSIM_SetIMUGyroRateZ(0);
    HALSIM_SetIMUAccelX(0);
    HALSIM_SetIMUAccelY(0);
    HALSIM_SetIMUAccelZ(0);
    HALSIM_SetIMUYaw(0);
  }
};
}  // namespace

TEST_CASE_METHOD(IMUProviderTest,
                 "IMU providers initialize firmware sensors on reconnect",
                 "[imu]") {
  for (auto name : {"RomiGyro", "XRPGyro"}) {
    ProviderContainer providers;
    HALSimWSProviderIMU::Initialize(
        [&](auto key, auto provider) { providers.Add(key, provider); }, name);
    auto gyro = providers.Get(std::string{"Gyro/"} + name);
    auto accel = providers.Get("Accel/BuiltInAccel");
    REQUIRE(gyro);
    REQUIRE(accel);
    auto connection = std::make_shared<Connection>();
    for (int i = 0; i < 2; ++i) {
      gyro->OnNetworkConnected(connection);
      accel->OnNetworkConnected(connection);
      REQUIRE(connection->messages.size() == 2);
      CHECK(connection->messages[0] ==
            json::object("type", "Gyro", "device", name, "data",
                         json::object("<init", true)));
      CHECK(connection->messages[1] ==
            json::object("type", "Accel", "device", "BuiltInAccel", "data",
                         json::object("<init", true, "<range", 8)));
      gyro->OnNetworkDisconnected();
      accel->OnNetworkDisconnected();
      connection->messages.clear();
    }
  }
}

TEST_CASE_METHOD(IMUProviderTest,
                 "IMU providers convert units and preserve partial updates",
                 "[imu]") {
  HALSimWSProviderIMU gyro{"Gyro/RomiGyro", "Gyro", "RomiGyro"};
  HALSimWSProviderIMU accel{"Accel/BuiltInAccel", "Accel", "BuiltInAccel"};
  gyro.OnNetValueChanged(json::object(">angle_x", 30, ">angle_y", -45,
                                      ">angle_z", 450, ">rate_x", 90, ">rate_y",
                                      -180, ">rate_z", 270));
  accel.OnNetValueChanged(json::object(">x", -0.5, ">y", 0.25, ">z", 1));
  // Invalid fields and unrelated device fields must not overwrite sensor data.
  gyro.OnNetValueChanged(json::object(">angle_x", "invalid", ">angle_y", false,
                                      ">rate_x", nullptr, ">x", 5));
  accel.OnNetValueChanged(
      json::object(">x", "invalid", ">y", nullptr, ">z", true, ">angle_z", 1));
  int32_t status = 0;
  int64_t timestamp = 0;
  HAL_EulerAngles3d angles;
  HAL_GetIMUEulerAnglesFlat(&angles, &status);
  CHECK(angles.x == Catch::Approx(std::numbers::pi / 6));
  CHECK(angles.y == Catch::Approx(-std::numbers::pi / 4));
  CHECK(angles.z == Catch::Approx(2.5 * std::numbers::pi));
  CHECK(HAL_GetIMUYawFlat(&timestamp) == angles.z);
  HAL_GyroRate3d rates;
  HAL_GetIMUGyroRates(&rates, &status);
  CHECK(rates.x == Catch::Approx(std::numbers::pi / 2));
  CHECK(rates.y == Catch::Approx(-std::numbers::pi));
  CHECK(rates.z == Catch::Approx(1.5 * std::numbers::pi));
  HAL_Acceleration3d acceleration;
  HAL_GetIMUAcceleration(&acceleration, &status);
  CHECK(acceleration.x == Catch::Approx(-4.903325));
  CHECK(acceleration.y == Catch::Approx(2.4516625));
  CHECK(acceleration.z == Catch::Approx(9.80665));
  gyro.OnNetValueChanged(json::object(">angle_z", 480));
  accel.OnNetValueChanged(json::object(">y", -1));
  HAL_GetIMUEulerAnglesFlat(&angles, &status);
  CHECK(angles.x == Catch::Approx(std::numbers::pi / 6));
  CHECK(angles.z == Catch::Approx(8 * std::numbers::pi / 3));
  CHECK(HAL_GetIMUYawFlat(&timestamp) == angles.z);
  HAL_GetIMUAcceleration(&acceleration, &status);
  CHECK(acceleration.x == Catch::Approx(-4.903325));
  CHECK(acceleration.y == Catch::Approx(-9.80665));
  CHECK(acceleration.z == Catch::Approx(9.80665));
  CHECK(status == 0);
}
