/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

class TestBench {
 public:
  /* Analog input channels */
  static const uint32_t kCameraGyroChannel = 0;
  static const uint32_t kFakeCompressorChannel = 1;
  static const uint32_t kFakeAnalogOutputChannel = 2;

  /* Analog output channels */
  static const uint32_t kAnalogOutputChannel = 0;

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
  static const uint32_t kVictorChannel = 1;
  static const uint32_t kJaguarChannel = 2;
  static const uint32_t kCameraPanChannel = 8;
  static const uint32_t kCameraTiltChannel = 9;

  /* MXP digital channels */
  static const uint32_t kTalonChannel = 10;
  static const uint32_t kFakePressureSwitchChannel = 11;
  static const uint32_t kFakeSolenoid1Channel = 12;
  static const uint32_t kFakeSolenoid2Channel = 13;
  static const uint32_t kFakeRelayForward = 18;
  static const uint32_t kFakeRelayReverse = 19;

  /* Relay channels */
  static const uint32_t kRelayChannel = 0;

  /* PDP channels */
  static const uint32_t kJaguarPDPChannel = 6;
  static const uint32_t kVictorPDPChannel = 8;
  static const uint32_t kTalonPDPChannel = 10;

  /* PCM channels */
  static const int32_t kSolenoidChannel1 = 0;
  static const int32_t kSolenoidChannel2 = 1;

  /* Filter constants */
  static constexpr double kFilterStep = 0.005;
  static constexpr double kFilterTime = 2.0;
  static constexpr double kSinglePoleIIRTimeConstant = 0.015915;
  static constexpr double kSinglePoleIIRExpectedOutput = -3.2172003;
  static constexpr double kHighPassTimeConstant = 0.006631;
  static constexpr double kHighPassExpectedOutput = 10.074717;
  static constexpr int32_t kMovAvgTaps = 6;
  static constexpr double kMovAvgExpectedOutput = -10.191644;
};
