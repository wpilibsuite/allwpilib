#ifndef NATIVE_DRIVEDATA_H

#include "DriverStation.h"
#include <wpi/priority_mutex.h>
#include <wpi/priority_condition_variable.h>

#define NATIVE_DRIVEDATA_H

struct Mau_SharedJoystick {
	bool initd;
    HAL_JoystickAxes joyAxes;
    HAL_JoystickPOVs joyPOVs;
    HAL_JoystickButtons joyButtons;
    HAL_JoystickDescriptor joyDescriptor;
};

class Mau_DriveData {
    static wpi::priority_mutex memLock;
    static wpi::priority_condition_variable memSignal;

    static HAL_AllianceStationID allianceID;
    static HAL_MatchInfo matchInfo;
    static HAL_ControlWord controlWord;
    static Mau_SharedJoystick joysticks[6];

    static void unlockAndSignal();
    static Mau_SharedJoystick getJoystick(int joyNumber);
public:

    static void updateAllianceID(HAL_AllianceStationID id);
    static void updateMatchInfo(HAL_MatchInfo* info);
    static void updateMatchType(HAL_MatchType type);

    static void updateIsEnabled(bool enabled);
    static void updateIsAutonomous(bool auton);
    static void updateIsTest(bool test);
    static void updateEStop(bool eStop);
    static void updateIsFmsAttached(bool fms);
    static void updateIsDsAttached(bool ds);
    static void updateControlWord(bool enabled, bool auton, bool test, bool estop, bool fms, bool ds);

    static void updateJoyAxis(int joyNumber, int16_t axisCount, uint8_t* axes);
    static void updateJoyPOV(int joyNumber, int povsCount, uint16_t* povs);
    static void updateJoyButtons(int joyNumber, uint8_t buttonCount, uint32_t buttons);
    static void updateJoyDescriptor(int joyNumber, HAL_JoystickDescriptor* desc);
    static void updateJoyOutputs(int32_t joyNumber, int64_t outputs, int32_t leftRumble, int32_t rightRumble);

    static HAL_AllianceStationID* readAllianceID();
    static HAL_MatchInfo* readMatchInfo();
    static HAL_MatchType* readMatchType();

    static bool readIsEnabled();
    static bool readIsAutonomous();
    static bool readIsTest();
    static bool readEStop();
    static bool readIsFmsAttached();
    static bool readIsDsAttached();
    static HAL_ControlWord* readControlWord();

    static HAL_JoystickAxes* readJoyAxes(int joyNumber);
    static HAL_JoystickPOVs* readJoyPOVs(int joyNumber);
    static HAL_JoystickButtons* readJoyButtons(int joyNumber);
    static HAL_JoystickDescriptor* readJoyDescriptor(int joyNumber);

    static wpi::priority_mutex* getMutex();
    static wpi::priority_condition_variable* getDataSignal();
};

#endif //NATIVE_DRIVEDATA_H
