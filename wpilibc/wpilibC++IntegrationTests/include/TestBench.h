/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "WPILib.h"

class TestBench {
public:
	/* Analog channels */
	static const uint32_t kCameraGyroChannel = 0;
	
	/* DIO channels */
	static const uint32_t kTalonEncoderChannelA = 0;
	static const uint32_t kTalonEncoderChannelB = 1;
	static const uint32_t kVictorEncoderChannelA = 2;
	static const uint32_t kVictorEncoderChannelB = 3;
	static const uint32_t kJaguarEncoderChannelA = 4;
	static const uint32_t kJaguarEncoderChannelB = 5;
	static const uint32_t kLoop1OutputChannel = 6;
	static const uint32_t kLoop1InputChannel = 7;
	static const uint32_t kLoop2OutputChannel = 8;
	static const uint32_t kLoop2InputChannel = 9;
	
	/* PWM channels */
	static const uint32_t kTalonChannel = 0;
	static const uint32_t kVictorChannel = 1;
	static const uint32_t kJaguarChannel = 2;
	static const uint32_t kCameraPanChannel = 8;
	static const uint32_t kCameraTiltChannel = 9;
};

