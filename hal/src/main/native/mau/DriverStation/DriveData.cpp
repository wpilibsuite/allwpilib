
#include "DriveData.h"

Mau_DriveData* driveData;

Mau_DriveData::Mau_DriveData(Alliance al, DSInfo info) {
    alliance = al;
    stationInfo = info;
}

void Mau_DriveData::addJoystick(int joyNumber, Joystick joy, JoystickDescriptor desc, JoystickType type,
                                JoystickAxisType axes) {
    Mau_SharedJoystick newStick;
    newStick.joystick = joy;
    newStick.joyDescriptor = desc;
    newStick.joyType = type;
    newStick.joyAxisType = axes;

    joysticks[joyNumber] = newStick;
}

Mau_SharedJoystick Mau_DriveData::getJoystick(int index) {
    if (!(index >= 0 && index < 6)) {
        // Log an Error!
    } else {
        return joysticks[index];
    }
}

Alliance Mau_DriveData::getAlliance() {
    return alliance;
}

DSInfo Mau_DriveData::getDSInfo() {
    return stationInfo;
}

