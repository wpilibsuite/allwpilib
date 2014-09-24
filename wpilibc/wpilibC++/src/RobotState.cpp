#include "RobotState.h"

RobotStateInterface* RobotState::impl = 0;

void RobotState::SetImplementation(RobotStateInterface* i) {
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
