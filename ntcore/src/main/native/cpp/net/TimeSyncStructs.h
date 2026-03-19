// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once
#include <stdint.h>

#include <wpi/util/struct/Struct.hpp>

namespace wpi::tsp {

struct TspPing {
  uint8_t version;
  uint8_t message_id;
  uint64_t client_time;
};

struct TspPong : public TspPing {
  TspPong(TspPing ping, uint64_t servertime)
      : TspPing{ping}, server_time{servertime} {}
  uint64_t server_time;
};

}  // namespace wpi::tsp

template <>
struct wpi::util::Struct<wpi::tsp::TspPing> {
  static constexpr std::string_view GetTypeName() { return "TspPing"; }
  static constexpr size_t GetSize() { return 10; }
  static constexpr std::string_view GetSchema() {
    return "uint8 version;uint8 message_id;uint64 client_time";
  }

  static wpi::tsp::TspPing Unpack(std::span<const uint8_t> data) {
    return wpi::tsp::TspPing{
        wpi::util::UnpackStruct<uint8_t, 0>(data),
        wpi::util::UnpackStruct<uint8_t, 1>(data),
        wpi::util::UnpackStruct<uint64_t, 2>(data),
    };
  }
  static void Pack(std::span<uint8_t> data, const wpi::tsp::TspPing& value) {
    wpi::util::PackStruct<0>(data, value.version);
    wpi::util::PackStruct<1>(data, value.message_id);
    wpi::util::PackStruct<2>(data, value.client_time);
  }
};

template <>
struct wpi::util::Struct<wpi::tsp::TspPong> {
  static constexpr std::string_view GetTypeName() { return "TspPong"; }
  static constexpr size_t GetSize() { return 18; }
  static constexpr std::string_view GetSchema() {
    return "uint8 version;uint8 message_id;uint64 client_time;uint64_t "
           "server_time";
  }

  static wpi::tsp::TspPong Unpack(std::span<const uint8_t> data) {
    return wpi::tsp::TspPong{
        wpi::tsp::TspPing{
            wpi::util::UnpackStruct<uint8_t, 0>(data),
            wpi::util::UnpackStruct<uint8_t, 1>(data),
            wpi::util::UnpackStruct<uint64_t, 2>(data),
        },
        wpi::util::UnpackStruct<uint64_t, 10>(data),
    };
  }
  static void Pack(std::span<uint8_t> data, const wpi::tsp::TspPong& value) {
    wpi::util::PackStruct<0>(data, value.version);
    wpi::util::PackStruct<1>(data, value.message_id);
    wpi::util::PackStruct<2>(data, value.client_time);
    wpi::util::PackStruct<10>(data, value.server_time);
  }
};

static_assert(wpi::util::StructSerializable<wpi::tsp::TspPong>);
static_assert(wpi::util::StructSerializable<wpi::tsp::TspPing>);
