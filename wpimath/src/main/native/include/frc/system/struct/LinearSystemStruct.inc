// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "frc/system/struct/LinearSystemStruct.h"

template <int States, int Inputs, int Outputs>
frc::LinearSystem<States, Inputs, Outputs>
wpi::Struct<frc::LinearSystem<States, Inputs, Outputs>>::Unpack(
    std::span<const uint8_t> data) {
  constexpr size_t kAOff = 0;
  constexpr size_t kBOff =
      kAOff + wpi::GetStructSize<frc::Matrixd<States, States>>();
  constexpr size_t kCOff =
      kBOff + wpi::GetStructSize<frc::Matrixd<States, Inputs>>();
  constexpr size_t kDOff =
      kCOff + wpi::GetStructSize<frc::Matrixd<Outputs, States>>();
  return frc::LinearSystem<States, Inputs, Outputs>{
      wpi::UnpackStruct<frc::Matrixd<States, States>, kAOff>(data),
      wpi::UnpackStruct<frc::Matrixd<States, Inputs>, kBOff>(data),
      wpi::UnpackStruct<frc::Matrixd<Outputs, States>, kCOff>(data),
      wpi::UnpackStruct<frc::Matrixd<Outputs, Inputs>, kDOff>(data)};
}

template <int States, int Inputs, int Outputs>
void wpi::Struct<frc::LinearSystem<States, Inputs, Outputs>>::Pack(
    std::span<uint8_t> data,
    const frc::LinearSystem<States, Inputs, Outputs>& value) {
  constexpr size_t kAOff = 0;
  constexpr size_t kBOff =
      kAOff + wpi::GetStructSize<frc::Matrixd<States, States>>();
  constexpr size_t kCOff =
      kBOff + wpi::GetStructSize<frc::Matrixd<States, Inputs>>();
  constexpr size_t kDOff =
      kCOff + wpi::GetStructSize<frc::Matrixd<Outputs, States>>();
  wpi::PackStruct<kAOff>(data, value.A());
  wpi::PackStruct<kBOff>(data, value.B());
  wpi::PackStruct<kCOff>(data, value.C());
  wpi::PackStruct<kDOff>(data, value.D());
}
