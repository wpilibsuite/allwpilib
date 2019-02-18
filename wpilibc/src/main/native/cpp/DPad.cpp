#include "frc/DPad.h"

using namespace frc;
DPad::DPad(GenericHID& joystick, int povNumber) : up(joystick, static_cast<int>(Angles::kUp), povNumber),
                                                  right(joystick, static_cast<int>(Angles::kRight), povNumber),
                                                  down(joystick, static_cast<int>(Angles::kDown), povNumber),
                                                  left(joystick, static_cast<int>(Angles::kLeft), povNumber) {}