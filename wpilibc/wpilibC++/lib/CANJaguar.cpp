/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2009. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "CANJaguar.h"
#include "Timer.h"
#define tNIRIO_i32 int
#include "NetworkCommunication/CANSessionMux.h"
#include "CAN/can_proto.h"
//#include "NetworkCommunication/UsageReporting.h"
#include "WPIErrors.h"
#include <cstdio>
#include <cassert>
#include "LiveWindow/LiveWindow.h"

/* we are on ARM-LE now, not Freescale so no need to swap */
//TODO: is this defined in a PN way? or is this Jag-specific?
#define swap16(x)	(x)
#define swap32(x)	(x)

/* Compare floats for equality as fixed point numbers */
#define FXP8_EQ(a,b) ((int16_t)((a)*256.0)==(int16_t)((b)*256.0))
#define FXP16_EQ(a,b) ((int16_t)((a)*65536.0)==(int16_t)((b)*65536.0))

const int32_t CANJaguar::kControllerRate;
constexpr double CANJaguar::kApproxBusVoltage;

static const int32_t kSendMessagePeriod = 20;
static const uint32_t kFullMessageIDMask = (CAN_MSGID_API_M | CAN_MSGID_MFR_M | CAN_MSGID_DTYPE_M);

static int32_t sendMessageHelper(uint32_t messageID, const uint8_t *data, uint8_t dataSize, int32_t period)
{
	static const uint32_t kTrustedMessages[] = {
			LM_API_VOLT_T_EN, LM_API_VOLT_T_SET, LM_API_SPD_T_EN, LM_API_SPD_T_SET,
			LM_API_VCOMP_T_EN, LM_API_VCOMP_T_SET, LM_API_POS_T_EN, LM_API_POS_T_SET,
			LM_API_ICTRL_T_EN, LM_API_ICTRL_T_SET};

	int32_t status=0;

	for (uint8_t i=0; i<(sizeof(kTrustedMessages)/sizeof(kTrustedMessages[0])); i++)
	{
		if ((kFullMessageIDMask & messageID) == kTrustedMessages[i])
		{
			uint8_t dataBuffer[8];
			dataBuffer[0] = 0;
			dataBuffer[1] = 0;

			// Make sure the data will still fit after adjusting for the token.
			assert(dataSize <= 6);

			for (uint8_t j=0; j < dataSize; j++)
			{
				dataBuffer[j + 2] = data[j];
			}

			FRC_NetworkCommunication_CANSessionMux_sendMessage(messageID, dataBuffer, dataSize + 2, period, &status);

			return status;
		}
	}

	FRC_NetworkCommunication_CANSessionMux_sendMessage(messageID, data, dataSize, period, &status);

	return status;
}

/**
 * Common initialization code called by all constructors.
 */
void CANJaguar::InitCANJaguar()
{
	m_table = NULL;
	m_safetyHelper = new MotorSafetyHelper(this);

	m_value = 0.0f;
	m_speedReference = kSpeedRef_None;
	m_positionReference = kPosRef_None;
	m_p = 0.0;
	m_i = 0.0;
	m_d = 0.0;
	m_busVoltage = 0.0f;
	m_outputVoltage = 0.0f;
	m_outputCurrent = 0.0f;
	m_temperature = 0.0f;
	m_position = 0.0;
	m_speed = 0.0;
	m_limits = 0x00;
	m_faults = 0x0000;
	m_firmwareVersion = 0;
	m_hardwareVersion = 0;
	m_neutralMode = kNeutralMode_Jumper;
	m_encoderCodesPerRev = 0;
	m_potentiometerTurns = 0;
	m_limitMode = kLimitMode_SwitchInputsOnly;
	m_forwardLimit = 0.0;
	m_reverseLimit = 0.0;
	m_maxOutputVoltage = 30.0;
	m_voltageRampRate = 0.0;
	m_faultTime = 0.0f;

	// Parameters only need to be verified if they are set
	m_controlModeVerified = false; // Needs to be verified because it's set in the constructor
	m_speedRefVerified = true;
	m_posRefVerified = true;
	m_pVerified = true;
	m_iVerified = true;
	m_dVerified = true;
	m_neutralModeVerified = true;
	m_encoderCodesPerRevVerified = true;
	m_potentiometerTurnsVerified = true;
	m_limitModeVerified = true;
	m_forwardLimitVerified = true;
	m_reverseLimitVerified = true;
	m_maxOutputVoltageVerified = true;
	m_voltageRampRateVerified = true;
	m_faultTimeVerified = true;

	uint8_t dataBuffer[8];
	uint8_t dataSize;

	// Request all status data periodically
	requestMessage(LM_API_STATUS_VOLTBUS, kSendMessagePeriod);
	requestMessage(LM_API_STATUS_VOUT, kSendMessagePeriod);
	requestMessage(LM_API_STATUS_CURRENT, kSendMessagePeriod);
	requestMessage(LM_API_STATUS_TEMP, kSendMessagePeriod);
	requestMessage(LM_API_STATUS_POS, kSendMessagePeriod);
	requestMessage(LM_API_STATUS_SPD, kSendMessagePeriod);
	requestMessage(LM_API_STATUS_LIMIT, kSendMessagePeriod);
	requestMessage(LM_API_STATUS_FAULT, kSendMessagePeriod);
	requestMessage(LM_API_STATUS_POWER, kSendMessagePeriod);

	// Request firmware and hardware version only once
	requestMessage(CAN_IS_FRAME_REMOTE | CAN_MSGID_API_FIRMVER);
	requestMessage(LM_API_HWVER);

	if(getMessage(CAN_MSGID_API_FIRMVER, CAN_MSGID_FULL_M, dataBuffer, &dataSize))
		m_firmwareVersion = unpackint32_t(dataBuffer);
	else
		wpi_setWPIErrorWithContext(JaguarMessageNotFound, "getMessage");

	if(getMessage(LM_API_HWVER, CAN_MSGID_FULL_M, dataBuffer, &dataSize))
		m_hardwareVersion = dataBuffer[0];

	if (m_deviceNumber < 1 || m_deviceNumber > 63)
	{
		char buf[256];
		snprintf(buf, 256, "device number \"%d\" must be between 1 and 63", m_deviceNumber);
		wpi_setWPIErrorWithContext(ParameterOutOfRange, buf);
		return;
	}

	if (StatusIsFatal())
		return;

	// 3330 was the first shipping RDK firmware version for the Jaguar
	if (m_firmwareVersion >= 3330 || m_firmwareVersion < 101)
	{
		char buf[256];
		if (m_firmwareVersion < 3330)
		{
			snprintf(buf, 256, "Jag #%d firmware (%d) is too old (must be at least version 101 of the FIRST approved firmware)", m_deviceNumber, m_firmwareVersion);
		}
		else
		{
			snprintf(buf, 256, "Jag #%d firmware (%d) is not FIRST approved (must be at least version 101 of the FIRST approved firmware)", m_deviceNumber, m_firmwareVersion);
		}
		wpi_setWPIErrorWithContext(JaguarVersionError, buf);
		return;
	}

	switch (m_controlMode)
	{
	case kPercentVbus:
	case kVoltage:
		// No additional configuration required... start enabled.
		EnableControl();
		break;
	default:
		break;
	}

	HALReport(HALUsageReporting::kResourceType_CANJaguar, m_deviceNumber, m_controlMode);
	LiveWindow::GetInstance()->AddActuator("CANJaguar", m_deviceNumber, this);
}

/**
 * Constructor
 *
 * @param deviceNumber The the address of the Jaguar on the CAN bus.
 */
CANJaguar::CANJaguar(uint8_t deviceNumber, ControlMode controlMode)
	: m_deviceNumber (deviceNumber)
	, m_controlMode (controlMode)
	, m_maxOutputVoltage (kApproxBusVoltage)
	, m_safetyHelper (NULL)
{
	InitCANJaguar();

}

CANJaguar::~CANJaguar()
{
	int32_t status;

	// Disable periodic setpoints
	if(m_controlMode == kPercentVbus)
		FRC_NetworkCommunication_CANSessionMux_sendMessage(m_deviceNumber | LM_API_VOLT_T_SET, NULL, 0, CAN_SEND_PERIOD_STOP_REPEATING, &status);
	else if(m_controlMode == kSpeed)
		FRC_NetworkCommunication_CANSessionMux_sendMessage(m_deviceNumber | LM_API_SPD_T_SET, NULL, 0, CAN_SEND_PERIOD_STOP_REPEATING, &status);
	else if(m_controlMode == kPosition)
		FRC_NetworkCommunication_CANSessionMux_sendMessage(m_deviceNumber | LM_API_POS_T_SET, NULL, 0, CAN_SEND_PERIOD_STOP_REPEATING, &status);
	else if(m_controlMode == kCurrent)
		FRC_NetworkCommunication_CANSessionMux_sendMessage(m_deviceNumber | LM_API_ICTRL_T_SET, NULL, 0, CAN_SEND_PERIOD_STOP_REPEATING, &status);
	else if(m_controlMode == kVoltage)
		FRC_NetworkCommunication_CANSessionMux_sendMessage(m_deviceNumber | LM_API_VCOMP_T_SET, NULL, 0, CAN_SEND_PERIOD_STOP_REPEATING, &status);

	delete m_safetyHelper;
	m_safetyHelper = NULL;
}

/**
 * Set the output set-point value.
 *
 * The scale and the units depend on the mode the Jaguar is in.
 * In PercentVbus Mode, the outputValue is from -1.0 to 1.0 (same as PWM Jaguar).
 * In Voltage Mode, the outputValue is in Volts.
 * In Current Mode, the outputValue is in Amps.
 * In Speed Mode, the outputValue is in Rotations/Minute.
 * In Position Mode, the outputValue is in Rotations.
 *
 * @param outputValue The set-point to sent to the motor controller.
 * @param syncGroup The update group to add this Set() to, pending UpdateSyncGroup().  If 0, update immediately.
 */
void CANJaguar::Set(float outputValue, uint8_t syncGroup)
{
	uint32_t messageID;
	uint8_t dataBuffer[8];
	uint8_t dataSize;

	if (m_safetyHelper && !m_safetyHelper->IsAlive())
	{
		EnableControl();
	}

	switch(m_controlMode)
	{
	case kPercentVbus:
		{
			messageID = LM_API_VOLT_T_SET;
			if (outputValue > 1.0) outputValue = 1.0;
			if (outputValue < -1.0) outputValue = -1.0;
			dataSize = packPercentage(dataBuffer, outputValue);
		}
		break;
	case kSpeed:
		{
			messageID = LM_API_SPD_T_SET;
			dataSize = packFXP16_16(dataBuffer, outputValue);
		}
		break;
	case kPosition:
		{
			messageID = LM_API_POS_T_SET;
			dataSize = packFXP16_16(dataBuffer, outputValue);
		}
		break;
	case kCurrent:
		{
			messageID = LM_API_ICTRL_T_SET;
			dataSize = packFXP8_8(dataBuffer, outputValue);
		}
		break;
	case kVoltage:
		{
			messageID = LM_API_VCOMP_T_SET;
			dataSize = packFXP8_8(dataBuffer, outputValue);
		}
		break;
	default:
		return;
	}
	if (syncGroup != 0)
	{
		dataBuffer[dataSize] = syncGroup;
		dataSize++;
	}

	sendMessage(messageID, dataBuffer, dataSize, kSendMessagePeriod);

	if (m_safetyHelper) m_safetyHelper->Feed();

	m_value = outputValue;

	verify();
}

/**
 * Get the recently set outputValue setpoint.
 *
 * The scale and the units depend on the mode the Jaguar is in.
 * In PercentVbus Mode, the outputValue is from -1.0 to 1.0 (same as PWM Jaguar).
 * In Voltage Mode, the outputValue is in Volts.
 * In Current Mode, the outputValue is in Amps.
 * In Speed Mode, the outputValue is in Rotations/Minute.
 * In Position Mode, the outputValue is in Rotations.
 *
 * @return The most recently set outputValue setpoint.
 */
float CANJaguar::Get()
{
	return m_value;
}

/**
 * Common interface for disabling a motor.
 *
 * @deprecated Call DisableControl instead.
 */
void CANJaguar::Disable()
{
	DisableControl();
}

/**
 * Write out the PID value as seen in the PIDOutput base object.
 *
 * @deprecated Call Set instead.
 *
 * @param output Write out the PercentVbus value as was computed by the PIDController
 */
void CANJaguar::PIDWrite(float output)
{
	if (m_controlMode == kPercentVbus)
	{
		Set(output);
	}
	else
	{
		wpi_setWPIErrorWithContext(IncompatibleMode, "PID only supported in PercentVbus mode");
	}
}

uint8_t CANJaguar::packPercentage(uint8_t *buffer, double value)
{
	int16_t intValue = (int16_t)(value * 32767.0);
	*((int16_t*)buffer) = swap16(intValue);
	return sizeof(int16_t);
}

uint8_t CANJaguar::packFXP8_8(uint8_t *buffer, double value)
{
	int16_t intValue = (int16_t)(value * 256.0);
	*((int16_t*)buffer) = swap16(intValue);
	return sizeof(int16_t);
}

uint8_t CANJaguar::packFXP16_16(uint8_t *buffer, double value)
{
	int32_t intValue = (int32_t)(value * 65536.0);
	*((int32_t*)buffer) = swap32(intValue);
	return sizeof(int32_t);
}

uint8_t CANJaguar::packint16_t(uint8_t *buffer, int16_t value)
{
	*((int16_t*)buffer) = swap16(value);
	return sizeof(int16_t);
}

uint8_t CANJaguar::packint32_t(uint8_t *buffer, int32_t value)
{
	*((int32_t*)buffer) = swap32(value);
	return sizeof(int32_t);
}

double CANJaguar::unpackPercentage(uint8_t *buffer)
{
	int16_t value = *((int16_t*)buffer);
	value = swap16(value);
	return value / 32767.0;
}

double CANJaguar::unpackFXP8_8(uint8_t *buffer)
{
	int16_t value = *((int16_t*)buffer);
	value = swap16(value);
	return value / 256.0;
}

double CANJaguar::unpackFXP16_16(uint8_t *buffer)
{
	int32_t value = *((int32_t*)buffer);
	value = swap32(value);
	return value / 65536.0;
}

int16_t CANJaguar::unpackint16_t(uint8_t *buffer)
{
	int16_t value = *((int16_t*)buffer);
	return swap16(value);
}

int32_t CANJaguar::unpackint32_t(uint8_t *buffer)
{
	int32_t value = *((int32_t*)buffer);
	return swap32(value);
}

/**
 * Send a message to the Jaguar.
 *
 * @param messageID The messageID to be used on the CAN bus (device number is added internally)
 * @param data The up to 8 bytes of data to be sent with the message
 * @param dataSize Specify how much of the data in "data" to send
 * @param periodic If true, tell NetworkCommunications to send the package every
 * 20 ms.
 */
void CANJaguar::sendMessage(uint32_t messageID, const uint8_t *data, uint8_t dataSize, int32_t period)
{
	int32_t localStatus = sendMessageHelper(messageID | m_deviceNumber, data, dataSize, period);

	if(localStatus < 0)
	{
		wpi_setErrorWithContext(localStatus, "sendMessage");
	}
}

/**
 * Request a message from the Jaguar, but don't wait for it to arrive.
 *
 * @param messageID The message to request
 */
void CANJaguar::requestMessage(uint32_t messageID, int32_t period)
{
	sendMessageHelper(messageID | m_deviceNumber, NULL, 0, period);
}

/**
 * Get a previously requested message.
 *
 * Jaguar always generates a message with the same message ID when replying.
 *
 * @param messageID The messageID to read from the CAN bus (device number is added internally)
 * @param data The up to 8 bytes of data that was received with the message
 * @param dataSize Indicates how much data was received
 *
 * @return true if the message was found.  Otherwise, no new message is available.
 */
bool CANJaguar::getMessage(uint32_t messageID, uint32_t messageMask, uint8_t *data, uint8_t *dataSize)
{
	uint32_t targetedMessageID = messageID | m_deviceNumber;
	int32_t status = 0;
	uint32_t timeStamp;

	// Caller may have set bit31 for remote frame transmission so clear invalid bits[31-29]
	targetedMessageID &= CAN_MSGID_FULL_M;

	// Get the data.
	FRC_NetworkCommunication_CANSessionMux_receiveMessage(&targetedMessageID, messageMask, data, dataSize, &timeStamp, &status);

	// Do we already have the most recent value?
	if(status == ERR_CANSessionMux_MessageNotFound)
		return false;
	else
		wpi_setErrorWithContext(status, "receiveMessage");

	return true;
}

/**
 * Check all unverified params and make sure they're equal to their local
 * cached versions. If a value isn't available, it gets requested.  If a value
 * doesn't match up, it gets set again.
 */
void CANJaguar::verify()
{
	uint8_t dataBuffer[8];
	uint8_t dataSize;

	// If the Jaguar lost power, everything should be considered unverified.
	if(getMessage(LM_API_STATUS_POWER, CAN_MSGID_FULL_M, dataBuffer, &dataSize))
	{
		bool powerCycled = (bool)dataBuffer[0];

		if(powerCycled)
		{
			// Clear the power cycled bit
			dataBuffer[0] = 1;
			sendMessage(LM_API_STATUS_POWER, dataBuffer, sizeof(uint8_t));

			// Set all configuration data again.  This will resend it to the
			// Jaguar and mark everything as unverified.
			EnableControl();
			SetSpeedReference(m_speedReference);
			SetPositionReference(m_positionReference);
			ConfigNeutralMode(m_neutralMode);
			ConfigEncoderCodesPerRev(m_encoderCodesPerRev);
			ConfigPotentiometerTurns(m_potentiometerTurns);

			if(m_controlMode == kCurrent || m_controlMode == kSpeed || m_controlMode == kPosition)
				SetPID(m_p, m_i, m_d);

			if(m_limitMode == kLimitMode_SoftPositionLimits)
				ConfigSoftPositionLimits(m_forwardLimit, m_reverseLimit);
			else
				DisableSoftPositionLimits();

			ConfigMaxOutputVoltage(m_maxOutputVoltage);

			if(m_controlMode == kVoltage || m_controlMode == kPercentVbus)
				SetVoltageRampRate(m_voltageRampRate);

			requestMessage(LM_API_STATUS_VOLTBUS, kSendMessagePeriod);
			requestMessage(LM_API_STATUS_VOUT, kSendMessagePeriod);
			requestMessage(LM_API_STATUS_CURRENT, kSendMessagePeriod);
			requestMessage(LM_API_STATUS_TEMP, kSendMessagePeriod);
			requestMessage(LM_API_STATUS_POS, kSendMessagePeriod);
			requestMessage(LM_API_STATUS_SPD, kSendMessagePeriod);
			requestMessage(LM_API_STATUS_LIMIT, kSendMessagePeriod);
			requestMessage(LM_API_STATUS_FAULT, kSendMessagePeriod);
			requestMessage(LM_API_STATUS_POWER, kSendMessagePeriod);
		}
	}

	// Verify that any recently set parameters are correct
	if(!m_controlModeVerified)
	{
		if(getMessage(LM_API_STATUS_CMODE, CAN_MSGID_FULL_M, dataBuffer, &dataSize))
		{
			ControlMode mode = (ControlMode)dataBuffer[0];

			if(m_controlMode == mode)
				m_controlModeVerified = true;
			else
				// Enable control again to resend the control mode
				EnableControl();
		}
		else
		{
			// Verification is needed but not available - request it again.
			requestMessage(LM_API_STATUS_CMODE);
		}
	}

	if(!m_speedRefVerified)
	{
		if(getMessage(LM_API_SPD_REF, CAN_MSGID_FULL_M, dataBuffer, &dataSize))
		{
			SpeedReference speedRef = (SpeedReference)dataBuffer[0];

			if(m_speedReference == speedRef)
				m_speedRefVerified = true;
			else
				// It's wrong - set it again
				SetSpeedReference(m_speedReference);
		}
		else
		{
			// Verification is needed but not available - request it again.
			requestMessage(LM_API_SPD_REF);
		}
	}

	if(!m_posRefVerified)
	{
		if(getMessage(LM_API_POS_REF, CAN_MSGID_FULL_M, dataBuffer, &dataSize))
		{
			PositionReference posRef = (PositionReference)dataBuffer[0];

			if(m_positionReference == posRef)
				m_posRefVerified = true;
			else
				// It's wrong - set it again
				SetPositionReference(m_positionReference);
		}
		else
		{
			// Verification is needed but not available - request it again.
			requestMessage(LM_API_POS_REF);
		}
	}

	if(!m_pVerified)
	{
		uint32_t message;

		if(m_controlMode == kSpeed)
			message = LM_API_SPD_PC;
		else if(m_controlMode == kPosition)
			message = LM_API_POS_PC;
		else if(m_controlMode == kCurrent)
			message = LM_API_ICTRL_PC;

		if(getMessage(message, CAN_MSGID_FULL_M, dataBuffer, &dataSize))
		{
			double p = unpackFXP16_16(dataBuffer);

			if(FXP16_EQ(m_p, p))
				m_pVerified = true;
			else
				// It's wrong - set it again
				SetP(m_p);
		}
		else
		{
			// Verification is needed but not available - request it again.
			requestMessage(message);
		}
	}

	if(!m_iVerified)
	{
		uint32_t message;

		if(m_controlMode == kSpeed)
			message = LM_API_SPD_IC;
		else if(m_controlMode == kPosition)
			message = LM_API_POS_IC;
		else if(m_controlMode == kCurrent)
			message = LM_API_ICTRL_IC;

		if(getMessage(message, CAN_MSGID_FULL_M, dataBuffer, &dataSize))
		{
			double i = unpackFXP16_16(dataBuffer);

			if(FXP16_EQ(m_i, i))
				m_iVerified = true;
			else
				// It's wrong - set it again
				SetI(m_i);
		}
		else
		{
			// Verification is needed but not available - request it again.
			requestMessage(message);
		}
	}

	if(!m_dVerified)
	{
		uint32_t message;

		if(m_controlMode == kSpeed)
			message = LM_API_SPD_DC;
		else if(m_controlMode == kPosition)
			message = LM_API_POS_DC;
		else if(m_controlMode == kCurrent)
			message = LM_API_ICTRL_DC;

		if(getMessage(message, CAN_MSGID_FULL_M, dataBuffer, &dataSize))
		{
			double d = unpackFXP16_16(dataBuffer);

			if(FXP16_EQ(m_d, d))
				m_dVerified = true;
			else
				// It's wrong - set it again
				SetD(m_d);
		}
		else
		{
			// Verification is needed but not available - request it again.
			requestMessage(message);
		}
	}

	if(!m_neutralModeVerified)
	{
		if(getMessage(LM_API_CFG_BRAKE_COAST, CAN_MSGID_FULL_M, dataBuffer, &dataSize))
		{
			NeutralMode mode = (NeutralMode)dataBuffer[0];

			if(mode == m_neutralMode)
				m_neutralModeVerified = true;
			else
				// It's wrong - set it again
				ConfigNeutralMode(m_neutralMode);
		}
		else
		{
			// Verification is needed but not available - request it again.
			requestMessage(LM_API_CFG_BRAKE_COAST);
		}
	}

	if(!m_encoderCodesPerRevVerified)
	{
		if(getMessage(LM_API_CFG_ENC_LINES, CAN_MSGID_FULL_M, dataBuffer, &dataSize))
		{
			uint16_t codes = unpackint16_t(dataBuffer);

			if(codes == m_encoderCodesPerRev)
				m_encoderCodesPerRevVerified = true;
			else
				// It's wrong - set it again
				ConfigEncoderCodesPerRev(m_encoderCodesPerRev);
		}
		else
		{
			// Verification is needed but not available - request it again.
			requestMessage(LM_API_CFG_ENC_LINES);
		}
	}

	if(!m_potentiometerTurnsVerified)
	{
		if(getMessage(LM_API_CFG_POT_TURNS, CAN_MSGID_FULL_M, dataBuffer, &dataSize))
		{
			uint16_t turns = unpackint16_t(dataBuffer);

			if(turns == m_potentiometerTurns)
				m_potentiometerTurnsVerified = true;
			else
				// It's wrong - set it again
				ConfigPotentiometerTurns(m_potentiometerTurns);
		}
		else
		{
			// Verification is needed but not available - request it again.
			requestMessage(LM_API_CFG_POT_TURNS);
		}
	}

	if(!m_limitModeVerified)
	{
		if(getMessage(LM_API_CFG_LIMIT_MODE, CAN_MSGID_FULL_M, dataBuffer, &dataSize))
		{
			LimitMode mode = (LimitMode)dataBuffer[0];

			if(mode == m_limitMode)
				m_limitModeVerified = true;
			else
			{
				// It's wrong - set it again
				ConfigLimitMode(m_limitMode);
			}
		}
		else
		{
			// Verification is needed but not available - request it again.
			requestMessage(LM_API_CFG_LIMIT_MODE);
		}
	}

	if(!m_forwardLimitVerified)
	{
		if(getMessage(LM_API_CFG_LIMIT_FWD, CAN_MSGID_FULL_M, dataBuffer, &dataSize))
		{
			double limit = unpackFXP16_16(dataBuffer);

			if(FXP16_EQ(limit, m_forwardLimit))
				m_forwardLimitVerified = true;
			else
			{
				// It's wrong - set it again
				ConfigForwardLimit(m_forwardLimit);
			}
		}
		else
		{
			// Verification is needed but not available - request it again.
			requestMessage(LM_API_CFG_LIMIT_FWD);
		}
	}

	if(!m_reverseLimitVerified)
	{
		if(getMessage(LM_API_CFG_LIMIT_REV, CAN_MSGID_FULL_M, dataBuffer, &dataSize))
		{
			double limit = unpackFXP16_16(dataBuffer);

			if(FXP16_EQ(limit, m_reverseLimit))
				m_reverseLimitVerified = true;
			else
			{
				// It's wrong - set it again
				ConfigReverseLimit(m_reverseLimit);
			}
		}
		else
		{
			// Verification is needed but not available - request it again.
			requestMessage(LM_API_CFG_LIMIT_REV);
		}
	}

	if(!m_maxOutputVoltageVerified)
	{
		if(getMessage(LM_API_CFG_MAX_VOUT, CAN_MSGID_FULL_M, dataBuffer, &dataSize))
		{
			double voltage = unpackFXP16_16(dataBuffer);

			if(FXP16_EQ(voltage, m_maxOutputVoltage))
				m_maxOutputVoltageVerified = true;
			else
			{
				// It's wrong - set it again
				ConfigMaxOutputVoltage(m_maxOutputVoltage);
			}
		}
		else
		{
			// Verification is needed but not available - request it again.
			requestMessage(LM_API_CFG_MAX_VOUT);
		}
	}

	if(!m_voltageRampRateVerified)
	{
		if(m_controlMode == kPercentVbus)
		{
			if(getMessage(LM_API_VOLT_SET_RAMP, CAN_MSGID_FULL_M, dataBuffer, &dataSize))
			{
				double rate = unpackPercentage(dataBuffer);

				if(FXP16_EQ(rate, m_voltageRampRate))
					m_voltageRampRateVerified = true;
				else
				{
					// It's wrong - set it again
					SetVoltageRampRate(m_voltageRampRate);
				}
			}
			else
			{
				// Verification is needed but not available - request it again.
				requestMessage(LM_API_VOLT_SET_RAMP);
			}
		}
		else if(m_controlMode == kVoltage)
		{
			if(getMessage(LM_API_VCOMP_IN_RAMP, CAN_MSGID_FULL_M, dataBuffer, &dataSize))
			{
				double rate = unpackFXP8_8(dataBuffer);

				if(FXP8_EQ(rate, m_voltageRampRate))
					m_voltageRampRateVerified = true;
				else
				{
					// It's wrong - set it again
					SetVoltageRampRate(m_voltageRampRate);
				}
			}
			else
			{
				// Verification is needed but not available - request it again.
				requestMessage(LM_API_VCOMP_IN_RAMP);
			}
		}
	}
}

/**
 * Set the reference source device for speed controller mode.
 *
 * Choose encoder as the source of speed feedback when in speed control mode.
 *
 * @param reference Specify a SpeedReference.
 */
void CANJaguar::SetSpeedReference(SpeedReference reference)
{
	uint8_t dataBuffer[8];

	// Send the speed reference parameter
	dataBuffer[0] = reference;
	sendMessage(LM_API_SPD_REF, dataBuffer, sizeof(uint8_t));

	m_speedReference = reference;
	m_speedRefVerified = false;
}

/**
 * Get the reference source device for speed controller mode.
 *
 * @return A SpeedReference indicating the currently selected reference device for speed controller mode.
 */
CANJaguar::SpeedReference CANJaguar::GetSpeedReference()
{
	return m_speedReference;
}

/**
 * Set the reference source device for position controller mode.
 *
 * Choose between using and encoder and using a potentiometer
 * as the source of position feedback when in position control mode.
 *
 * @param reference Specify a PositionReference.
 */
void CANJaguar::SetPositionReference(PositionReference reference)
{
	uint8_t dataBuffer[8];

	// Send the position reference parameter
	dataBuffer[0] = reference;
	sendMessage(LM_API_POS_REF, dataBuffer, sizeof(uint8_t));

	m_positionReference = reference;
	m_posRefVerified = false;
}

/**
 * Get the reference source device for position controller mode.
 *
 * @return A PositionReference indicating the currently selected reference device for position controller mode.
 */
CANJaguar::PositionReference CANJaguar::GetPositionReference()
{
	return m_positionReference;
}

/**
 * Set the P, I, and D constants for the closed loop modes.
 *
 * @param p The proportional gain of the Jaguar's PID controller.
 * @param i The integral gain of the Jaguar's PID controller.
 * @param d The differential gain of the Jaguar's PID controller.
 */
void CANJaguar::SetPID(double p, double i, double d)
{
	SetP(p);
	SetI(i);
	SetD(d);
}

/**
 * Set the P constant for the closed loop modes.
 *
 * @param p The proportional gain of the Jaguar's PID controller.
 */
void CANJaguar::SetP(double p)
{
	uint8_t dataBuffer[8];
	uint8_t dataSize;

	switch(m_controlMode)
	{
	case kPercentVbus:
	case kVoltage:
		wpi_setWPIErrorWithContext(IncompatibleMode, "PID constants only apply in Speed, Position, and Current mode");
		break;
	case kSpeed:
		dataSize = packFXP16_16(dataBuffer, p);
		sendMessage(LM_API_SPD_PC, dataBuffer, dataSize);
		break;
	case kPosition:
		dataSize = packFXP16_16(dataBuffer, p);
		sendMessage(LM_API_POS_PC, dataBuffer, dataSize);
		break;
	case kCurrent:
		dataSize = packFXP16_16(dataBuffer, p);
		sendMessage(LM_API_ICTRL_PC, dataBuffer, dataSize);
		break;
	}

	m_p = p;
	m_pVerified = false;
}

/**
 * Set the I constant for the closed loop modes.
 *
 * @param i The integral gain of the Jaguar's PID controller.
 */
void CANJaguar::SetI(double i)
{
	uint8_t dataBuffer[8];
	uint8_t dataSize;

	switch(m_controlMode)
	{
	case kPercentVbus:
	case kVoltage:
		wpi_setWPIErrorWithContext(IncompatibleMode, "PID constants only apply in Speed, Position, and Current mode");
		break;
	case kSpeed:
		dataSize = packFXP16_16(dataBuffer, i);
		sendMessage(LM_API_SPD_IC, dataBuffer, dataSize);
		break;
	case kPosition:
		dataSize = packFXP16_16(dataBuffer, i);
		sendMessage(LM_API_POS_IC, dataBuffer, dataSize);
		break;
	case kCurrent:
		dataSize = packFXP16_16(dataBuffer, i);
		sendMessage(LM_API_ICTRL_IC, dataBuffer, dataSize);
		break;
	}

	m_i = i;
	m_iVerified = false;
}

/**
 * Set the D constant for the closed loop modes.
 *
 * @param d The derivative gain of the Jaguar's PID controller.
 */
void CANJaguar::SetD(double d)
{
	uint8_t dataBuffer[8];
	uint8_t dataSize;

	switch(m_controlMode)
	{
	case kPercentVbus:
	case kVoltage:
		wpi_setWPIErrorWithContext(IncompatibleMode, "PID constants only apply in Speed, Position, and Current mode");
		break;
	case kSpeed:
		dataSize = packFXP16_16(dataBuffer, d);
		sendMessage(LM_API_SPD_DC, dataBuffer, dataSize);
		break;
	case kPosition:
		dataSize = packFXP16_16(dataBuffer, d);
		sendMessage(LM_API_POS_DC, dataBuffer, dataSize);
		break;
	case kCurrent:
		dataSize = packFXP16_16(dataBuffer, d);
		sendMessage(LM_API_ICTRL_DC, dataBuffer, dataSize);
		break;
	}

	m_d = d;
	m_dVerified = false;
}

/**
 * Get the Proportional gain of the controller.
 *
 * @return The proportional gain.
 */
double CANJaguar::GetP()
{
	if(m_controlMode == kPercentVbus || m_controlMode == kVoltage)
	{
		wpi_setWPIErrorWithContext(IncompatibleMode, "PID constants only apply in Speed, Position, and Current mode");
		return 0.0;
	}

	return m_p;
}

/**
 * Get the Intregral gain of the controller.
 *
 * @return The integral gain.
 */
double CANJaguar::GetI()
{
	if(m_controlMode == kPercentVbus || m_controlMode == kVoltage)
	{
		wpi_setWPIErrorWithContext(IncompatibleMode, "PID constants only apply in Speed, Position, and Current mode");
		return 0.0;
	}

	return m_i;
}

/**
 * Get the Differential gain of the controller.
 *
 * @return The differential gain.
 */
double CANJaguar::GetD()
{
	if(m_controlMode == kPercentVbus || m_controlMode == kVoltage)
	{
		wpi_setWPIErrorWithContext(IncompatibleMode, "PID constants only apply in Speed, Position, and Current mode");
		return 0.0;
	}

	return m_d;
}

/**
 * Enable the closed loop controller.
 *
 * Start actually controlling the output based on the feedback.
 * If starting a position controller with an encoder reference,
 * use the encoderInitialPosition parameter to initialize the
 * encoder state.
 *
 * @param encoderInitialPosition Encoder position to set if position with encoder reference.  Ignored otherwise.
 */
void CANJaguar::EnableControl(double encoderInitialPosition)
{
	uint8_t dataBuffer[8];
	uint8_t dataSize = 0;

	switch(m_controlMode)
	{
	case kPercentVbus:
		sendMessage(LM_API_VOLT_T_EN, dataBuffer, dataSize);
		break;
	case kSpeed:
		sendMessage(LM_API_SPD_T_EN, dataBuffer, dataSize);
		break;
	case kPosition:
		dataSize = packFXP16_16(dataBuffer, encoderInitialPosition);
		sendMessage(LM_API_POS_T_EN, dataBuffer, dataSize);
		break;
	case kCurrent:
		sendMessage(LM_API_ICTRL_T_EN, dataBuffer, dataSize);
		break;
	case kVoltage:
		sendMessage(LM_API_VCOMP_T_EN, dataBuffer, dataSize);
		break;
	}

	m_controlModeVerified = false;
}

/**
 * Disable the closed loop controller.
 *
 * Stop driving the output based on the feedback.
 */
void CANJaguar::DisableControl()
{
	uint8_t dataBuffer[8];
	uint8_t dataSize = 0;

	switch(m_controlMode)
	{
	case kPercentVbus:
		sendMessage(LM_API_VOLT_DIS, dataBuffer, dataSize);
		break;
	case kSpeed:
		sendMessage(LM_API_SPD_DIS, dataBuffer, dataSize);
		break;
	case kPosition:
		sendMessage(LM_API_POS_DIS, dataBuffer, dataSize);
		break;
	case kCurrent:
		sendMessage(LM_API_ICTRL_DIS, dataBuffer, dataSize);
		break;
	case kVoltage:
		sendMessage(LM_API_VCOMP_DIS, dataBuffer, dataSize);
		break;
	}
}

/**
 * Change the control mode of this Jaguar object.
 *
 * After changing modes, configure any PID constants or other settings needed
 * and then EnableControl() to actually change the mode on the Jaguar.
 *
 * @param controlMode The new mode.
 */
void CANJaguar::ChangeControlMode(ControlMode controlMode)
{
	// Disable the previous mode
	DisableControl();

	// Update the local mode
	m_controlMode = controlMode;

	HALReport(HALUsageReporting::kResourceType_CANJaguar, m_deviceNumber, m_controlMode);
}

/**
 * Get the active control mode from the Jaguar.
 *
 * Ask the Jag what mode it is in.
 *
 * @return ControlMode that the Jag is in.
 */
CANJaguar::ControlMode CANJaguar::GetControlMode()
{
	return m_controlMode;
}

/**
 * Get the voltage at the battery input terminals of the Jaguar.
 *
 * @return The bus voltage in Volts.
 */
float CANJaguar::GetBusVoltage()
{
	uint8_t dataBuffer[8];
	uint8_t dataSize;

	if(getMessage(LM_API_STATUS_VOLTBUS, CAN_MSGID_FULL_M, dataBuffer, &dataSize))
	{
		m_busVoltage = unpackFXP8_8(dataBuffer);
	}

	return m_busVoltage;
}

/**
 * Get the voltage being output from the motor terminals of the Jaguar.
 *
 * @return The output voltage in Volts.
 */
float CANJaguar::GetOutputVoltage()
{
	uint8_t dataBuffer[8];
	uint8_t dataSize;

	if(getMessage(LM_API_STATUS_VOUT, CAN_MSGID_FULL_M, dataBuffer, &dataSize))
	{
		m_outputVoltage = unpackFXP8_8(dataBuffer);
	}

	return m_outputVoltage;
}

/**
 * Get the current through the motor terminals of the Jaguar.
 *
 * @return The output current in Amps.
 */
float CANJaguar::GetOutputCurrent()
{
	uint8_t dataBuffer[8];
	uint8_t dataSize;

	if(getMessage(LM_API_STATUS_CURRENT, CAN_MSGID_FULL_M, dataBuffer, &dataSize))
	{
		m_outputCurrent = unpackFXP8_8(dataBuffer);
	}

	return m_outputCurrent;
}

/**
 * Get the internal temperature of the Jaguar.
 *
 * @return The temperature of the Jaguar in degrees Celsius.
 */
float CANJaguar::GetTemperature()
{
	uint8_t dataBuffer[8];
	uint8_t dataSize;

	if(getMessage(LM_API_STATUS_TEMP, CAN_MSGID_FULL_M, dataBuffer, &dataSize))
	{
		m_temperature = unpackFXP8_8(dataBuffer);
	}

	return m_temperature;
}

/**
 * Get the position of the encoder or potentiometer.
 *
 * @return The position of the motor in rotations based on the configured feedback.
 */
double CANJaguar::GetPosition()
{
	uint8_t dataBuffer[8];
	uint8_t dataSize;

	if(getMessage(LM_API_STATUS_POS, CAN_MSGID_FULL_M, dataBuffer, &dataSize))
	{
		m_position = unpackFXP16_16(dataBuffer);
	}

	return m_position;
}

/**
 * Get the speed of the encoder.
 *
 * @return The speed of the motor in RPM based on the configured feedback.
 */
double CANJaguar::GetSpeed()
{
	uint8_t dataBuffer[8];
	uint8_t dataSize;

	if(getMessage(LM_API_STATUS_SPD, CAN_MSGID_FULL_M, dataBuffer, &dataSize))
	{
		m_speed = unpackFXP16_16(dataBuffer);
	}

	return m_speed;
}

/**
 * Get the status of the forward limit switch.
 *
 * @return The motor is allowed to turn in the forward direction when true.
 */
bool CANJaguar::GetForwardLimitOK()
{
	uint8_t dataBuffer[8];
	uint8_t dataSize;

	if(getMessage(LM_API_STATUS_LIMIT, CAN_MSGID_FULL_M, dataBuffer, &dataSize))
	{
		m_limits = dataBuffer[0];
	}

	return m_limits & kForwardLimit;
}

/**
 * Get the status of the reverse limit switch.
 *
 * @return The motor is allowed to turn in the reverse direction when true.
 */
bool CANJaguar::GetReverseLimitOK()
{
	uint8_t dataBuffer[8];
	uint8_t dataSize;

	if(getMessage(LM_API_STATUS_LIMIT, CAN_MSGID_FULL_M, dataBuffer, &dataSize))
	{
		m_limits = dataBuffer[0];
	}

	return m_limits & kReverseLimit;
}

/**
 * Get the status of any faults the Jaguar has detected.
 *
 * @return A bit-mask of faults defined by the "Faults" enum.
 */
uint16_t CANJaguar::GetFaults()
{
	uint8_t dataBuffer[8];
	uint8_t dataSize;

	if(getMessage(LM_API_STATUS_FAULT, CAN_MSGID_FULL_M, dataBuffer, &dataSize))
	{
		m_faults = unpackint16_t(dataBuffer);
	}

	return m_faults;
}

/**
 * Set the maximum voltage change rate.
 *
 * When in PercentVbus or Voltage output mode, the rate at which the voltage changes can
 * be limited to reduce current spikes.  Set this to 0.0 to disable rate limiting.
 *
 * @param rampRate The maximum rate of voltage change in Percent Voltage mode in V/s.
 */
void CANJaguar::SetVoltageRampRate(double rampRate)
{
	uint8_t dataBuffer[8];
	uint8_t dataSize;
	uint32_t message;

	switch(m_controlMode)
	{
	case kPercentVbus:
		dataSize = packPercentage(dataBuffer, rampRate / (m_maxOutputVoltage * kControllerRate));
		message = LM_API_VOLT_SET_RAMP;
		break;
	case kVoltage:
		dataSize = packFXP8_8(dataBuffer, rampRate / kControllerRate);
		message = LM_API_VCOMP_IN_RAMP;
		break;
	case kCurrent:
	case kSpeed:
	case kPosition:
	default:
		wpi_setWPIErrorWithContext(IncompatibleMode, "SetVoltageRampRate only applies in Voltage and Percent mode");
		return;
	}

	sendMessage(message, dataBuffer, dataSize);

	m_voltageRampRate = rampRate;
	m_voltageRampRateVerified = false;
}

/**
 * Get the version of the firmware running on the Jaguar.
 *
 * @return The firmware version.  0 if the device did not respond.
 */
uint32_t CANJaguar::GetFirmwareVersion()
{
	return m_firmwareVersion;
}

/**
 * Get the version of the Jaguar hardware.
 *
 * @return The hardware version. 1: Jaguar,  2: Black Jaguar
 */
uint8_t CANJaguar::GetHardwareVersion()
{
	return m_hardwareVersion;
}

/**
 * Configure what the controller does to the H-Bridge when neutral (not driving the output).
 *
 * This allows you to override the jumper configuration for brake or coast.
 *
 * @param mode Select to use the jumper setting or to override it to coast or brake.
 */
void CANJaguar::ConfigNeutralMode(NeutralMode mode)
{
	uint8_t dataBuffer[8];

	// Set the neutral mode
	dataBuffer[0] = mode;
	sendMessage(LM_API_CFG_BRAKE_COAST, dataBuffer, sizeof(uint8_t));

	m_neutralMode = mode;
	m_neutralModeVerified = false;
}

/**
 * Configure how many codes per revolution are generated by your encoder.
 *
 * @param codesPerRev The number of counts per revolution in 1X mode.
 */
void CANJaguar::ConfigEncoderCodesPerRev(uint16_t codesPerRev)
{
	uint8_t dataBuffer[8];
	uint8_t dataSize;

	// Set the codes per revolution mode
	dataSize = packint16_t(dataBuffer, codesPerRev);
	sendMessage(LM_API_CFG_ENC_LINES, dataBuffer, sizeof(uint16_t));

	m_encoderCodesPerRev = codesPerRev;
	m_encoderCodesPerRevVerified = false;
}

/**
 * Configure the number of turns on the potentiometer.
 *
 * There is no special support for continuous turn potentiometers.
 * Only integer numbers of turns are supported.
 *
 * @param turns The number of turns of the potentiometer
 */
void CANJaguar::ConfigPotentiometerTurns(uint16_t turns)
{
	uint8_t dataBuffer[8];
	uint8_t dataSize;

	// Set the pot turns
	dataSize = packint16_t(dataBuffer, turns);
	sendMessage(LM_API_CFG_POT_TURNS, dataBuffer, dataSize);

	m_potentiometerTurns = turns;
	m_potentiometerTurnsVerified = false;
}

/**
 * Configure Soft Position Limits when in Position Controller mode.
 *
 * When controlling position, you can add additional limits on top of the limit switch inputs
 * that are based on the position feedback.  If the position limit is reached or the
 * switch is opened, that direction will be disabled.
 *

 * @param forwardLimitPosition The position that if exceeded will disable the forward direction.
 * @param reverseLimitPosition The position that if exceeded will disable the reverse direction.
 */
void CANJaguar::ConfigSoftPositionLimits(double forwardLimitPosition, double reverseLimitPosition)
{
	ConfigLimitMode(kLimitMode_SoftPositionLimits);
	ConfigForwardLimit(forwardLimitPosition);
	ConfigReverseLimit(reverseLimitPosition);
}

/**
 * Disable Soft Position Limits if previously enabled.
 *
 * Soft Position Limits are disabled by default.
 */
void CANJaguar::DisableSoftPositionLimits()
{
	ConfigLimitMode(kLimitMode_SwitchInputsOnly);
}

/**
 * Set the limit mode for position control mode.
 *
 * Use ConfigSoftPositionLimits or DisableSoftPositionLimits to set this
 * automatically.
 */
void CANJaguar::ConfigLimitMode(LimitMode mode)
{
	uint8_t dataBuffer[8];

	dataBuffer[0] = mode;
	sendMessage(LM_API_CFG_LIMIT_MODE, dataBuffer, sizeof(uint8_t));

	m_limitMode = mode;
	m_limitModeVerified = false;
}

/**
* Set the position that if exceeded will disable the forward direction.
*
* Use ConfigSoftPositionLimits to set this and the limit mode automatically.
*/
void CANJaguar::ConfigForwardLimit(double forwardLimitPosition)
{
	uint8_t dataBuffer[8];
	uint8_t dataSize;

	dataSize = packFXP16_16(dataBuffer, forwardLimitPosition);
	dataBuffer[dataSize++] = 1;
	sendMessage(LM_API_CFG_LIMIT_FWD, dataBuffer, dataSize);

	m_forwardLimit = forwardLimitPosition;
	m_forwardLimitVerified = false;
}

/**
* Set the position that if exceeded will disable the reverse direction.
*
* Use ConfigSoftPositionLimits to set this and the limit mode automatically.
*/
void CANJaguar::ConfigReverseLimit(double reverseLimitPosition)
{
	uint8_t dataBuffer[8];
	uint8_t dataSize;

	dataSize = packFXP16_16(dataBuffer, reverseLimitPosition);
	dataBuffer[dataSize++] = 0;
	sendMessage(LM_API_CFG_LIMIT_REV, dataBuffer, dataSize);

	m_reverseLimit = reverseLimitPosition;
	m_reverseLimitVerified = false;
}

/**
 * Configure the maximum voltage that the Jaguar will ever output.
 *
 * This can be used to limit the maximum output voltage in all modes so that
 * motors which cannot withstand full bus voltage can be used safely.
 *
 * @param voltage The maximum voltage output by the Jaguar.
 */
void CANJaguar::ConfigMaxOutputVoltage(double voltage)
{
	uint8_t dataBuffer[8];
	uint8_t dataSize;

	dataSize = packFXP8_8(dataBuffer, voltage);
	sendMessage(LM_API_CFG_MAX_VOUT, dataBuffer, dataSize);

	m_maxOutputVoltage = voltage;
	m_maxOutputVoltageVerified = false;
}

/**
 * Configure how long the Jaguar waits in the case of a fault before resuming operation.
 *
 * Faults include over temerature, over current, and bus under voltage.
 * The default is 3.0 seconds, but can be reduced to as low as 0.5 seconds.
 *
 * @param faultTime The time to wait before resuming operation, in seconds.
 */
void CANJaguar::ConfigFaultTime(float faultTime)
{
	uint8_t dataBuffer[8];
	uint8_t dataSize;

	// Message takes ms
	dataSize = packint16_t(dataBuffer, (int16_t)(faultTime * 1000.0));
	sendMessage(LM_API_CFG_FAULT_TIME, dataBuffer, dataSize);

	m_faultTime = faultTime;
	m_faultTimeVerified = false;
}

/**
 * Update all the motors that have pending sets in the syncGroup.
 *
 * @param syncGroup A bitmask of groups to generate synchronous output.
 */
void CANJaguar::UpdateSyncGroup(uint8_t syncGroup)
{
	sendMessageHelper(CAN_MSGID_API_SYNC, &syncGroup, sizeof(syncGroup), CAN_SEND_PERIOD_NO_REPEAT);
}


void CANJaguar::SetExpiration(float timeout)
{
	if (m_safetyHelper) m_safetyHelper->SetExpiration(timeout);
}

float CANJaguar::GetExpiration()
{
	if (!m_safetyHelper) return 0.0;
	return m_safetyHelper->GetExpiration();
}

bool CANJaguar::IsAlive()
{
	if (!m_safetyHelper) return false;
	return m_safetyHelper->IsAlive();
}

bool CANJaguar::IsSafetyEnabled()
{
	if (!m_safetyHelper) return false;
	return m_safetyHelper->IsSafetyEnabled();
}

void CANJaguar::SetSafetyEnabled(bool enabled)
{
	if (m_safetyHelper) m_safetyHelper->SetSafetyEnabled(enabled);
}

void CANJaguar::GetDescription(char *desc)
{
	sprintf(desc, "CANJaguar ID %d", m_deviceNumber);
}

/**
 * Common interface for stopping the motor
 * Part of the MotorSafety interface
 *
 * @deprecated Call DisableControl instead.
 */
void CANJaguar::StopMotor()
{
	DisableControl();
}

void CANJaguar::ValueChanged(ITable* source, const std::string& key, EntryValue value, bool isNew)
{
	Set(value.f);
}

void CANJaguar::UpdateTable()
{
	if (m_table != NULL)
	{
		m_table->PutNumber("Value", Get());
	}
}

void CANJaguar::StartLiveWindowMode()
{
	if (m_table != NULL)
	{
		m_table->AddTableListener("Value", this, true);
	}
}

void CANJaguar::StopLiveWindowMode()
{
	if (m_table != NULL)
	{
		m_table->RemoveTableListener(this);
	}
}

std::string CANJaguar::GetSmartDashboardType()
{
	return "Speed Controller";
}

void CANJaguar::InitTable(ITable *subTable)
{
	m_table = subTable;
	UpdateTable();
}

ITable * CANJaguar::GetTable()
{
	return m_table;
}
