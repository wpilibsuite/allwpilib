// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

namespace wpi::detail {
enum class TunableTypeValue {
  UNKNOWN = 0,
  BOOLEAN = 1,
  INT32 = 2,
  INT64 = 3,
  FLOAT = 4,
  DOUBLE = 5,
  STRING = 6,
  RAW = 7,
  BOOLEAN_ARRAY = 8,
  INT32_ARRAY = 9,
  INT64_ARRAY = 10,
  FLOAT_ARRAY = 11,
  DOUBLE_ARRAY = 12,
  STRING_ARRAY = 13,
  STRUCT = 14,
  PROTOBUF = 15,
  COMPLEX = 16,
  MEMBER_BOOLEAN = 17,
  MEMBER_INT32 = 18,
  MEMBER_INT64 = 19,
  MEMBER_FLOAT = 20,
  MEMBER_DOUBLE = 21,
  MEMBER_STRING = 22,
  MEMBER_RAW = 23,
  MEMBER_BOOLEAN_ARRAY = 24,
  MEMBER_INT32_ARRAY = 25,
  MEMBER_INT64_ARRAY = 26,
  MEMBER_FLOAT_ARRAY = 27,
  MEMBER_DOUBLE_ARRAY = 28,
  MEMBER_STRING_ARRAY = 29,
  MEMBER_STRUCT = 30,
  MEMBER_PROTOBUF = 31,
  MEMBER_COMPLEX = 32,
};
}  // namespace wpi::detail
