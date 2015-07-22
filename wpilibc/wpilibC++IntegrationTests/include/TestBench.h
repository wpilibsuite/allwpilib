/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <cstdint>

class TestBench {
 public:
  /* Analog input channels */
  static const uint32_t kCameraGyroChannel = 0;
  static const uint32_t kFakeCompressorChannel = 1;
  static const uint32_t kFakeAnalogOutputChannel = 2;

  /* Analog output channels */
  static const uint32_t kAnalogOutputChannel = 0;
  static const uint32_t kFakeJaguarPotentiometer = 1;

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
  static const uint32_t kFakeJaguarForwardLimit = 20;
  static const uint32_t kFakeJaguarReverseLimit = 21;

  /* Relay channels */
  static const uint32_t kRelayChannel = 0;
  static const uint32_t kCANJaguarRelayChannel = 1;

  /* CAN IDs */
  static const uint32_t kCANJaguarID = 2;

  /* PDP channels */
  static const uint32_t kJaguarPDPChannel = 6;
  static const uint32_t kVictorPDPChannel = 8;
  static const uint32_t kTalonPDPChannel = 11;

  /* PCM channels */
  static const int32_t kSolenoidChannel1 = 0;
  static const int32_t kSolenoidChannel2 = 1;
};
