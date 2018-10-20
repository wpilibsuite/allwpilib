
#pragma once

#include "frc/GenericHID.h"
#include "frc/buttons/Button.h"

namespace frc {
class POVButton : public Button {

 public:
  POVButton(GenericHID* joystick, int angle, int povNumber);
  virtual ~POVButton() = default;

  POVButton(POVButton&&) = default;
  POVButton& operator=(POVButton&&) = default;

  virtual bool Get();
 private:
  int m_angle;
  int m_povNumber;
  GenericHID* m_joystick;
};
}
