/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.
 */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#ifndef JOYSTICK_H_
#define JOYSTICK_H_

#include <cstdint>
#include <memory>
#include <vector>
#include "GenericHID.h"
#include "ErrorBase.h"

class DriverStation;

/**
 * Handle input from standard Joysticks connected to the Driver Station.
 * This class handles standard input that comes from the Driver Station. Each
 * time a value is requested
 * the most recent value is returned. There is a single class instance for each
 * joystick and the mapping
 * of ports to hardware buttons depends on the code in the driver station.
 */
class Joystick : public GenericHID, public ErrorBase {
 public:
  static const uint32_t kDefaultXAxis = 0;
  static const uint32_t kDefaultYAxis = 1;
  static const uint32_t kDefaultZAxis = 2;
  static const uint32_t kDefaultTwistAxis = 2;
  static const uint32_t kDefaultThrottleAxis = 3;
  typedef enum {
    kXAxis,
    kYAxis,
    kZAxis,
    kTwistAxis,
    kThrottleAxis,
    kNumAxisTypes
  } AxisType;
  static const uint32_t kDefaultTriggerButton = 1;
  static const uint32_t kDefaultTopButton = 2;
  typedef enum { kTriggerButton, kTopButton, kNumButtonTypes } ButtonType;
  typedef enum { kLeftRumble, kRightRumble } RumbleType;
  typedef enum {
    kUnknown = -1,
    kXInputUnknown = 0,
    kXInputGamepad = 1,
    kXInputWheel = 2,
    kXInputArcadeStick = 3,
    kXInputFlightStick = 4,
    kXInputDancePad = 5,
    kXInputGuitar = 6,
    kXInputGuitar2 = 7,
    kXInputDrumKit = 8,
    kXInputGuitar3 = 11,
    kXInputArcadePad = 19,
    kHIDJoystick = 20,
    kHIDGamepad = 21,
    kHIDDriving = 22,
    kHIDFlight = 23,
    kHID1stPerson = 24
  } HIDType;
  explicit Joystick(uint32_t port);
  Joystick(uint32_t port, uint32_t numAxisTypes, uint32_t numButtonTypes);
  virtual ~Joystick() = default;

  Joystick(const Joystick&) = delete;
  Joystick& operator=(const Joystick&) = delete;

  uint32_t GetAxisChannel(AxisType axis) const;
  void SetAxisChannel(AxisType axis, uint32_t channel);

  virtual float GetX(JoystickHand hand = kRightHand) const override;
  virtual float GetY(JoystickHand hand = kRightHand) const override;
  virtual float GetZ() const override;
  virtual float GetTwist() const override;
  virtual float GetThrottle() const override;
  virtual float GetAxis(AxisType axis) const;
  float GetRawAxis(uint32_t axis) const override;

  virtual bool GetTrigger(JoystickHand hand = kRightHand) const override;
  virtual bool GetTop(JoystickHand hand = kRightHand) const override;
  virtual bool GetBumper(JoystickHand hand = kRightHand) const override;
  virtual bool GetRawButton(uint32_t button) const override;
  virtual int GetPOV(uint32_t pov = 0) const override;
  bool GetButton(ButtonType button) const;
  static Joystick *GetStickForPort(uint32_t port);

  virtual float GetMagnitude() const;
  virtual float GetDirectionRadians() const;
  virtual float GetDirectionDegrees() const;

  bool GetIsXbox() const;
  Joystick::HIDType GetType() const;
  std::string GetName() const;
  int GetAxisType(uint8_t axis) const;

  int GetAxisCount() const;
  int GetButtonCount() const;
  int GetPOVCount() const;

  void SetRumble(RumbleType type, float value);
  void SetOutput(uint8_t outputNumber, bool value);
  void SetOutputs(uint32_t value);

 private:
  DriverStation &m_ds;
  uint32_t m_port;
  std::vector<uint32_t> m_axes;
  std::vector<uint32_t> m_buttons;
  uint32_t m_outputs = 0;
  uint16_t m_leftRumble = 0;
  uint16_t m_rightRumble = 0;
};

#endif
