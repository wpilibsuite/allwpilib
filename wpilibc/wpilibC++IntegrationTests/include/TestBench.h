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
	static const int kCameraGyroChannel = 1;
	
	/* DIO channels */
	static const int kTalonEncoderChannelA = 1;
	static const int kTalonEncoderChannelB = 2;
	static const int kVictorEncoderChannelA = 3;
	static const int kVictorEncoderChannelB = 4;
	static const int kJaguarEncoderChannelA = 5;
	static const int kJaguarEncoderChannelB = 6;
	static const int kLoop1OutputChannel = 7;
	static const int kLoop1InputChannel = 8;
	static const int kLoop2OutputChannel = 9;
	static const int kLoop2InputChannel = 10;
	
	/* PWM channels */
	static const int kTalonChannel = 1;
	static const int kVictorChannel = 2;
	static const int kJaguarChannel = 3;
	static const int kCameraPanChannel = 9;
	static const int kCameraTiltChannel = 10;
};

