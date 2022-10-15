// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <span>

#include <wpi/function_ref.h>

#include "glass/Model.h"

namespace wpi {
template <typename T>
class SmallVectorImpl;
}  // namespace wpi

namespace glass {

class LEDDisplayModel : public glass::Model {
 public:
  struct Data {
    uint8_t b;
    uint8_t g;
    uint8_t r;
    uint8_t padding;
  };

  virtual bool IsRunning() = 0;

  virtual std::span<const Data> GetData(wpi::SmallVectorImpl<Data>& buf) = 0;
};

class LEDDisplaysModel : public glass::Model {
 public:
  virtual size_t GetNumLEDDisplays() = 0;

  virtual void ForEachLEDDisplay(
      wpi::function_ref<void(LEDDisplayModel& model, int index)> func) = 0;
};

void DisplayLEDDisplay(LEDDisplayModel* model, int index);
void DisplayLEDDisplays(LEDDisplaysModel* model);

}  // namespace glass
