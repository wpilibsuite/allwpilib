#include "RobotState.h"

#include "Base.h"

std::shared_ptr<RobotStateInterface> RobotState::impl;

void RobotState::SetImplementation(RobotStateInterface& i) {
  impl = std::shared_ptr<RobotStateInterface>(
      &i, NullDeleter<RobotStateInterface>());
}

void RobotState::SetImplementation(
    std::shared_ptr<RobotStateInterface> i) {
  impl = i;
}

bool RobotState::IsDisabled() {
  if (impl != nullptr) {
    return impl->IsDisabled();
  }
  return true;
}

bool RobotState::IsEnabled() {
  if (impl != nullptr) {
    return impl->IsEnabled();
  }
  return false;
}

bool RobotState::IsOperatorControl() {
  if (impl != nullptr) {
    return impl->IsOperatorControl();
  }
  return true;
}

bool RobotState::IsAutonomous() {
  if (impl != nullptr) {
    return impl->IsAutonomous();
  }
  return false;
}

bool RobotState::IsTest() {
  if (impl != nullptr) {
    return impl->IsTest();
  }
  return false;
}
