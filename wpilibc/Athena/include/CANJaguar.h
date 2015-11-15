/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2009. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/
#pragma once

#include "ErrorBase.h"
#include "MotorSafety.h"
#include "Resource.h"
#include "MotorSafetyHelper.h"
#include "PIDOutput.h"
#include "CANSpeedController.h"
#include "HAL/cpp/Semaphore.hpp"
#include "HAL/HAL.hpp"
#include "LiveWindow/LiveWindowSendable.h"
#include "tables/ITableListener.h"
#include "NetworkCommunication/CANSessionMux.h"
#include "CAN/can_proto.h"

#include <atomic>
#include "HAL/cpp/priority_mutex.h"
#include <memory>
#include <utility>
#include <sstream>

/**
 * Luminary Micro / Vex Robotics Jaguar Speed Control
 */
class CANJaguar : public MotorSafety,
                  public CANSpeedController,
                  public ErrorBase,
                  public LiveWindowSendable,
                  public ITableListener {
 public:
  // The internal PID control loop in the Jaguar runs at 1kHz.
  static const int32_t kControllerRate = 1000;
  static constexpr double kApproxBusVoltage = 12.0;

  // Control mode tags
  /** Sets an encoder as the speed reference only. <br> Passed as the "tag" when
   * setting the control mode.*/
  static const struct EncoderStruct {
  } Encoder;
  /** Sets a quadrature encoder as the position and speed reference. <br> Passed
   * as the "tag" when setting the control mode.*/
  static const struct QuadEncoderStruct {
  } QuadEncoder;
  /** Sets a potentiometer as the position reference only. <br> Passed as the
   * "tag" when setting the control mode. */
  static const struct PotentiometerStruct {
  } Potentiometer;

  explicit CANJaguar(uint8_t deviceNumber);
  virtual ~CANJaguar();

  uint8_t getDeviceNumber() const;
  uint8_t GetHardwareVersion() const;

  // PIDOutput interface
  virtual void PIDWrite(float output) override;

  // Control mode methods
  void EnableControl(double encoderInitialPosition = 0.0);
  void DisableControl();

  void SetPercentMode();
  void SetPercentMode(EncoderStruct, uint16_t codesPerRev);
  void SetPercentMode(QuadEncoderStruct, uint16_t codesPerRev);
  void SetPercentMode(PotentiometerStruct);

  void SetCurrentMode(double p, double i, double d);
  void SetCurrentMode(EncoderStruct, uint16_t codesPerRev, double p, double i,
                      double d);
  void SetCurrentMode(QuadEncoderStruct, uint16_t codesPerRev, double p,
                      double i, double d);
  void SetCurrentMode(PotentiometerStruct, double p, double i, double d);

  void SetSpeedMode(EncoderStruct, uint16_t codesPerRev, double p, double i,
                    double d);
  void SetSpeedMode(QuadEncoderStruct, uint16_t codesPerRev, double p, double i,
                    double d);

  void SetPositionMode(QuadEncoderStruct, uint16_t codesPerRev, double p,
                       double i, double d);
  void SetPositionMode(PotentiometerStruct, double p, double i, double d);

  void SetVoltageMode();
  void SetVoltageMode(EncoderStruct, uint16_t codesPerRev);
  void SetVoltageMode(QuadEncoderStruct, uint16_t codesPerRev);
  void SetVoltageMode(PotentiometerStruct);

  // CANSpeedController interface
  virtual float Get() const override;
  virtual void Set(float value, uint8_t syncGroup = 0) override;
  virtual void Disable() override;
  virtual void SetP(double p) override;
  virtual void SetI(double i) override;
  virtual void SetD(double d) override;
  virtual void SetPID(double p, double i, double d) override;
  virtual double GetP() const override;
  virtual double GetI() const override;
  virtual double GetD() const override;
  virtual bool IsModePID(CANSpeedController::ControlMode mode) const override;
  virtual float GetBusVoltage() const override;
  virtual float GetOutputVoltage() const override;
  virtual float GetOutputCurrent() const override;
  virtual float GetTemperature() const override;
  virtual double GetPosition() const override;
  virtual double GetSpeed() const override;
  virtual bool GetForwardLimitOK() const override;
  virtual bool GetReverseLimitOK() const override;
  virtual uint16_t GetFaults() const override;
  virtual void SetVoltageRampRate(double rampRate) override;
  virtual uint32_t GetFirmwareVersion() const override;
  virtual void ConfigNeutralMode(NeutralMode mode) override;
  virtual void ConfigEncoderCodesPerRev(uint16_t codesPerRev) override;
  virtual void ConfigPotentiometerTurns(uint16_t turns) override;
  virtual void ConfigSoftPositionLimits(double forwardLimitPosition,
                                        double reverseLimitPosition) override;
  virtual void DisableSoftPositionLimits() override;
  virtual void ConfigLimitMode(LimitMode mode) override;
  virtual void ConfigForwardLimit(double forwardLimitPosition) override;
  virtual void ConfigReverseLimit(double reverseLimitPosition) override;
  virtual void ConfigMaxOutputVoltage(double voltage) override;
  virtual void ConfigFaultTime(float faultTime) override;
  virtual void SetControlMode(ControlMode mode);
  virtual ControlMode GetControlMode() const;

  static void UpdateSyncGroup(uint8_t syncGroup);

  void SetExpiration(float timeout) override;
  float GetExpiration() const override;
  bool IsAlive() const override;
  void StopMotor() override;
  bool IsSafetyEnabled() const override;
  void SetSafetyEnabled(bool enabled) override;
  void GetDescription(std::ostringstream& desc) const override;
  uint8_t GetDeviceID() const;

  // SpeedController overrides
  virtual void SetInverted(bool isInverted) override;
  virtual bool GetInverted() const override;

 protected:
  // Control mode helpers
  void SetSpeedReference(uint8_t reference);
  uint8_t GetSpeedReference() const;

  void SetPositionReference(uint8_t reference);
  uint8_t GetPositionReference() const;

  uint8_t packPercentage(uint8_t *buffer, double value);
  uint8_t packFXP8_8(uint8_t *buffer, double value);
  uint8_t packFXP16_16(uint8_t *buffer, double value);
  uint8_t packint16_t(uint8_t *buffer, int16_t value);
  uint8_t packint32_t(uint8_t *buffer, int32_t value);
  double unpackPercentage(uint8_t *buffer) const;
  double unpackFXP8_8(uint8_t *buffer) const;
  double unpackFXP16_16(uint8_t *buffer) const;
  int16_t unpackint16_t(uint8_t *buffer) const;
  int32_t unpackint32_t(uint8_t *buffer) const;

  void sendMessage(uint32_t messageID, const uint8_t *data, uint8_t dataSize,
                   int32_t period = CAN_SEND_PERIOD_NO_REPEAT);
  void requestMessage(uint32_t messageID,
                      int32_t period = CAN_SEND_PERIOD_NO_REPEAT);
  bool getMessage(uint32_t messageID, uint32_t mask, uint8_t *data,
                  uint8_t *dataSize) const;

  void setupPeriodicStatus();
  void updatePeriodicStatus() const;

  mutable priority_recursive_mutex m_mutex;

  uint8_t m_deviceNumber;
  float m_value = 0.0f;

  // Parameters/configuration
  ControlMode m_controlMode = kPercentVbus;
  uint8_t m_speedReference = LM_REF_NONE;
  uint8_t m_positionReference = LM_REF_NONE;
  double m_p = 0.0;
  double m_i = 0.0;
  double m_d = 0.0;
  NeutralMode m_neutralMode = kNeutralMode_Jumper;
  uint16_t m_encoderCodesPerRev = 0;
  uint16_t m_potentiometerTurns = 0;
  LimitMode m_limitMode = kLimitMode_SwitchInputsOnly;
  double m_forwardLimit = 0.0;
  double m_reverseLimit = 0.0;
  double m_maxOutputVoltage = 30.0;
  double m_voltageRampRate = 0.0;
  float m_faultTime = 0.0f;

  // Which parameters have been verified since they were last set?
  bool m_controlModeVerified = false; // Needs to be verified because it's set in the constructor
  bool m_speedRefVerified = true;
  bool m_posRefVerified = true;
  bool m_pVerified = true;
  bool m_iVerified = true;
  bool m_dVerified = true;
  bool m_neutralModeVerified = true;
  bool m_encoderCodesPerRevVerified = true;
  bool m_potentiometerTurnsVerified = true;
  bool m_forwardLimitVerified = true;
  bool m_reverseLimitVerified = true;
  bool m_limitModeVerified = true;
  bool m_maxOutputVoltageVerified = true;
  bool m_voltageRampRateVerified = true;
  bool m_faultTimeVerified = true;

  // Status data
  mutable float m_busVoltage = 0.0f;
  mutable float m_outputVoltage = 0.0f;
  mutable float m_outputCurrent = 0.0f;
  mutable float m_temperature = 0.0f;
  mutable double m_position = 0.0;
  mutable double m_speed = 0.0;
  mutable uint8_t m_limits = 0x00;
  mutable uint16_t m_faults = 0x0000;
  uint32_t m_firmwareVersion = 0;
  uint8_t m_hardwareVersion = 0;

  // Which periodic status messages have we received at least once?
  mutable std::atomic<bool> m_receivedStatusMessage0{false};
  mutable std::atomic<bool> m_receivedStatusMessage1{false};
  mutable std::atomic<bool> m_receivedStatusMessage2{false};

  bool m_controlEnabled = false;

  void verify();

  std::unique_ptr<MotorSafetyHelper> m_safetyHelper;

  void ValueChanged(ITable* source, llvm::StringRef key,
                    std::shared_ptr<nt::Value> value, bool isNew) override;
  void UpdateTable() override;
  void StartLiveWindowMode() override;
  void StopLiveWindowMode() override;
  std::string GetSmartDashboardType() const override;
  void InitTable(std::shared_ptr<ITable> subTable) override;
  std::shared_ptr<ITable> GetTable() const override;

  std::shared_ptr<ITable> m_table;

 private:
  void InitCANJaguar();
  bool m_isInverted = false;
};
