/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#ifndef __AXIS_CAMERA_PARAMS_H__
#define __AXIS_CAMERA_PARAMS_H__

#include "EnumCameraParameter.h"
#include "ErrorBase.h"
#include "IntCameraParameter.h"
#include "Task.h"
#include "HAL/Semaphore.h"
#include <vector>

/**
 * AxisCameraParams class.
 * This class handles parameter configuration the Axis 206 Ethernet Camera.
 * It starts up a tasks with an independent connection to the camera that monitors
 * for changes to parameters and updates the camera.
 * It is only separate from AxisCamera to isolate the parameter code from the image streaming code.
 */
class AxisCameraParams : public ErrorBase
{
public:
	typedef enum Exposure_t {kExposure_Automatic, kExposure_Hold, kExposure_FlickerFree50Hz, kExposure_FlickerFree60Hz} Exposure;
	typedef enum WhiteBalance_t {kWhiteBalance_Automatic, kWhiteBalance_Hold, kWhiteBalance_FixedOutdoor1, kWhiteBalance_FixedOutdoor2, kWhiteBalance_FixedIndoor, kWhiteBalance_FixedFlourescent1, kWhiteBalance_FixedFlourescent2} WhiteBalance;
	typedef enum Resolution_t {kResolution_640x480, kResolution_640x360, kResolution_320x240, kResolution_160x120} Resolution;
	typedef enum Rotation_t {kRotation_0, kRotation_180} Rotation;

protected:
	AxisCameraParams(const char* ipAddress);
	virtual ~AxisCameraParams();

public:
	// Mid-stream gets & writes
	void WriteBrightness(int);
	int GetBrightness();
	void WriteWhiteBalance(WhiteBalance_t whiteBalance);
	WhiteBalance_t GetWhiteBalance();
	void WriteColorLevel(int);
	int GetColorLevel();
	void WriteExposureControl(Exposure_t);
	Exposure_t GetExposureControl();
	void WriteExposurePriority(int);
	int GetExposurePriority();
	void WriteMaxFPS(int);
	int GetMaxFPS();

	// New-Stream gets & writes (i.e. require restart)
	void WriteResolution(Resolution_t);
	Resolution_t GetResolution();
	void WriteCompression(int);
	int GetCompression();
	void WriteRotation(Rotation_t);
	Rotation_t GetRotation();

protected:
	virtual void RestartCameraTask() = 0;
	int CreateCameraSocket(const char *requestString);

	static int s_ParamTaskFunction(AxisCameraParams* thisPtr);
	int ParamTaskFunction();

	int UpdateCamParam(const char *param);
	int ReadCamParams();

	Task m_paramTask;
	uint32_t m_ipAddress; // IPv4
	SEMAPHORE_ID m_paramChangedSem;
	SEMAPHORE_ID m_socketPossessionSem;

	//Camera Properties
	IntCameraParameter *m_brightnessParam;
	IntCameraParameter *m_compressionParam;
	IntCameraParameter *m_exposurePriorityParam;
	IntCameraParameter *m_colorLevelParam;
	IntCameraParameter *m_maxFPSParam;
	EnumCameraParameter *m_rotationParam;
	EnumCameraParameter *m_resolutionParam;
	EnumCameraParameter *m_exposureControlParam;
	EnumCameraParameter *m_whiteBalanceParam;

	// A vector to access all properties simply.
	typedef std::vector<IntCameraParameter*> ParameterVector_t;
	ParameterVector_t m_parameters;
};


#endif
