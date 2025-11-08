// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <span>

#include "wpi/glass/Model.hpp"
#include "wpi/util/function_ref.hpp"

namespace wpi::util {
template <typename T>
class SmallVectorImpl;
}  // namespace wpi

namespace wpi::glass {

class LEDDisplayModel : public wpi::glass::Model {
 public:
  struct Data {
    uint8_t r;
    uint8_t g;
    uint8_t b;
  };

  virtual std::span<const Data> GetData(wpi::util::SmallVectorImpl<Data>& buf) = 0;
};

class LEDDisplaysModel : public wpi::glass::Model {
 public:
  virtual size_t GetNumLEDDisplays() = 0;

  virtual void ForEachLEDDisplay(
      wpi::util::function_ref<void(LEDDisplayModel& model, int index)> func) = 0;
};

void DisplayLEDDisplay(LEDDisplayModel* model, int index);
void DisplayLEDDisplays(LEDDisplaysModel* model);

}  // namespace wpi::glass
