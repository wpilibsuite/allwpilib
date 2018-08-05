#ifndef NATIVE_MAUFWIDRIVEDATA_H

#include "DriverStationInternal.h"
#include "MauDriveData.h"
#include "ds.fwi.hpp"
#include "shared.fwi.hpp"
#include "DriverStation.h"
#include <vector>
#include <map>

#define NATIVE_MAUFWIDRIVEDATA_H

//// ----- Bagel Data: Utility ----- ////

const static HAL_AllianceStationID Bagel_HALRedIDs[3] = {HAL_AllianceStationID_kRed1,
                                                    HAL_AllianceStationID_kRed2,
                                                    HAL_AllianceStationID_kRed3};
const static HAL_AllianceStationID Bagel_HALBlueIDs[3] = {HAL_AllianceStationID_kBlue1,
                                                     HAL_AllianceStationID_kBlue2,
                                                     HAL_AllianceStationID_kBlue3};

static void Bagel_getIDFromAlliance(HAL_AllianceStationID* id, int station, const HAL_AllianceStationID* idList) {
    HAL_AllianceStationID newID = idList[station];
    id = &newID;
}


static Toast::Memory::RobotState Bagel_controlToState(HAL_ControlWord* halWord) {
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

static HAL_JoystickDescriptor Bagel_convertDescription(Toast::Memory::Shared::DS::Joystick* joy, bool parseAxes) {
    Toast::Memory::Shared::DS::JoystickDescriptor* fwiDesc = joy->get_descriptor();
    HAL_JoystickDescriptor halDesc;
    halDesc.isXbox = fwiDesc->get_is_xbox();
    halDesc.type = (uint8_t) fwiDesc->get_type();
    char* newName = fwiDesc->get_name();
    uint8_t nameLength = fwiDesc->get_name_length();
    for (int index = 0; index < nameLength; index++) {
        halDesc.name[index] = newName[index];
    }

    halDesc.axisCount = joy->get_num_axis();
    halDesc.povCount = joy->get_num_pov();
    halDesc.buttonCount = joy->get_num_button();
    if (parseAxes) {
        for (int index = 0; index < halDesc.axisCount; index) {
            halDesc.axisTypes[index] = (uint8_t) fwiDesc->get_axis_type(index);
        }
    }
    return halDesc;
}

//// ----- Bagel Data: Write ----- ////

static void Bagel_writeJoystick(int joyNumber, Toast::Memory::Shared::DS::Joystick* joy) {
    Toast::Memory::Shared::DS::JoystickDescriptor* newJoyDesc;
    newJoyDesc = joy->get_descriptor();
    HAL_JoystickDescriptor joyDesc = Bagel_convertDescription(joy, false);

    HAL_JoystickAxes joyAxes;
    joyAxes.count = joy->get_num_axis();
    for (int index = 0; index < joyAxes.count; index) {
        joyAxes.axes[index] = joy->get_axis(index);
        joyDesc.axisTypes[index] = (uint8_t) newJoyDesc->get_axis_type(index);
    }

    HAL_JoystickPOVs joyPOVs;
    joyPOVs.count = joy->get_num_pov();
    for (int index = 0; index < joyPOVs.count; index) {
        joyAxes.axes[index] = joy->get_pov(index);
    }

    HAL_JoystickButtons joyButtons;
    joyButtons.count = joy->get_num_button();
    joyButtons.buttons = joy->get_button_mask();

    mau::sharedMemory->writeJoystick(joyNumber, joyAxes, joyPOVs, joyButtons, joyDesc);
}

static void Bagel_writeAllJoysticks(int joyCount, Toast::Memory::Shared::DS::Joystick* newJoys) {
    for (int index = 0; index < joyCount; index++) {
        Bagel_writeJoystick(index, &newJoys[index]);
    }
}

//// ----- Bagel Data: Update ----- ////

static void Bagel_updateAllianceID(Toast::Memory::Shared::DS::Alliance alliance, int pos) {
    HAL_AllianceStationID id;

    switch (alliance) {
        case Toast::Memory::Shared::DS::Alliance::Red:
            Bagel_getIDFromAlliance(&id, pos, Bagel_HALRedIDs);
            break;
        case Toast::Memory::Shared::DS::Alliance::Blue:
            Bagel_getIDFromAlliance(&id, pos, Bagel_HALBlueIDs);
            break;
    }
    
    mau::sharedMemory->updateAllianceID(&id);
}

// --- Update: Joystick --- //

static void Bagel_updateRobotState(Toast::Memory::RobotState roboState) {
    bool enabled = true;
    bool auton = false;
    bool test = false;
    if (roboState == Toast::Memory::RobotState::DISABLED) {
        enabled = false;
    } else {
        if (roboState == Toast::Memory::RobotState::TEST) {
            test = true;
        }
        if (roboState == Toast::Memory::RobotState::AUTO) {
            auton = true;
        }
    }

    mau::sharedMemory->updateIsEnabled(enabled);
    mau::sharedMemory->updateIsAutonomous(auton);
    mau::sharedMemory->updateIsTest(test);
}

static void Bagel_updateJoystickAxes(int joyNumber, int axesNum, uint8_t* axes) {
    for (int index = 0; index < axesNum; index++) {
        mau::sharedMemory->updateJoyAxis(joyNumber, index, (float) axes[index]);
        index++;
    }
}

static void Bagel_updateJoystickPOVs(int joyNumber, int povsNum, uint16_t* povs) {
    for (int index = 0; index < povsNum; index++) {
        mau::sharedMemory->updateJoyPOV(joyNumber, index, povs[index]);
        index++;
    }
}

static void Bagel_updateJoystickButtons(int joyNumber, uint32_t buttonMask) {
    mau::sharedMemory->updateJoyButtons(joyNumber, buttonMask);
}

static void Bagel_updateJoystickDescriptor(int joyNumber, Toast::Memory::Shared::DS::Joystick* joy) {
    HAL_JoystickDescriptor desc = Bagel_convertDescription(joy, true);
    mau::sharedMemory->updateJoyDescriptor(joyNumber, &desc);
}

//// ----- Bagel Data: Read ----- ////

static Toast::Memory::Shared::DS::Alliance Bagel_readAlliance() {
    HAL_AllianceStationID id = *mau::sharedMemory->readAllianceID();
    Toast::Memory::Shared::DS::Alliance output;
    switch (id) {
        case HAL_AllianceStationID_kRed1:
            output = Toast::Memory::Shared::DS::Alliance::Red;
            break;
        case HAL_AllianceStationID_kRed2:
            output = Toast::Memory::Shared::DS::Alliance::Red;
            break;
        case HAL_AllianceStationID_kRed3:
            output = Toast::Memory::Shared::DS::Alliance::Red;
            break;
        case HAL_AllianceStationID_kBlue1:
            output = Toast::Memory::Shared::DS::Alliance::Blue;
            break;
        case HAL_AllianceStationID_kBlue2:
            output = Toast::Memory::Shared::DS::Alliance::Blue;
            break;
        case HAL_AllianceStationID_kBlue3:
            output = Toast::Memory::Shared::DS::Alliance::Blue;
            break;
    }
    return output;
}

static Toast::Memory::Shared::DS::DSInfo Bagel_readDSInfo() {
    HAL_ControlWord* halWord = mau::sharedMemory->readControlWord();
    Toast::Memory::Shared::DS::DSInfo dsInfo;

    dsInfo.set_ds_attached(halWord->dsAttached);
//    dsInfo.set_new_control_data();
    dsInfo.set_fms_attached(halWord->fmsAttached);
//    dsInfo.set_system_active()
//    dsInfo.set_match_time()
    dsInfo.set_alliance(Bagel_readAlliance());

    return dsInfo;
}

static Toast::Memory::RobotState Bagel_readRobotState() {
    return Bagel_controlToState(mau::sharedMemory->readControlWord());
}

#endif //NATIVE_MAUFWIDRIVEDATA_H
