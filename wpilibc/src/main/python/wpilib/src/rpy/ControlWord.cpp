#include "rpy/ControlWord.h"

#include <hal/DriverStation.h>

namespace rpy {

std::tuple<bool, bool, bool> GetControlState() {
  HAL_ControlWord controlWord;
  HAL_GetControlWord(&controlWord);

  bool enable = controlWord.enabled != 0 && controlWord.dsAttached != 0;
  bool auton = controlWord.autonomous != 0;
  bool test = controlWord.test != 0;

  return std::make_tuple(enable, auton, test);
}

} // namespace rpy
