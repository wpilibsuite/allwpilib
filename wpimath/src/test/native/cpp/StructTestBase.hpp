// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <cstring>

#include <catch2/catch_test_macros.hpp>

#include "wpi/util/struct/Struct.hpp"

template <typename T>
class StructTest {};

#define CATCH_TYPED_TEST_SUITE_P(Suite)

#define CATCH_TYPED_TEST_P(Suite, Name) \
  template <typename TypeParam>         \
  void Suite##_##Name()

#define REGISTER_CATCH_TYPED_TEST_SUITE_P(Suite, ...)

#define INSTANTIATE_CATCH_TYPED_TEST_SUITE_P(Prefix, Suite, TypeParam) \
  TEST_CASE(#Suite " " #Prefix " RoundTrip", "[wpimath]") {            \
    Suite##_RoundTrip<TypeParam>();                                    \
  }                                                                    \
  TEST_CASE(#Suite " " #Prefix " DoublePack", "[wpimath]") {           \
    Suite##_DoublePack<TypeParam>();                                   \
  }                                                                    \
  TEST_CASE(#Suite " " #Prefix " DoubleUnpack", "[wpimath]") {         \
    Suite##_DoubleUnpack<TypeParam>();                                 \
  }

// For these tests:
// TypeParam defines Type, TEST_DATA, and CheckEq
// Type is the data type
// StructType is the instantiation of wpi::util::Struct<>

CATCH_TYPED_TEST_P(StructTest, RoundTrip) {
  using Type = typename TypeParam::Type;
  using StructType = wpi::util::Struct<Type>;
  uint8_t buffer[StructType::GetSize()];
  std::memset(buffer, 0, StructType::GetSize());
  wpi::util::PackStruct(buffer, TypeParam::TEST_DATA);

  Type unpacked_data = wpi::util::UnpackStruct<Type>(buffer);
  TypeParam::CheckEq(TypeParam::TEST_DATA, unpacked_data);
}

CATCH_TYPED_TEST_P(StructTest, DoublePack) {
  using Type = typename TypeParam::Type;
  using StructType = wpi::util::Struct<Type>;
  uint8_t buffer[StructType::GetSize()];
  std::memset(buffer, 0, StructType::GetSize());
  wpi::util::PackStruct(buffer, TypeParam::TEST_DATA);
  wpi::util::PackStruct(buffer, TypeParam::TEST_DATA);

  Type unpacked_data = wpi::util::UnpackStruct<Type>(buffer);
  TypeParam::CheckEq(TypeParam::TEST_DATA, unpacked_data);
}

CATCH_TYPED_TEST_P(StructTest, DoubleUnpack) {
  using Type = typename TypeParam::Type;
  using StructType = wpi::util::Struct<Type>;
  uint8_t buffer[StructType::GetSize()];
  std::memset(buffer, 0, StructType::GetSize());
  wpi::util::PackStruct(buffer, TypeParam::TEST_DATA);

  {
    Type unpacked_data = wpi::util::UnpackStruct<Type>(buffer);
    TypeParam::CheckEq(TypeParam::TEST_DATA, unpacked_data);
  }

  {
    Type unpacked_data = wpi::util::UnpackStruct<Type>(buffer);
    TypeParam::CheckEq(TypeParam::TEST_DATA, unpacked_data);
  }
}

REGISTER_CATCH_TYPED_TEST_SUITE_P(StructTest, RoundTrip, DoublePack,
                                  DoubleUnpack);
