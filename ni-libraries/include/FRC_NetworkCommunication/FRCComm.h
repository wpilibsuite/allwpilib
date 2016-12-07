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

#ifdef _WIN32
#include <windows.h>
#ifdef USE_THRIFT
#  define EXPORT_FUNC
# else
#  define EXPORT_FUNC __declspec(dllexport) __cdecl
# endif
#elif defined(__vxworks)
# include <vxWorks.h>
# define EXPORT_FUNC
#elif defined(__linux)
# include <stdint.h>
# include <pthread.h>
# define EXPORT_FUNC
#endif

#define ERR_FRCSystem_NetCommNotResponding -44049
#define ERR_FRCSystem_NoDSConnection -44018

#ifdef _WIN32
# define __DEPRECATED__ __declspec(deprecated)
#else
# define __DEPRECATED__ __attribute__((__deprecated__))
#endif

enum AllianceStationID_t {
	kAllianceStationID_red1,
	kAllianceStationID_red2,
	kAllianceStationID_red3,
	kAllianceStationID_blue1,
	kAllianceStationID_blue2,
	kAllianceStationID_blue3,
};

enum MatchType_t {
	kMatchType_none,
	kMatchType_practice,
	kMatchType_qualification,
	kMatchType_elimination,
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
	/**
	 * Safely copy data into the status packet to be sent back to the driver station.
	 * @deprecated battery is the only parameter to this function that is still used, and only on cRIO / simulation.
	 */
	__DEPRECATED__ int EXPORT_FUNC setStatusData(float battery, uint8_t dsDigitalOut, uint8_t updateNumber,
			const char *userDataHigh, int userDataHighLength,
			const char *userDataLow, int userDataLowLength, int wait_ms);
	/**
	 * Send error data to the DS
	 * @deprecated This old method is hard to parse on the DS side. It will be removed soon. Use FRC_NetworkCommunication_sendError instead.
	 * @param errorData is a cstr of the error message
	 * @param errorDataLength is the length of the errorData
	 * @param wait_ms is ignored (included for binary compatibility)
	 * @return 0 on success, 1 on no DS connection
	 */
	__DEPRECATED__ int EXPORT_FUNC setErrorData(const char *errors, int errorsLength, int wait_ms);

	/**
	 * Send a console output line to the Driver Station
	 * @param line a null-terminated string
	 * @return 0 on success, other on failure
	 */
	int EXPORT_FUNC FRC_NetworkCommunication_sendConsoleLine(const char *line);

	/**
	 * Send an error to the Driver Station
	 * @param isError true if error, false if warning
	 * @param errorCode value of error condition
	 * @param isLVCode true if error code is defined in errors.txt, false if not (i.e. made up for C++)
	 * @param details error description that contains details such as which resource number caused the failure
	 * @param location Source file, function, and line number that the error was generated - optional
	 * @param callStack The details about what functions were called through before the error was reported - optional
	 * @return 0 on success, other on failure
	 */
	int EXPORT_FUNC FRC_NetworkCommunication_sendError(int isError, int32_t errorCode, int isLVCode,
		const char *details, const char *location, const char *callStack);

#ifdef _WIN32
	void EXPORT_FUNC setNewDataSem(HANDLE);
#elif defined (__vxworks)
	void EXPORT_FUNC setNewDataSem(SEM_ID);
#else
	void EXPORT_FUNC setNewDataSem(pthread_cond_t *);
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
