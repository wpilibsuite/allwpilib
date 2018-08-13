#ifndef NATIVE_DRIVEDATA_H

#include "DriverStation.h"
#include <wpi/priority_mutex.h>
#include <wpi/priority_condition_variable.h>

#define NATIVE_DRIVEDATA_H

#define Mau_kMatchNameLength 100
#define Mau_kMatchMessageLength 200
#define Mau_kJoystickNameLength 60

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

    static void updateJoyAxis(int joyNumber, int16_t axisCount, uint8_t* axes);
    static void updateJoyPOV(int joyNumber, int povsCount, uint16_t* povs);
    static void updateJoyButtons(int joyNumber, uint8_t buttonCount, uint32_t buttons);
    static void updateJoyDescriptor(int joyNumber, HAL_JoystickDescriptor* desc);
    static void updateJoyOutputs(int32_t joyNumber, int64_t outputs, int32_t leftRumble, int32_t rightRumble);

	static void scribeMatchInfo(HAL_MatchInfo* info);
	static void scribeJoyAxes(int joyNumber, HAL_JoystickAxes* axes);
	static void scribeJoyPOVs(int joyNumber, HAL_JoystickPOVs* povs);
	static void scribeJoyButtons(int joyNumber, HAL_JoystickButtons* buttons);
	static void scribeJoyDescriptor(int joyNumber, HAL_JoystickDescriptor* desc);
	static void scribeJoyName(int joyNumber, char* name);

    static HAL_AllianceStationID readAllianceID();
    static HAL_MatchType readMatchType();
    static HAL_ControlWord readControlWord();
	static HAL_Bool readJoyIsXbox(int joyNumber);
	static int32_t readJoyType(int joyNumber);
	static int32_t readJoyAxisType(int joyNumber, int axisNumber);

    static wpi::priority_mutex* getMutex();
    static wpi::priority_condition_variable* getDataSignal();
};

#endif //NATIVE_DRIVEDATA_H
