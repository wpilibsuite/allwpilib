#pragma once

#include <frc/Joystick.h>
#include <frc/buttons/POVButton.h>

namespace frc {
class DPad {
  public:
    DPad(GenericHID& joystick, int povNumber = 0);
    virtual ~DPad() = default;

    DPad(DPad&&) = default;
    DPad& operator=(DPad&&) = default;

    frc::POVButton up;
    frc::POVButton right;
    frc::POVButton down;
    frc::POVButton left;
  private:
    enum class Angles {
      kUp = 0,
      kRight = 90,
      kDown = 180,
      kLeft = 270,
    };
};
}