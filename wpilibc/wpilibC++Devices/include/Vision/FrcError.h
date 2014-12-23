/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#pragma once

/* Error Codes */
#define ERR_VISION_GENERAL_ERROR			166000	// 
#define ERR_COLOR_NOT_FOUND					166100	// TrackAPI.cpp
#define ERR_PARTICLE_TOO_SMALL				166101	// TrackAPI.cpp

#define ERR_CAMERA_FAILURE					166200	// AxisCamera.cpp
#define ERR_CAMERA_SOCKET_CREATE_FAILED		166201	// AxisCamera.cpp
#define ERR_CAMERA_CONNECT_FAILED			166202	// AxisCamera.cpp
#define ERR_CAMERA_STALE_IMAGE				166203	// AxisCamera.cpp
#define ERR_CAMERA_NOT_INITIALIZED			166204	// AxisCamera.cpp
#define ERR_CAMERA_NO_BUFFER_AVAILABLE		166205	// AxisCamera.cpp
#define ERR_CAMERA_HEADER_ERROR				166206	// AxisCamera.cpp
#define ERR_CAMERA_BLOCKING_TIMEOUT			166207	// AxisCamera.cpp
#define ERR_CAMERA_AUTHORIZATION_FAILED		166208	// AxisCamera.cpp
#define ERR_CAMERA_TASK_SPAWN_FAILED		166209	// AxisCamera.cpp
#define ERR_CAMERA_TASK_INPUT_OUT_OF_RANGE	166210	// AxisCamera.cpp
#define ERR_CAMERA_COMMAND_FAILURE			166211	// AxisCamera.cpp

/* error handling functions */
int GetLastVisionError();
const char* GetVisionErrorText(int errorCode);

