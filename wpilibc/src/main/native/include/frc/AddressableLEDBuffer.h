/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <array>

#include "frc/AddressableLED.h"

namespace frc {
template <size_t N>
class AddressableLEDBuffer : public std::array<AddressableLED::LEDData, N> {
 public:
  constexpr int GetLength() const noexcept { return this->size(); }

  constexpr void SetLED(size_t index, int r, int g, int b) {
    this->operator[](index).SetLED(r, g, b);
  }
};
}  // namespace frc
