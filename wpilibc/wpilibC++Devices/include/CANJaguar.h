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
#include "HAL/Semaphore.hpp"
#include "HAL/HAL.hpp"
#include "LiveWindow/LiveWindowSendable.h"
#include "tables/ITable.h"
#include "NetworkCommunication/CANSessionMux.h"

#include <atomic>
#include <mutex>
#include <utility>

/**
 * Luminary Micro / Vex Robotics Jaguar Speed Control
 */
class CANJaguar : public MotorSafety,
					public CANSpeedController,
					public ErrorBase,
					public LiveWindowSendable,
					public ITableListener
{
public:
	// The internal PID control loop in the Jaguar runs at 1kHz.
	static const int32_t kControllerRate = 1000;
	static constexpr double kApproxBusVoltage = 12.0;

	// Control mode tags
	/** Sets an encoder as the speed reference only. <br> Passed as the "tag" when setting the control mode.*/
	static const struct EncoderStruct       {} Encoder;
	/** Sets a quadrature encoder as the position and speed reference. <br> Passed as the "tag" when setting the control mode.*/
	static const struct QuadEncoderStruct   {} QuadEncoder;
	/** Sets a potentiometer as the position reference only. <br> Passed as the "tag" when setting the control mode. */
	static const struct PotentiometerStruct {} Potentiometer;

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
	void SetCurrentMode(EncoderStruct, uint16_t codesPerRev, double p, double i, double d);
	void SetCurrentMode(QuadEncoderStruct, uint16_t codesPerRev, double p, double i, double d);
	void SetCurrentMode(PotentiometerStruct, double p, double i, double d);

	void SetSpeedMode(EncoderStruct, uint16_t codesPerRev, double p, double i, double d);
	void SetSpeedMode(QuadEncoderStruct, uint16_t codesPerRev, double p, double i, double d);

	void SetPositionMode(QuadEncoderStruct, uint16_t codesPerRev, double p, double i, double d);
	void SetPositionMode(PotentiometerStruct, double p, double i, double d);

	void SetVoltageMode();
	void SetVoltageMode(EncoderStruct, uint16_t codesPerRev);
	void SetVoltageMode(QuadEncoderStruct, uint16_t codesPerRev);
	void SetVoltageMode(PotentiometerStruct);

	// CANSpeedController interface
	virtual float Get() const override;
	virtual void Set(float value, uint8_t syncGroup=0) override;
	virtual void Disable() override;
	virtual void SetP(double p) override;
	virtual void SetI(double i) override;
	virtual void SetD(double d) override;
	virtual void SetPID(double p, double i, double d) override;
	virtual double GetP() const override;
	virtual double GetI() const override;
	virtual double GetD() const override;
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
	virtual void ConfigSoftPositionLimits(double forwardLimitPosition, double reverseLimitPosition) override;
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
	void GetDescription(char *desc) const override;
	uint8_t GetDeviceID() const;

  //SpeedController overrides
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

	void sendMessage(uint32_t messageID, const uint8_t *data, uint8_t dataSize, int32_t period = CAN_SEND_PERIOD_NO_REPEAT);
	void requestMessage(uint32_t messageID, int32_t period = CAN_SEND_PERIOD_NO_REPEAT);
	bool getMessage(uint32_t messageID, uint32_t mask, uint8_t *data, uint8_t *dataSize) const;

	void setupPeriodicStatus();
	void updatePeriodicStatus() const;

	mutable std::recursive_mutex m_mutex;

	uint8_t m_deviceNumber;
	float m_value;

	// Parameters/configuration
	ControlMode m_controlMode;
	uint8_t m_speedReference;
	uint8_t m_positionReference;
	double m_p;
	double m_i;
	double m_d;
	NeutralMode m_neutralMode;
	uint16_t m_encoderCodesPerRev;
	uint16_t m_potentiometerTurns;
	LimitMode m_limitMode;
	double m_forwardLimit;
	double m_reverseLimit;
	double m_maxOutputVoltage;
	double m_voltageRampRate;
	float m_faultTime;

	// Which parameters have been verified since they were last set?
	bool m_controlModeVerified;
	bool m_speedRefVerified;
	bool m_posRefVerified;
	bool m_pVerified;
	bool m_iVerified;
	bool m_dVerified;
	bool m_neutralModeVerified;
	bool m_encoderCodesPerRevVerified;
	bool m_potentiometerTurnsVerified;
	bool m_forwardLimitVerified;
	bool m_reverseLimitVerified;
	bool m_limitModeVerified;
	bool m_maxOutputVoltageVerified;
	bool m_voltageRampRateVerified;
	bool m_faultTimeVerified;

	// Status data
	mutable float m_busVoltage;
	mutable float m_outputVoltage;
	mutable float m_outputCurrent;
	mutable float m_temperature;
	mutable double m_position;
	mutable double m_speed;
	mutable uint8_t m_limits;
	mutable uint16_t m_faults;
	uint32_t m_firmwareVersion;
	uint8_t m_hardwareVersion;

	// Which periodic status messages have we received at least once?
	mutable std::atomic<bool> m_receivedStatusMessage0;
	mutable std::atomic<bool> m_receivedStatusMessage1;
	mutable std::atomic<bool> m_receivedStatusMessage2;

	bool m_controlEnabled;

	void verify();

	MotorSafetyHelper *m_safetyHelper;

	void ValueChanged(ITable* source, const std::string& key, EntryValue value, bool isNew) override;
	void UpdateTable() override;
	void StartLiveWindowMode() override;
	void StopLiveWindowMode() override;
	std::string GetSmartDashboardType() const override;
	void InitTable(ITable *subTable) override;
	ITable * GetTable() const override;

	ITable *m_table;

private:
	void InitCANJaguar();
	bool m_isInverted;
};
