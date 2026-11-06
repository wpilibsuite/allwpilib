// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/fields/struct/FieldIdStruct.hpp"

namespace {
constexpr size_t ID_OFF = 0;
}  // namespace

using StructType = wpi::util::Struct<wpi::fields::FieldId>;

wpi::fields::FieldId StructType::Unpack(std::span<const uint8_t> data) {
  return wpi::fields::FieldId{
      wpi::util::UnpackStruct<uint32_t, ID_OFF>(data),
  };
}

void StructType::Pack(std::span<uint8_t> data,
                      const wpi::fields::FieldId& value) {
  wpi::util::PackStruct<ID_OFF>(data, std::to_underlying(value));
}
