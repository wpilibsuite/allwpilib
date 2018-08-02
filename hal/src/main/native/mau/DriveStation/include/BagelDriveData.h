#ifndef NATIVE_MAUFWIDRIVEDATA_H

#include "DriverStationInternal.h"
#include "MauDriveData.h"
#include "ds.fwi.hpp"
#include "shared.fwi.hpp"

#define NATIVE_MAUFWIDRIVEDATA_H

static void Bagel_addJoystick(Mau_DriveData, int joyNumber, Toast::Memory::Shared::DS::Joystick joy,
                       Toast::Memory::Shared::DS::JoystickDescriptor desc,
                       Toast::Memory::Shared::DS::JoystickType type,
                       Toast::Memory::Shared::DS::JoystickAxisType axes) {

}

HAL_JoystickAxes Bagel_readJoyAxes(int joyNumber) {

}

HAL_JoystickPOVs Bagel_readJoyPOVs(int joyNumber);

HAL_JoystickButtons Bagel_readJoyButtons(int joyNumber);

Toast::Memory::Shared::DS::JoystickDescriptor Bagel_readJoyDescriptor(int joyNumber);

Toast::Memory::Shared::DS::Alliance Bagel_readAlliance() {
    HAL_AllianceStationID id = *mau::sharedMemory->readAllianceID();
    switch(id) {
        case HAL_AllianceStationID_kRed1:
            return Toast::Memory::Shared::DS::Alliance::Red;
        case HAL_AllianceStationID_kRed2:
            return Toast::Memory::Shared::DS::Alliance::Red;
        case HAL_AllianceStationID_kRed3:
            return Toast::Memory::Shared::DS::Alliance::Red;
        case HAL_AllianceStationID_kBlue1:
            return Toast::Memory::Shared::DS::Alliance::Blue;
        case HAL_AllianceStationID_kBlue2:
            return Toast::Memory::Shared::DS::Alliance::Blue;
        case HAL_AllianceStationID_kBlue3:
            return Toast::Memory::Shared::DS::Alliance::Blue;
    }
}

Toast::Memory::Shared::DS::DSInfo Bagel_readDSInfo() {
    HAL_ControlWord* halWord = mau::sharedMemory->readCurrentControl();
    Toast::Memory::Shared::DS::DSInfo dsInfo;

    dsInfo.set_ds_attached(halWord->dsAttached);
    dsInfo.set_new_control_data(mau::sharedMemory->readControlUpdated());
    dsInfo.set_fms_attached(halWord->fmsAttached);
//    dsInfo.set_system_active()
//    dsInfo.set_match_time()
    dsInfo.set_alliance(Bagel_readAlliance());

    return dsInfo;
}

Toast::Memory::RobotState Bagel_controlToState(HAL_ControlWord* halWord) {
    if(!halWord->enabled) {
        return Toast::Memory::RobotState::DISABLED;
    } else if(halWord->test) {
        return Toast::Memory::RobotState::TEST;
    } else if(halWord ->autonomous) {
        return Toast::Memory::RobotState::AUTO;
    } else {
        return Toast::Memory::RobotState::TELEOP;
    }
}

Toast::Memory::RobotState Bagel_readPreviousState() {
    return Bagel_controlToState(mau::sharedMemory->readPreviousControl());
}

Toast::Memory::RobotState Bagel_readCurrentState() {
    return Bagel_controlToState(mau::sharedMemory->readCurrentControl());
}


#endif //NATIVE_MAUFWIDRIVEDATA_H
