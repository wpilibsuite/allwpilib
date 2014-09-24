#include "RobotState.h"

RobotStateInterface* RobotState::impl = 0;

void RobotState::SetImplementation(RobotStateInterface* i) {
    impl = i;
}

bool RobotState::IsDisabled() {
    if (impl != 0) {
        return impl->IsDisabled();
    }
    return true;
}

bool RobotState::IsEnabled() {
    if (impl != 0) {
        return impl->IsEnabled();
    }
    return false;
}

bool RobotState::IsOperatorControl() {
    if (impl != 0) {
        return impl->IsOperatorControl();
    }
    return true;
}

bool RobotState::IsAutonomous() {
    if (impl != 0) {
        return impl->IsAutonomous();
    }
    return false;
}

bool RobotState::IsTest() {
    if (impl != 0) {
        return impl->IsTest();
    }
    return false;
}
