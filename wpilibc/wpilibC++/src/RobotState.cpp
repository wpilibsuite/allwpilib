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
        impl->IsEnabled();
    }
    return false;
}

bool RobotState::IsOperatorControl() {
    if (impl != 0) {
        impl->IsOperatorControl();
    }
    return true;
}

bool RobotState::IsAutonomous() {
    if (impl != 0) {
        impl->IsAutonomous();
    }
    return false;
}

bool RobotState::IsTest() {
    if (impl != 0) {
        impl->IsTest();
    }
    return false;
}
