#ifndef NATIVE_DRIVEDATA_H

#include "DriverStation.h"
#include <wpi/priority_mutex.h>

#define NATIVE_DRIVEDATA_H

struct Mau_SharedJoystick {
    HAL_JoystickAxes* joyAxes;
    HAL_JoystickPOVs* joyPOVs;
    HAL_JoystickButtons* joyButtons;
    HAL_JoystickDescriptor* joyDescriptor;
};

class Mau_DriveData {
    Mau_SharedJoystick joysticks[6];
    HAL_ControlWord* prevControlWord;
    HAL_ControlWord* currControlWord;
    bool wordUnread;
    HAL_AllianceStationID* allianceID;
    HAL_MatchInfo* matchInfo;
    wpi::priority_mutex* memLock;

    void createMemoryMutex();
    Mau_SharedJoystick getJoystick(int joyNumber);
public:
    Mau_DriveData();
    Mau_DriveData(HAL_ControlWord* word, HAL_AllianceStationID* id, HAL_MatchInfo* info);
    void writeJoystick(int joyNumber,
                       HAL_JoystickAxes axes,
                       HAL_JoystickPOVs povs,
                       HAL_JoystickButtons buttons,
                       HAL_JoystickDescriptor desc);

    void updateControlWord(bool enabled, bool auton, bool test, bool estop, bool fms, bool ds);
    void updateJoyAxis(int joyNumber, int axisIndex, float axis);
    void updateJoyPOV(int joyNumber, int povIndex, uint16_t pov);
    void updateJoyButtons(int joyNumber, uint32_t buttons);

    HAL_ControlWord* readPreviousControl();
    HAL_ControlWord* readCurrentControl();
    bool readIfControlRead();
    HAL_AllianceStationID* readAllianceID();
    HAL_MatchInfo* readMatchInfo();
    HAL_MatchType* readMatchType();
    HAL_JoystickAxes* readJoyAxes(int joyNumber);
    HAL_JoystickPOVs* readJoyPOVs(int joyNumber);
    HAL_JoystickButtons* readJoyButtons(int joyNumber);
    HAL_JoystickDescriptor* readJoyDescriptor(int joyNumber);
};

#endif //NATIVE_DRIVEDATA_H