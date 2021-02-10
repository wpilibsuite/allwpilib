// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

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
  static const uint32_t kFakePwmOutput = 14;

  /* Relay channels */
  static const uint32_t kRelayChannel = 0;

  /* PDP channels */
  static const uint32_t kJaguarPDPChannel = 6;
  static const uint32_t kVictorPDPChannel = 8;
  static const uint32_t kTalonPDPChannel = 10;

  /* PCM channels */
  static const int32_t kSolenoidChannel1 = 0;
  static const int32_t kSolenoidChannel2 = 1;
};
