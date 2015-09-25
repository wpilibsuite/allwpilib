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
#include "HAL/CanTalonSRX.h"
#include "MotorSafetyHelper.h"
#include "LiveWindow/LiveWindowSendable.h"
#include "tables/ITable.h"

#include <memory>

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
    EncFalling = 5,
    CtreMagEncoder_Relative = 6, //!< Cross The Road Electronics Magnetic Encoder in Absolute/PulseWidth Mode
    CtreMagEncoder_Absolute = 7, //!< Cross The Road Electronics Magnetic Encoder in Relative/Quadrature Mode
    PulseWidth = 8,
  };
  /**
   * Depending on the sensor type, Talon can determine if sensor is plugged in ot not.
   */
  enum FeedbackDeviceStatus {
	FeedbackStatusUnknown = 0, 		//!< Sensor status could not be determined.  Not all sensors can do this.
	FeedbackStatusPresent = 1, 		//!< Sensor is present and working okay.
	FeedbackStatusNotPresent = 2, 	//!< Sensor is not present, not plugged in, not powered, etc...
  };
  enum StatusFrameRate {
    StatusFrameRateGeneral = 0,
    StatusFrameRateFeedback = 1,
    StatusFrameRateQuadEncoder = 2,
    StatusFrameRateAnalogTempVbat = 3,
    StatusFrameRatePulseWidthMeas = 4,
  };
  explicit CANTalon(int deviceNumber);
  explicit CANTalon(int deviceNumber, int controlPeriodMs);
  DEFAULT_MOVE_CONSTRUCTOR(CANTalon);
  virtual ~CANTalon();

  // PIDOutput interface
  virtual void PIDWrite(float output) override;

  // PIDSource interface
  virtual double PIDGet() override;

  // MotorSafety interface
  virtual void SetExpiration(float timeout) override;
  virtual float GetExpiration() const override;
  virtual bool IsAlive() const override;
  virtual void StopMotor() override;
  virtual void SetSafetyEnabled(bool enabled) override;
  virtual bool IsSafetyEnabled() const override;
  virtual void GetDescription(std::ostringstream& desc) const override;

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
  virtual bool IsModePID(CANSpeedController::ControlMode mode) const override;
  virtual float GetBusVoltage() const override;
  virtual float GetOutputVoltage() const override;
  virtual float GetOutputCurrent() const override;
  virtual float GetTemperature() const override;
  void SetPosition(double pos);
  virtual double GetPosition() const override;
  virtual double GetSpeed() const override;
  virtual int GetClosedLoopError() const;
  virtual void SetAllowableClosedLoopErr(uint32_t allowableCloseLoopError);
  virtual int GetAnalogIn() const;
  virtual void SetAnalogPosition(int newPosition);
  virtual int GetAnalogInRaw() const;
  virtual int GetAnalogInVel() const;
  virtual int GetEncPosition() const;
  virtual void SetEncPosition(int);
  virtual int GetEncVel() const;
  int GetPinStateQuadA() const;
  int GetPinStateQuadB() const;
  int GetPinStateQuadIdx() const;
  int IsFwdLimitSwitchClosed() const;
  int IsRevLimitSwitchClosed() const;
  int GetNumberOfQuadIdxRises() const;
  void SetNumberOfQuadIdxRises(int rises);
  virtual int GetPulseWidthPosition() const;
  virtual void SetPulseWidthPosition(int newpos);
  virtual int GetPulseWidthVelocity() const;
  virtual int GetPulseWidthRiseToFallUs() const;
  virtual int GetPulseWidthRiseToRiseUs() const;
  virtual FeedbackDeviceStatus IsSensorPresent(FeedbackDevice feedbackDevice)const;
  virtual bool GetForwardLimitOK() const override;
  virtual bool GetReverseLimitOK() const override;
  virtual uint16_t GetFaults() const override;
  uint16_t GetStickyFaults() const;
  void ClearStickyFaults();
  virtual void SetVoltageRampRate(double rampRate) override;
  virtual void SetVoltageCompensationRampRate(double rampRate);
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
  void ConfigPeakOutputVoltage(double forwardVoltage,double reverseVoltage);
  void ConfigNominalOutputVoltage(double forwardVoltage,double reverseVoltage);
  /**
   * Enables Talon SRX to automatically zero the Sensor Position whenever an
   * edge is detected on the index signal.
   * @param enable 		boolean input, pass true to enable feature or false to disable.
   * @param risingEdge 	boolean input, pass true to clear the position on rising edge,
   *					pass false to clear the position on falling edge.
   */
  void EnableZeroSensorPositionOnIndex(bool enable, bool risingEdge);
  void ConfigSetParameter(uint32_t paramEnum, double value);
  bool GetParameter(uint32_t paramEnum, double & dvalue) const;

  virtual void ConfigFaultTime(float faultTime) override;
  virtual void SetControlMode(ControlMode mode);
  void SetFeedbackDevice(FeedbackDevice device);
  void SetStatusFrameRateMs(StatusFrameRate stateFrame, int periodMs);
  virtual ControlMode GetControlMode() const;
  void SetSensorDirection(bool reverseSensor);
  void SetClosedLoopOutputDirection(bool reverseOutput);
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
  void ValueChanged(ITable* source, llvm::StringRef key,
                    std::shared_ptr<nt::Value> value, bool isNew) override;
  void UpdateTable() override;
  void StartLiveWindowMode() override;
  void StopLiveWindowMode() override;
  std::string GetSmartDashboardType() const override;
  void InitTable(std::shared_ptr<ITable> subTable) override;
  std::shared_ptr<ITable> GetTable() const override;

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
  std::unique_ptr<CanTalonSRX> m_impl;
  std::unique_ptr<MotorSafetyHelper> m_safetyHelper;
  int m_profile = 0;  // Profile from CANTalon to use. Set to zero until we can
                  // actually test this.

  bool m_controlEnabled = true;
  ControlMode m_controlMode = kPercentVbus;
  TalonControlMode m_sendMode;

  double m_setPoint = 0;
  /**
   * Encoder CPR, counts per rotations, also called codes per revoluion.
   * Default value of zero means the API behaves as it did during the 2015 season, each position
   * unit is a single pulse and there are four pulses per count (4X).
   * Caller can use ConfigEncoderCodesPerRev to set the quadrature encoder CPR.
   */
  uint32_t m_codesPerRev = 0;
  /**
   * Number of turns per rotation.  For example, a 10-turn pot spins ten full rotations from
   * a wiper voltage of zero to 3.3 volts.  Therefore knowing the
   * number of turns a full voltage sweep represents is necessary for calculating rotations
   * and velocity.
   * A default value of zero means the API behaves as it did during the 2015 season, there are 1024
   * position units from zero to 3.3V.
   */
  uint32_t m_numPotTurns = 0;
  /**
   * Although the Talon handles feedback selection, caching the feedback selection is helpful at the API level
   * for scaling into rotations and RPM.
   */
  FeedbackDevice m_feedbackDevice = QuadEncoder;

  static const unsigned int kDelayForSolicitedSignalsUs = 4000;
  /**
   * @param devToLookup FeedbackDevice to lookup the scalar for.  Because Talon
   * 				allows multiple sensors to be attached simultaneously, caller must
   *				specify which sensor to lookup.
   * @return 		The number of native Talon units per rotation of the selected sensor.
   *				Zero if the necessary sensor information is not available.
   * @see ConfigEncoderCodesPerRev
   * @see ConfigPotentiometerTurns
   */
  double GetNativeUnitsPerRotationScalar(FeedbackDevice devToLookup)const;
  /**
   * Fixup the sendMode so Set() serializes the correct demand value.
   * Also fills the modeSelecet in the control frame to disabled.
   * @param mode Control mode to ultimately enter once user calls Set().
   * @see Set()
   */
  void ApplyControlMode(CANSpeedController::ControlMode mode);
  /**
   * @param fullRotations 	double precision value representing number of rotations of selected feedback sensor.
   *							If user has never called the config routine for the selected sensor, then the caller
   *							is likely passing rotations in engineering units already, in which case it is returned
   *							as is.
   *							@see ConfigPotentiometerTurns
   *							@see ConfigEncoderCodesPerRev
   * @return fullRotations in native engineering units of the Talon SRX firmware.
   */
  int32_t ScaleRotationsToNativeUnits(FeedbackDevice devToLookup, double fullRotations) const;
  /**
   * @param rpm 	double precision value representing number of rotations per minute of selected feedback sensor.
   *							If user has never called the config routine for the selected sensor, then the caller
   *							is likely passing rotations in engineering units already, in which case it is returned
   *							as is.
   *							@see ConfigPotentiometerTurns
   *							@see ConfigEncoderCodesPerRev
   * @return sensor velocity in native engineering units of the Talon SRX firmware.
   */
  int32_t ScaleVelocityToNativeUnits(FeedbackDevice devToLookup, double rpm) const;
  /**
   * @param nativePos 	integral position of the feedback sensor in native Talon SRX units.
   *							If user has never called the config routine for the selected sensor, then the return
   *							will be in TALON SRX units as well to match the behavior in the 2015 season.
   *							@see ConfigPotentiometerTurns
   *							@see ConfigEncoderCodesPerRev
   * @return double precision number of rotations, unless config was never performed.
   */
  double ScaleNativeUnitsToRotations(FeedbackDevice devToLookup, int32_t nativePos) const;
  /**
   * @param nativeVel 	integral velocity of the feedback sensor in native Talon SRX units.
   *							If user has never called the config routine for the selected sensor, then the return
   *							will be in TALON SRX units as well to match the behavior in the 2015 season.
   *							@see ConfigPotentiometerTurns
   *							@see ConfigEncoderCodesPerRev
   * @return double precision of sensor velocity in RPM, unless config was never performed.
   */
  double ScaleNativeUnitsToRpm(FeedbackDevice devToLookup, int32_t nativeVel) const;

  // LiveWindow stuff.
  std::shared_ptr<ITable> m_table;
  bool m_isInverted;

  HasBeenMoved m_hasBeenMoved;
};
