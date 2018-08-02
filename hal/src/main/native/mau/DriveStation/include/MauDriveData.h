#ifndef NATIVE_DRIVEDATA_H

#include "DriverStation.h"

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
    bool wordUpdated;
    HAL_AllianceStationID* allianceID;
    HAL_MatchInfo* matchInfo;

    Mau_SharedJoystick getJoystick(int joyNumber);
public:
    Mau_DriveData(HAL_ControlWord* word, HAL_AllianceStationID* id, HAL_MatchInfo* info);
    void HAL_writeJoystick(int joyNumber,
                           HAL_JoystickAxes axes,
                           HAL_JoystickPOVs povs,
                           HAL_JoystickButtons buttons,
                           HAL_JoystickDescriptor desc);
    HAL_ControlWord* readPreviousControl();
    HAL_ControlWord* readCurrentControl();
    bool readControlUpdated();
    HAL_AllianceStationID* readAllianceID();
    HAL_MatchInfo* readMatchInfo();
    HAL_MatchType* readMatchType();
    HAL_JoystickAxes* readJoyAxes(int joyNumber);
    HAL_JoystickPOVs* readJoyPOVs(int joyNumber);
    HAL_JoystickButtons* readJoyButtons(int joyNumber);
    HAL_JoystickDescriptor* readJoyDescriptor(int joyNumber);
};

#endif //NATIVE_DRIVEDATA_H