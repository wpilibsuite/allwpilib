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
#define EXPORT_FUNC __declspec(dllexport) __cdecl
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

// Commandeer some bytes at the end for advanced I/O feedback.
#define IO_CONFIG_DATA_SIZE 32
#define SYS_STATUS_DATA_SIZE 44
#define USER_STATUS_DATA_SIZE (984 - IO_CONFIG_DATA_SIZE - SYS_STATUS_DATA_SIZE)
#define USER_DS_LCD_DATA_SIZE 128

struct FRCCommonControlData{
	uint16_t packetIndex;
	union {
		uint8_t control;
#ifndef __vxworks
		struct {
			uint8_t checkVersions :1;
			uint8_t test :1;
			uint8_t resync : 1;
			uint8_t fmsAttached:1;
			uint8_t autonomous : 1;
			uint8_t enabled : 1;
			uint8_t notEStop : 1;
			uint8_t reset : 1;
		};
#else
		struct {
			uint8_t reset : 1;
			uint8_t notEStop : 1;
			uint8_t enabled : 1;
			uint8_t autonomous : 1;
			uint8_t fmsAttached:1;
			uint8_t resync : 1;
			uint8_t test :1;
			uint8_t checkVersions :1;
		};
#endif
	};
	uint8_t dsDigitalIn;
	uint16_t teamID;

	char dsID_Alliance;
	char dsID_Position;

	union {
		int8_t stick0Axes[6];
		struct {
			int8_t stick0Axis1;
			int8_t stick0Axis2;
			int8_t stick0Axis3;
			int8_t stick0Axis4;
			int8_t stick0Axis5;
			int8_t stick0Axis6;
		};
	};
	uint16_t stick0Buttons;		// Left-most 4 bits are unused

	union {
		int8_t stick1Axes[6];
		struct {
			int8_t stick1Axis1;
			int8_t stick1Axis2;
			int8_t stick1Axis3;
			int8_t stick1Axis4;
			int8_t stick1Axis5;
			int8_t stick1Axis6;
		};
	};
	uint16_t stick1Buttons;		// Left-most 4 bits are unused

	union {
		int8_t stick2Axes[6];
		struct {
			int8_t stick2Axis1;
			int8_t stick2Axis2;
			int8_t stick2Axis3;
			int8_t stick2Axis4;
			int8_t stick2Axis5;
			int8_t stick2Axis6;
		};
	};
	uint16_t stick2Buttons;		// Left-most 4 bits are unused

	union {
		int8_t stick3Axes[6];
		struct {
			int8_t stick3Axis1;
			int8_t stick3Axis2;
			int8_t stick3Axis3;
			int8_t stick3Axis4;
			int8_t stick3Axis5;
			int8_t stick3Axis6;
		};
	};
	uint16_t stick3Buttons;		// Left-most 4 bits are unused

	//Analog inputs are 10 bit right-justified
	uint16_t analog1;
	uint16_t analog2;
	uint16_t analog3;
	uint16_t analog4;

	uint64_t cRIOChecksum;
	uint32_t FPGAChecksum0;
	uint32_t FPGAChecksum1;
	uint32_t FPGAChecksum2;
	uint32_t FPGAChecksum3;

	char versionData[8];
};

#define kFRC_NetworkCommunication_DynamicType_DSEnhancedIO_Input 17
#define kFRC_NetworkCommunication_DynamicType_DSEnhancedIO_Output 18
#define kFRC_NetworkCommunication_DynamicType_Kinect_Header 19
#define kFRC_NetworkCommunication_DynamicType_Kinect_Extra1 20
#define kFRC_NetworkCommunication_DynamicType_Kinect_Vertices1 21
#define kFRC_NetworkCommunication_DynamicType_Kinect_Extra2 22
#define kFRC_NetworkCommunication_DynamicType_Kinect_Vertices2 23
#define kFRC_NetworkCommunication_DynamicType_Kinect_Joystick 24
#define kFRC_NetworkCommunication_DynamicType_Kinect_Custom 25

extern "C" {
#ifndef SIMULATION
	void EXPORT_FUNC getFPGAHardwareVersion(uint16_t *fpgaVersion, uint32_t *fpgaRevision);
#endif
	int EXPORT_FUNC getCommonControlData(FRCCommonControlData *data, int wait_ms);
	int EXPORT_FUNC getRecentCommonControlData(FRCCommonControlData *commonData, int wait_ms);
	int EXPORT_FUNC getRecentStatusData(uint8_t *batteryInt, uint8_t *batteryDec, uint8_t *dsDigitalOut, int wait_ms);
	int EXPORT_FUNC getDynamicControlData(uint8_t type, char *dynamicData, int32_t maxLength, int wait_ms);
	int EXPORT_FUNC setStatusData(float battery, uint8_t dsDigitalOut, uint8_t updateNumber,
			const char *userDataHigh, int userDataHighLength,
			const char *userDataLow, int userDataLowLength, int wait_ms);
	int EXPORT_FUNC setStatusDataFloatAsInt(int battery, uint8_t dsDigitalOut, uint8_t updateNumber,
			const char *userDataHigh, int userDataHighLength,
			const char *userDataLow, int userDataLowLength, int wait_ms);
	int EXPORT_FUNC setErrorData(const char *errors, int errorsLength, int wait_ms);
	int EXPORT_FUNC setUserDsLcdData(const char *userDsLcdData, int userDsLcdDataLength, int wait_ms);
	int EXPORT_FUNC overrideIOConfig(const char *ioConfig, int wait_ms);

#ifdef SIMULATION
	void EXPORT_FUNC setNewDataSem(HANDLE);
#else
# if defined (__vxworks)
	void EXPORT_FUNC setNewDataSem(SEM_ID);
	void EXPORT_FUNC setResyncSem(SEM_ID);
# else
	void EXPORT_FUNC setNewDataSem(pthread_mutex_t *);
	void EXPORT_FUNC setResyncSem(pthread_mutex_t *);
# endif
	void EXPORT_FUNC signalResyncActionDone(void);
#endif

	// this uint32_t is really a LVRefNum
	void EXPORT_FUNC setNewDataOccurRef(uint32_t refnum);
#ifndef SIMULATION
	void EXPORT_FUNC setResyncOccurRef(uint32_t refnum);
#endif

	void EXPORT_FUNC FRC_NetworkCommunication_getVersionString(char *version);
	void EXPORT_FUNC FRC_NetworkCommunication_observeUserProgramStarting(void);
	void EXPORT_FUNC FRC_NetworkCommunication_observeUserProgramDisabled(void);
	void EXPORT_FUNC FRC_NetworkCommunication_observeUserProgramAutonomous(void);
	void EXPORT_FUNC FRC_NetworkCommunication_observeUserProgramTeleop(void);
	void EXPORT_FUNC FRC_NetworkCommunication_observeUserProgramTest(void);
	void EXPORT_FUNC FRC_NetworkCommunication_Reserve();

};

#endif
