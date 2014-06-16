/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2009. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/
#pragma once

#include "ErrorBase.h"
#include "MotorSafety.h"
#include "MotorSafetyHelper.h"
#include "PIDOutput.h"
#include "SpeedController.h"
#include "HAL/Semaphore.hpp"
#include "HAL/HAL.hpp"
#include "LiveWindow/LiveWindowSendable.h"
#include "tables/ITable.h"

#include <utility>

/**
 * Luminary Micro Jaguar Speed Control
 */
class CANJaguar : public MotorSafety,
					public SpeedController,
					public ErrorBase,
					public LiveWindowSendable,
					public ITableListener
{
public:
	// The internal PID control loop in the Jaguar runs at 1kHz.
	static const int32_t kControllerRate = 1000;
	static constexpr double kApproxBusVoltage = 12.0;

	typedef enum {kPercentVbus, kCurrent, kSpeed, kPosition, kVoltage} ControlMode;
	typedef enum {kCurrentFault = 1, kTemperatureFault = 2, kBusVoltageFault = 4, kGateDriverFault = 8} Faults;
	typedef enum {kForwardLimit = 1, kReverseLimit = 2} Limits;
	typedef enum {kPosRef_QuadEncoder = 0, kPosRef_Potentiometer = 1, kPosRef_None = 0xFF} PositionReference;
	typedef enum {kSpeedRef_Encoder = 0, kSpeedRef_InvEncoder = 2, kSpeedRef_QuadEncoder = 3, kSpeedRef_None = 0xFF} SpeedReference;
	typedef enum {kNeutralMode_Jumper = 0, kNeutralMode_Brake = 1, kNeutralMode_Coast = 2} NeutralMode;
	typedef enum {kLimitMode_SwitchInputsOnly = 0, kLimitMode_SoftPositionLimits = 1} LimitMode;

	explicit CANJaguar(uint8_t deviceNumber, ControlMode controlMode = kPercentVbus);
	virtual ~CANJaguar();

	// SpeedController interface
	virtual float Get();
	virtual void Set(float value, uint8_t syncGroup=0);
	virtual void Disable();

	// PIDOutput interface
	virtual void PIDWrite(float output);

	// Other Accessors
	void SetSpeedReference(SpeedReference reference);
	SpeedReference GetSpeedReference();
	void SetPositionReference(PositionReference reference);
	PositionReference GetPositionReference();
	void SetPID(double p, double i, double d);
	double GetP();
	double GetI();
	double GetD();
	void EnableControl(double encoderInitialPosition = 0.0);
	void DisableControl();
	void ChangeControlMode(ControlMode controlMode);
	ControlMode GetControlMode();
	float GetBusVoltage();
	float GetOutputVoltage();
	float GetOutputCurrent();
	float GetTemperature();
	double GetPosition();
	double GetSpeed();
	bool GetForwardLimitOK();
	bool GetReverseLimitOK();
	uint16_t GetFaults();
	void SetVoltageRampRate(double rampRate);
	virtual uint32_t GetFirmwareVersion();
	uint8_t GetHardwareVersion();
	void ConfigNeutralMode(NeutralMode mode);
	void ConfigEncoderCodesPerRev(uint16_t codesPerRev);
	void ConfigPotentiometerTurns(uint16_t turns);
	void ConfigSoftPositionLimits(double forwardLimitPosition, double reverseLimitPosition);
	void DisableSoftPositionLimits();
	void ConfigMaxOutputVoltage(double voltage);
	void ConfigFaultTime(float faultTime);

	static void UpdateSyncGroup(uint8_t syncGroup);

	void SetExpiration(float timeout);
	float GetExpiration();
	bool IsAlive();
	void StopMotor();
	bool IsSafetyEnabled();
	void SetSafetyEnabled(bool enabled);
	void GetDescription(char *desc);

protected:
	uint8_t packPercentage(uint8_t *buffer, double value);
	uint8_t packFXP8_8(uint8_t *buffer, double value);
	uint8_t packFXP16_16(uint8_t *buffer, double value);
	uint8_t packint16_t(uint8_t *buffer, int16_t value);
	uint8_t packint32_t(uint8_t *buffer, int32_t value);
	double unpackPercentage(uint8_t *buffer);
	double unpackFXP8_8(uint8_t *buffer);
	double unpackFXP16_16(uint8_t *buffer);
	int16_t unpackint16_t(uint8_t *buffer);
	int32_t unpackint32_t(uint8_t *buffer);
	virtual void setTransaction(uint32_t messageID, const uint8_t *data, uint8_t dataSize);
	virtual bool getTransaction(uint32_t messageID, uint8_t *data, uint8_t *dataSize);

	static int32_t sendMessage(uint32_t messageID, const uint8_t *data, uint8_t dataSize);
	static int32_t receiveMessage(uint32_t *messageID, uint8_t *data, uint8_t *dataSize);

	uint8_t m_deviceNumber;
	ControlMode m_controlMode;
	MUTEX_ID m_transactionSemaphore;
	double m_maxOutputVoltage;

    enum CANValue {
        CAN_VALUE = 0x1,
        CAN_SPEED_REFERENCE = 0x2,
        CAN_POSITION_REFERENCE = 0x4,
        CAN_P = 0x8, CAN_I = 0x10, CAN_D = 0x20,
        CAN_BUS_VOLTAGE = 0x40,
        CAN_OUTPUT_VOLTAGE = 0x80,
        CAN_OUTPUT_CURRENT = 0x100,
        CAN_TEMPERATURE = 0x200,
        CAN_POSITION = 0x400,
        CAN_SPEED = 0x800,
        CAN_LIMITS = 0x1000,
        CAN_FAULTS = 0x2000,
        CAN_FIRMWARE_VERSION = 0x4000,
        CAN_HARDWARE_VERSION = 0x8000,
        CAN_EVERYTHING = 0xffff
    };

    bool isUnverified(CANValue value) const;
    void verifyCANValues();

    // Keep track of what cached CAN values have been verified.
    CANValue m_verified_values;

    // Cached CAN data
    float m_value;
    SpeedReference m_speedReference;
    PositionReference m_positionReference;
    double m_p, m_i, m_d;
    float m_busVoltage, m_outputVoltage, m_outputCurrent, m_temperature;
    double m_position, m_speed;
    uint8_t m_limits;
    uint16_t m_faults;
    uint32_t m_firmwareVersion;
    uint8_t m_hardwareVersion;

	MotorSafetyHelper *m_safetyHelper;

	void ValueChanged(ITable* source, const std::string& key, EntryValue value, bool isNew);
	void UpdateTable();
	void StartLiveWindowMode();
	void StopLiveWindowMode();
	std::string GetSmartDashboardType();
	void InitTable(ITable *subTable);
	ITable * GetTable();

	ITable *m_table;

private:
	void InitCANJaguar();
};
