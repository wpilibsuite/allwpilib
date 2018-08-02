#ifndef NATIVE_MAUFWIDRIVEDATA_H

#include "DriverStationInternal.h"
#include "MauDriveData.h"
#include "ds.fwi.hpp"
#include "shared.fwi.hpp"
#include <vector>

#define NATIVE_MAUFWIDRIVEDATA_H

// ----- Bagel Data: Write ----- //

static void Bagel_writeJoystick(int joyNumber, Toast::Memory::Shared::DS::Joystick* newJoy) {
    Toast::Memory::Shared::DS::JoystickDescriptor* newJoyDesc;
    newJoyDesc = newJoy->get_descriptor();

    HAL_JoystickDescriptor joyDesc;
    joyDesc.isXbox = newJoyDesc->get_is_xbox();
    joyDesc.type = (uint8_t) newJoyDesc->get_type();
    char* newName = newJoyDesc->get_name();
    uint8_t nameLength = newJoyDesc->get_name_length();
    for (int index = 0; index < nameLength; index++) {
        joyDesc.name[index] = newName[index];
    }

    HAL_JoystickAxes joyAxes;
    joyAxes.count = newJoy->get_num_axis();
    joyDesc.axisCount = joyAxes.count;
    for (int index = 0; index < joyAxes.count; index) {
        joyAxes.axes[index] = newJoy->get_axis(index);
        joyDesc.axisTypes[index] = (uint8_t) newJoyDesc->get_axis_type(index);
    }

    HAL_JoystickPOVs joyPOVs;
    joyPOVs.count = newJoy->get_num_pov();
    joyDesc.povCount = joyPOVs.count;
    for (int index = 0; index < joyPOVs.count; index) {
        joyAxes.axes[index] = newJoy->get_pov(index);
    }

    HAL_JoystickButtons joyButtons;
    joyButtons.count = newJoy->get_num_button();
    joyDesc.buttonCount = joyButtons.count;
    joyButtons.buttons = newJoy->get_button_mask();

    mau::sharedMemory->writeJoystick(joyNumber, joyAxes, joyPOVs, joyButtons, joyDesc);
}

static void Bagel_writeAllJoysticks(int joyCount, Toast::Memory::Shared::DS::Joystick* newJoys) {
    for(int index = 0; index < joyCount; index++) {
        Bagel_writeJoystick(index, &newJoys[index]);
    }
}

// ----- Bagel Data: Update ----- //

static void Bagel_updateRobotState(Toast::Memory::RobotState roboState, Toast::Memory::Shared::DS::DSInfo info) {
    bool enabled = true;
    bool auton = false;
    bool test = false;
    if(roboState == Toast::Memory::RobotState::DISABLED) {
        enabled = false;
    } else {
        if(roboState == Toast::Memory::RobotState::TEST) {
            test = true;
        }
        if(roboState == Toast::Memory::RobotState::AUTO) {
            auton = true;
        }
    }

    bool estop = false; // I have no idea what this is
    bool fms = info.get_fms_attached();
    bool ds = info.get_ds_attached();
    mau::sharedMemory->updateControlWord(enabled, auton, test, estop, fms, ds);
}

static void Bagel_updateJoystickAxes(int joyNumber, std::vector<int8_t> axes) {
    int index = 0;
    for (int8_t axis : axes) {
        mau::sharedMemory->updateJoyAxis(joyNumber, index, (float) axis);
        index++;
    }
}

static void Bagel_updateJoystickPOVs(int joyNumber, std::vector<int16_t> povs) {
    int index = 0;
    for (int16_t pov : povs) {
        mau::sharedMemory->updateJoyPOV(joyNumber, index, pov);
        index++;
    }
}

static void Bagel_updateJoystickButtons(int joyNumber, uint32_t buttonMask) {
    mau::sharedMemory->updateJoyButtons(joyNumber, buttonMask);
}

// ----- Bagel Data: Read ----- //}

Toast::Memory::Shared::DS::JoystickDescriptor Bagel_readJoyDescriptor(int joyNumber);

Toast::Memory::Shared::DS::Alliance Bagel_readAlliance() {
    HAL_AllianceStationID id = *mau::sharedMemory->readAllianceID();
    switch (id) {
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
    dsInfo.set_new_control_data(mau::sharedMemory->readIfControlRead());
    dsInfo.set_fms_attached(halWord->fmsAttached);
//    dsInfo.set_system_active()
//    dsInfo.set_match_time()
    dsInfo.set_alliance(Bagel_readAlliance());

    return dsInfo;
}

Toast::Memory::RobotState Bagel_controlToState(HAL_ControlWord* halWord) {
    if (!halWord->enabled) {
        return Toast::Memory::RobotState::DISABLED;
    } else if (halWord->test) {
        return Toast::Memory::RobotState::TEST;
    } else if (halWord->autonomous) {
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
