/*************************************************************
 * 					NOTICE
 * 
 * 	These are the only externally exposed functions to the
 *   NetworkCommunication library
 * 
 * This is an implementation of FRC Spec for Comm Protocol
 * Revision 4.5, June 30, 2008
 *
 * Copyright (c) National Instruments 2008.  All Rights Reserved.
 * 
 *************************************************************/

#ifndef __FRC_COMM_H__
#define __FRC_COMM_H__

#ifdef SIMULATION
#include <vxWorks_compat.h>
#ifdef USE_THRIFT
#define EXPORT_FUNC
#else
#define EXPORT_FUNC __declspec(dllexport) __cdecl
#endif
#else
#if defined(__vxworks)
#include <vxWorks.h>
#define EXPORT_FUNC
#else
#include <stdint.h>
#include <pthread.h>
#define EXPORT_FUNC
#endif
#endif

#define ERR_FRCSystem_NetCommNotResponding -44049

enum AllianceStationID_t {
	kAllianceStationID_red1,
	kAllianceStationID_red2,
	kAllianceStationID_red3,
	kAllianceStationID_blue1,
	kAllianceStationID_blue2,
	kAllianceStationID_blue3,
};

struct ControlWord_t {
#ifndef __vxworks
	uint32_t enabled : 1;
	uint32_t autonomous : 1;
	uint32_t test :1;
	uint32_t eStop : 1;
	uint32_t fmsAttached:1;
	uint32_t dsAttached:1;
	uint32_t control_reserved : 26;
#else
	uint32_t control_reserved : 26;
	uint32_t dsAttached:1;
	uint32_t fmsAttached:1;
	uint32_t eStop : 1;
	uint32_t test :1;
	uint32_t autonomous : 1;
	uint32_t enabled : 1;
#endif
};

struct JoystickAxes_t {
	uint16_t count;
	int16_t axes[1];
};

struct JoystickPOV_t {
	uint16_t count;
	int16_t povs[1];
};

#ifdef __cplusplus
extern "C" {
#endif
	int EXPORT_FUNC FRC_NetworkCommunication_Reserve(void *instance);
#ifndef SIMULATION
	void EXPORT_FUNC getFPGAHardwareVersion(uint16_t *fpgaVersion, uint32_t *fpgaRevision);
#endif
	int EXPORT_FUNC setStatusData(float battery, uint8_t dsDigitalOut, uint8_t updateNumber,
			const char *userDataHigh, int userDataHighLength,
			const char *userDataLow, int userDataLowLength, int wait_ms);
	int EXPORT_FUNC setErrorData(const char *errors, int errorsLength, int wait_ms);
	
#ifdef SIMULATION
	void EXPORT_FUNC setNewDataSem(HANDLE);
#else
# if defined (__vxworks)
	void EXPORT_FUNC setNewDataSem(SEM_ID);
# else
	void EXPORT_FUNC setNewDataSem(pthread_mutex_t *);
# endif
#endif

	// this uint32_t is really a LVRefNum
	int EXPORT_FUNC setNewDataOccurRef(uint32_t refnum);

	int EXPORT_FUNC FRC_NetworkCommunication_getControlWord(struct ControlWord_t *controlWord);
	int EXPORT_FUNC FRC_NetworkCommunication_getAllianceStation(enum AllianceStationID_t *allianceStation);
	int EXPORT_FUNC FRC_NetworkCommunication_getMatchTime(float *matchTime);
	int EXPORT_FUNC FRC_NetworkCommunication_getJoystickAxes(uint8_t joystickNum, struct JoystickAxes_t *axes, uint8_t maxAxes);
	int EXPORT_FUNC FRC_NetworkCommunication_getJoystickButtons(uint8_t joystickNum, uint32_t *buttons, uint8_t *count);
	int EXPORT_FUNC FRC_NetworkCommunication_getJoystickPOVs(uint8_t joystickNum, struct JoystickPOV_t *povs, uint8_t maxPOVs);
	int EXPORT_FUNC FRC_NetworkCommunication_setJoystickOutputs(uint8_t joystickNum, uint32_t hidOutputs, uint16_t leftRumble, uint16_t rightRumble);
	int EXPORT_FUNC FRC_NetworkCommunication_getJoystickDesc(uint8_t joystickNum, uint8_t *isXBox, uint8_t *type, char *name,
		uint8_t *axisCount, uint8_t *axisTypes, uint8_t *buttonCount, uint8_t *povCount);

	void EXPORT_FUNC FRC_NetworkCommunication_getVersionString(char *version);
	int EXPORT_FUNC FRC_NetworkCommunication_observeUserProgramStarting(void);
	void EXPORT_FUNC FRC_NetworkCommunication_observeUserProgramDisabled(void);
	void EXPORT_FUNC FRC_NetworkCommunication_observeUserProgramAutonomous(void);
	void EXPORT_FUNC FRC_NetworkCommunication_observeUserProgramTeleop(void);
	void EXPORT_FUNC FRC_NetworkCommunication_observeUserProgramTest(void);
#ifdef __cplusplus
}
#endif

#endif
