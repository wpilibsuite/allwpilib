#include "frc/SpeedController.h"

using namespace frc;

void SpeedController::SetVoltage(units::volt_t output) {
  Set(output / units::volt_t(RobotController::GetInputVoltage()));
}