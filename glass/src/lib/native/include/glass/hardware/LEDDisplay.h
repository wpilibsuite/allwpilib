/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <wpi/ArrayRef.h>
#include <wpi/STLExtras.h>

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

  virtual wpi::ArrayRef<Data> GetData(wpi::SmallVectorImpl<Data>& buf) = 0;
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
