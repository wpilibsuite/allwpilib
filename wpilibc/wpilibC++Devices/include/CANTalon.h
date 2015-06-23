/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/
#pragma once

#include "SafePWM.h"
#include "CANSpeedController.h"
#include "PIDOutput.h"
#include "PIDSource.h"
#include "PIDInterface.h"
#include "MotorSafetyHelper.h"
#include "LiveWindow/LiveWindowSendable.h"
#include "tables/ITable.h"

class CanTalonSRX;

/**
 * CTRE Talon SRX Speed Controller with CAN Control
 */
class CANTalon : public MotorSafety,
                 public CANSpeedController,
                 public ErrorBase,
                 public LiveWindowSendable,
                 public ITableListener,
                 public PIDSource,
                 public PIDInterface {
 public:
  enum FeedbackDevice {
    QuadEncoder = 0,
    AnalogPot = 2,
    AnalogEncoder = 3,
    EncRising = 4,
    EncFalling = 5
  };
  enum StatusFrameRate {
    StatusFrameRateGeneral = 0,
    StatusFrameRateFeedback = 1,
    StatusFrameRateQuadEncoder = 2,
    StatusFrameRateAnalogTempVbat = 3,
  };
  explicit CANTalon(int deviceNumber);
  explicit CANTalon(int deviceNumber, int controlPeriodMs);
  virtual ~CANTalon();

  // PIDOutput interface
  virtual void PIDWrite(float output) override;

  // PIDSource interface
  virtual double PIDGet() const override;

  // MotorSafety interface
  virtual void SetExpiration(float timeout) override;
  virtual float GetExpiration() const override;
  virtual bool IsAlive() const override;
  virtual void StopMotor() override;
  virtual void SetSafetyEnabled(bool enabled) override;
  virtual bool IsSafetyEnabled() const override;
  virtual void GetDescription(char *desc) const override;

  // CANSpeedController interface
  virtual float Get() const override;
  virtual void Set(float value, uint8_t syncGroup = 0) override;
  virtual void Reset() override;
  virtual void SetSetpoint(float value) override;
  virtual void Disable() override;
  virtual void EnableControl();
  virtual void Enable() override;
  virtual void SetP(double p) override;
  virtual void SetI(double i) override;
  virtual void SetD(double d) override;
  void SetF(double f);
  void SetIzone(unsigned iz);
  virtual void SetPID(double p, double i, double d) override;
  virtual void SetPID(double p, double i, double d, double f);
  virtual double GetP() const override;
  virtual double GetI() const override;
  virtual double GetD() const override;
  virtual double GetF() const;
  virtual float GetBusVoltage() const override;
  virtual float GetOutputVoltage() const override;
  virtual float GetOutputCurrent() const override;
  virtual float GetTemperature() const override;
  void SetPosition(double pos);
  virtual double GetPosition() const override;
  virtual double GetSpeed() const override;
  virtual int GetClosedLoopError() const;
  virtual int GetAnalogIn() const;
  virtual int GetAnalogInRaw() const;
  virtual int GetAnalogInVel() const;
  virtual int GetEncPosition() const;
  virtual int GetEncVel() const;
  int GetPinStateQuadA() const;
  int GetPinStateQuadB() const;
  int GetPinStateQuadIdx() const;
  int IsFwdLimitSwitchClosed() const;
  int IsRevLimitSwitchClosed() const;
  int GetNumberOfQuadIdxRises() const;
  void SetNumberOfQuadIdxRises(int rises);
  virtual bool GetForwardLimitOK() const override;
  virtual bool GetReverseLimitOK() const override;
  virtual uint16_t GetFaults() const override;
  uint16_t GetStickyFaults() const;
  void ClearStickyFaults();
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
  /**
   * Change the fwd limit switch setting to normally open or closed.
   * Talon will disable momentarilly if the Talon's current setting
   * is dissimilar to the caller's requested setting.
   *
   * Since Talon saves setting to flash this should only affect
   * a given Talon initially during robot install.
   *
   * @param normallyOpen true for normally open.  false for normally closed.
   */
  void ConfigFwdLimitSwitchNormallyOpen(bool normallyOpen);
  /**
   * Change the rev limit switch setting to normally open or closed.
   * Talon will disable momentarilly if the Talon's current setting
   * is dissimilar to the caller's requested setting.
   *
   * Since Talon saves setting to flash this should only affect
   * a given Talon initially during robot install.
   *
   * @param normallyOpen true for normally open.  false for normally closed.
   */
  void ConfigRevLimitSwitchNormallyOpen(bool normallyOpen);
  virtual void ConfigMaxOutputVoltage(double voltage) override;
  virtual void ConfigFaultTime(float faultTime) override;
  virtual void SetControlMode(ControlMode mode);
  void SetFeedbackDevice(FeedbackDevice device);
  void SetStatusFrameRateMs(StatusFrameRate stateFrame, int periodMs);
  virtual ControlMode GetControlMode() const;
  void SetSensorDirection(bool reverseSensor);
  void SetCloseLoopRampRate(double rampRate);
  void SelectProfileSlot(int slotIdx);
  int GetIzone() const;
  int GetIaccum() const;
  void ClearIaccum();
  int GetBrakeEnableDuringNeutral() const;

  bool IsControlEnabled() const;
  bool IsEnabled() const override;
  double GetSetpoint() const override;

  // LiveWindow stuff.
  void ValueChanged(ITable *source, const std::string &key, EntryValue value,
                    bool isNew) override;
  void UpdateTable() override;
  void StartLiveWindowMode() override;
  void StopLiveWindowMode() override;
  std::string GetSmartDashboardType() const override;
  void InitTable(ITable *subTable) override;
  ITable *GetTable() const override;

  // SpeedController overrides
  virtual void SetInverted(bool isInverted) override;
  virtual bool GetInverted() const override;

 private:
  // Values for various modes as is sent in the CAN packets for the Talon.
  enum TalonControlMode {
    kThrottle = 0,
    kFollowerMode = 5,
    kVoltageMode = 4,
    kPositionMode = 1,
    kSpeedMode = 2,
    kCurrentMode = 3,
    kDisabled = 15
  };

  int m_deviceNumber;
  CanTalonSRX *m_impl;
  MotorSafetyHelper *m_safetyHelper;
  int m_profile;  // Profile from CANTalon to use. Set to zero until we can
                  // actually test this.

  bool m_controlEnabled;
  ControlMode m_controlMode;
  TalonControlMode m_sendMode;

  double m_setPoint;
  static const unsigned int kDelayForSolicitedSignalsUs = 4000;
  /**
   * Fixup the sendMode so Set() serializes the correct demand value.
   * Also fills the modeSelecet in the control frame to disabled.
   * @param mode Control mode to ultimately enter once user calls Set().
   * @see Set()
   */
  void ApplyControlMode(CANSpeedController::ControlMode mode);

  // LiveWindow stuff.
  ITable *m_table;
  bool m_isInverted;
};
