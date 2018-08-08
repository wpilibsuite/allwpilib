#ifndef NATIVE_DRIVEDATA_H

#include "DriverStation.h"
#include <wpi/priority_mutex.h>
#include <wpi/priority_condition_variable.h>

#define NATIVE_DRIVEDATA_H

struct Mau_SharedJoystick {
    HAL_JoystickAxes* joyAxes;
    HAL_JoystickPOVs* joyPOVs;
    HAL_JoystickButtons* joyButtons;
    HAL_JoystickDescriptor* joyDescriptor;

    int32_t joystickNum;
    int64_t outputs;
    int32_t leftRumble;
    int32_t rightRumble;
};

class Mau_DriveData {
    wpi::priority_mutex* memLock;
    wpi::priority_condition_variable* memSignal;

    HAL_AllianceStationID allianceID;
    HAL_MatchInfo* matchInfo;
    HAL_ControlWord* controlWord;
    Mau_SharedJoystick joysticks[6];

    void unlockAndSignal();
    Mau_SharedJoystick getJoystick(int joyNumber);
public:
    Mau_DriveData();
    void writeJoystick(int joyNumber,
                       HAL_JoystickAxes axes,
                       HAL_JoystickPOVs povs,
                       HAL_JoystickButtons buttons,
                       HAL_JoystickDescriptor desc);

    void updateAllianceID(HAL_AllianceStationID id);
    void updateMatchInfo(HAL_MatchInfo* info);
    void updateMatchType(HAL_MatchType* type);

    void updateIsEnabled(bool enabled);
    void updateIsAutonomous(bool auton);
    void updateIsTest(bool test);
    void updateEStop(bool eStop);
    void updateIsFmsAttached(bool fms);
    void updateIsDsAttached(bool ds);
    void updateControlWord(bool enabled, bool auton, bool test, bool estop, bool fms, bool ds);

    void updateJoyAxis(int joyNumber, int16_t axisCount, uint8_t* axes);
    void updateJoyPOV(int joyNumber, int povsCount, uint16_t* povs);
    void updateJoyButtons(int joyNumber, uint8_t buttonCount, uint32_t buttons);
    void updateJoyDescriptor(int joyNumber, HAL_JoystickDescriptor* desc);
    void updateJoyOutputs(int32_t joyNumber, int64_t outputs, int32_t leftRumble, int32_t rightRumble);

    HAL_AllianceStationID* readAllianceID();
    HAL_MatchInfo* readMatchInfo();
    HAL_MatchType* readMatchType();

    bool readIsEnabled();
    bool readIsAutonomous();
    bool readIsTest();
    bool readEStop();
    bool readIsFmsAttached();
    bool readIsDsAttached();
    HAL_ControlWord* readControlWord();

    HAL_JoystickAxes* readJoyAxes(int joyNumber);
    HAL_JoystickPOVs* readJoyPOVs(int joyNumber);
    HAL_JoystickButtons* readJoyButtons(int joyNumber);
    HAL_JoystickDescriptor* readJoyDescriptor(int joyNumber);

    wpi::priority_mutex* getMutex();
    wpi::priority_condition_variable* getDataSignal();
};

#endif //NATIVE_DRIVEDATA_H