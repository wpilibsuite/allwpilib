#ifndef NATIVE_DRIVEDATA_H
#define NATIVE_DRIVEDATA_H

#include "DriverStation/BagelCPP/ds.fwi.hpp"
#include <vector>

using namespace Toast::Memory::Shared::DS;

struct Mau_SharedJoystick {
    Joystick joystick;
    JoystickDescriptor joyDescriptor;
    JoystickType joyType;
    JoystickAxisType joyAxisType;
};

class Mau_DriveData {
    Mau_SharedJoystick joysticks[6];

    Alliance alliance;
    DSInfo stationInfo;

public:
    Mau_DriveData(Alliance al, DSInfo info) {
        alliance = al;
        stationInfo = info;
    }

    void addJoystick(int joyNumber, Joystick joy, JoystickDescriptor desc, JoystickType type, JoystickAxisType axes) {
        Mau_SharedJoystick newStick;
        newStick.joystick = joy;
        newStick.joyDescriptor = desc;
        newStick.joyType = type;
        newStick.joyAxisType = axes;

        joysticks[joyNumber] = newStick;
    }

    Mau_SharedJoystick getJoystick(int index) {
        if(!(index >= 0 && index < 6)) {
            // Log an Error!
        } else {
            return joysticks[index];
        }
    }

    Alliance getAlliance() {
        return alliance;
    }

    DSInfo getDSInfo() {
        return stationInfo;
    }

};

class Mau_DataStack {
    std::vector<Mau_DriveData> dataStack;

public:
    Mau_DataStack() {
        dataStack = std::vector<Mau_DriveData>();
    }

    void push(Mau_DriveData data) {
        dataStack.push_back(data);
    }

    Mau_DriveData top() {
        return dataStack.back();
    }

    Mau_DriveData get(int index) {
        return dataStack[index];
    }
};

#endif //NATIVE_DRIVEDATA_H
