#ifndef NATIVE_DRIVEDATA_H

#include "DriverStation/BagelCPP/ds.fwi.hpp"
#include <vector>

#define NATIVE_DRIVEDATA_H

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
    Mau_DriveData(Alliance al, DSInfo info);
    void addJoystick(int joyNumber, Joystick joy, JoystickDescriptor desc, JoystickType type, JoystickAxisType axes);
    Mau_SharedJoystick getJoystick(int index);
    Alliance getAlliance();
    DSInfo getDSInfo();

};

extern Mau_DriveData* driveData;

#endif //NATIVE_DRIVEDATA_H
