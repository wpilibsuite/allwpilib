// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/halsim/xrp/XRP.hpp"

#include <array>
#include <cstdint>
#include <string>
#include <vector>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "wpi/util/json.hpp"

using namespace wpilibxrp;
using wpi::util::json;

namespace {

void ReceiveSequence(XRP& xrp, uint16_t seq) {
  std::array<uint8_t, 5> packet{static_cast<uint8_t>(seq >> 8),
                                static_cast<uint8_t>(seq), 0, 0, 0};
  xrp.HandleXRPUpdate(packet);
}

std::vector<uint8_t> MakeControl(XRP& xrp, std::string_view name = {}) {
  wpi::util::SmallVector<wpi::net::uv::Buffer, 4> buffers;
  {
    wpi::net::raw_uv_ostream stream{buffers, 512};
    if (name.empty()) {
      xrp.SetupXRPSendBuffer(stream);
    } else {
      xrp.SetupRenameDeviceBuffer(stream, name);
    }
  }
  std::vector<uint8_t> packet;
  for (auto& buffer : buffers) {
    auto bytes = buffer.bytes();
    packet.insert(packet.end(), bytes.begin(), bytes.end());
    buffer.Deallocate();
  }
  return packet;
}

}  // namespace

TEST_CASE("XRP accepts status gaps across sequence rollover", "[xrp]") {
  XRP xrp;
  ReceiveSequence(xrp, 65532);
  ReceiveSequence(xrp, 1);
  CHECK(xrp.GetDataSnapshot().status.packet.sequence == 1);
  ReceiveSequence(xrp, 65535);
  CHECK(xrp.GetDataSnapshot().status.packet.sequence == 1);
  ReceiveSequence(xrp, 2);
  CHECK(xrp.GetDataSnapshot().status.packet.sequence == 2);
}

TEST_CASE("XRP ignores duplicate stale and ambiguous status sequences",
          "[xrp]") {
  XRP xrp;
  ReceiveSequence(xrp, 10);
  auto first = xrp.GetDataSnapshot().status.packet.lastUpdate;
  for (uint16_t seq : {10, 9, 65535, 32778}) {
    ReceiveSequence(xrp, seq);
    CHECK(xrp.GetDataSnapshot().status.packet.sequence == 10);
    CHECK(xrp.GetDataSnapshot().status.packet.lastUpdate == first);
  }
  ReceiveSequence(xrp, 32777);
  CHECK(xrp.GetDataSnapshot().status.packet.sequence == 32777);
  xrp.ResetStatusPacketSequence();
  ReceiveSequence(xrp, 0);
  CHECK(xrp.GetDataSnapshot().status.packet.sequence == 0);
}

TEST_CASE("XRP malformed status does not consume a sequence", "[xrp]") {
  XRP xrp;
  ReceiveSequence(xrp, 1);
  std::array<uint8_t, 5> unknownField{0, 2, 0, 0x80, 0};
  std::array<uint8_t, 5> missingEncoder{0, 2, 0, 0, 1};
  std::array<uint8_t, 6> extraPayload{0, 2, 0, 0, 0, 0};
  xrp.HandleXRPUpdate(unknownField);
  xrp.HandleXRPUpdate(missingEncoder);
  xrp.HandleXRPUpdate(extraPayload);
  CHECK(xrp.GetDataSnapshot().status.packet.sequence == 1);
  ReceiveSequence(xrp, 2);
  CHECK(xrp.GetDataSnapshot().status.packet.sequence == 2);
}

TEST_CASE("XRP encoder update preserves count and signed period", "[xrp]") {
  XRP xrp;
  xrp.HandleWPILibUpdate(json::object(
      "type", "Encoder", "device", "3", "data",
      json::object("<init", true, "<channel_a", 4, "<channel_b", 5)));
  json update;
  xrp.SetWPILibUpdateFunc([&](const json& data) { update = data; });
  // Firmware encoding: count -42, period 500 us, reverse direction.
  std::array<uint8_t, 13> packet{0,    1,    0, 0, 1, 0xff, 0xff,
                                 0xff, 0xd6, 0, 0, 3, 0xe8};
  xrp.HandleXRPUpdate(packet);
  REQUIRE(update.at("data").is_object());
  CHECK(update.at("device").get_string() == "3");
  CHECK(update.at("data").at(">count").get_int() == -42);
  CHECK(update.at("data").at(">period").get_number() == Catch::Approx(-0.0005));

  packet[1] = 2;
  packet[12] = 0xe9;
  xrp.HandleXRPUpdate(packet);
  CHECK(update.at("data").at(">period").get_number() == Catch::Approx(0.0005));

  packet[1] = 3;
  for (int i = 9; i < 13; ++i) {
    packet[i] = 0xff;
  }
  xrp.HandleXRPUpdate(packet);
  REQUIRE(update.at("data").is_object());
  CHECK(update.at("data").at(">count").get_int() == -42);
  CHECK_FALSE(update.at("data").contains(">period"));
}

TEST_CASE("XRP control and rename use the firmware wire format", "[xrp]") {
  XRP xrp;
  xrp.HandleWPILibUpdate(json::object("type", "DriverStation", "data",
                                      json::object(">enabled", true)));
  const std::array motors{"motorL", "motorR", "motor3", "motor4"};
  const std::array servos{"servo1", "servo2", "servo3", "servo4"};
  for (int i = 0; i < 4; ++i) {
    xrp.HandleWPILibUpdate(
        json::object("type", "XRPMotor", "device", motors[i], "data",
                     json::object("<throttle", i % 2 ? -1.0 : 1.0)));
    xrp.HandleWPILibUpdate(
        json::object("type", "XRPServo", "device", servos[i], "data",
                     json::object("<position", i % 2 ? 1.0 : 0.0)));
  }
  xrp.HandleWPILibUpdate(
      json::object("type", "DIO", "device", "1", "data",
                   json::object("<input", false, "<>value", true)));
  const std::vector<uint8_t> expected{0,    0, 1,   1,    0xff, 0, 0xff,
                                      0xff, 1, 0,   0xff, 0xff, 1, 0,
                                      180,  0, 180, 2,    2};
  CHECK(MakeControl(xrp) == expected);
  const std::vector<uint8_t> rename{0, 1, 1, 0x80, 0, 3, 'X', 'R', 'P'};
  CHECK(MakeControl(xrp, "XRP") == rename);
  auto next = MakeControl(xrp);
  CHECK(next[0] == 0);
  CHECK(next[1] == 2);
}
