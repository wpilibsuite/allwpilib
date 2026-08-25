// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/controller/struct/ArmFeedforwardStruct.hpp"

namespace {
constexpr size_t KS_OFF = 0;
constexpr size_t KG_OFF = KS_OFF + 8;
constexpr size_t KV_OFF = KG_OFF + 8;
constexpr size_t KA_OFF = KV_OFF + 8;
}  // namespace

using StructType = wpi::util::Struct<wpi::math::ArmFeedforward>;

wpi::math::ArmFeedforward StructType::Unpack(std::span<const uint8_t> data) {
  return wpi::math::ArmFeedforward{
      wpi::units::volts<>{wpi::util::UnpackStruct<double, KS_OFF>(data)},
      wpi::units::volts<>{wpi::util::UnpackStruct<double, KG_OFF>(data)},
      wpi::units::unit<wpi::math::ArmFeedforward::kv_unit>{
          wpi::util::UnpackStruct<double, KV_OFF>(data)},
      wpi::units::unit<wpi::math::ArmFeedforward::ka_unit>{
          wpi::util::UnpackStruct<double, KA_OFF>(data)},
  };
}

void StructType::Pack(std::span<uint8_t> data,
                      const wpi::math::ArmFeedforward& value) {
  wpi::util::PackStruct<KS_OFF>(data, value.GetKs().value());
  wpi::util::PackStruct<KG_OFF>(data, value.GetKg().value());
  wpi::util::PackStruct<KV_OFF>(data, value.GetKv().value());
  wpi::util::PackStruct<KA_OFF>(data, value.GetKa().value());
}
