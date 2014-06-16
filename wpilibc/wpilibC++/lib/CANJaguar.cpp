/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2009. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "CANJaguar.h"
#define tNIRIO_i32 int
#include "NetworkCommunication/CANSessionMux.h"
#include "CAN/can_proto.h"
//#include "NetworkCommunication/UsageReporting.h"
#include "WPIErrors.h"
#include <stdio.h>
#include "LiveWindow/LiveWindow.h"

/* we are on ARM-LE now, not Freescale so no need to swap */
//TODO: is this defined in a PN way? or is this Jag-specific?
#define swap16(x)	(x)
#define swap32(x)	(x)

#define kFullMessageIDMask (CAN_MSGID_API_M | CAN_MSGID_MFR_M | CAN_MSGID_DTYPE_M)

const int32_t CANJaguar::kControllerRate;
constexpr double CANJaguar::kApproxBusVoltage;

// TODO: Make this a parameter
const int kDefaultCANPeriod = 20;

/**
 * Common initialization code called by all constructors.
 */
void CANJaguar::InitCANJaguar()
{
	m_table = NULL;
	m_transactionSemaphore = initializeMutexNormal();
	if (m_deviceNumber < 1 || m_deviceNumber > 63)
	{
		char buf[256];
		snprintf(buf, 256, "device number \"%d\" must be between 1 and 63", m_deviceNumber);
		wpi_setWPIErrorWithContext(ParameterOutOfRange, buf);
		return;
	}
	uint32_t fwVer = GetFirmwareVersion();
	if (StatusIsFatal())
		return;

    // 3330 was the first shipping RDK firmware version for the Jaguar
	if (fwVer >= 3330 || fwVer < 101)
	{
		char buf[256];
		if (fwVer < 3330)
		{
			snprintf(buf, 256, "Jag #%d firmware (%d) is too old (must be at least version 101 of the FIRST approved firmware)", m_deviceNumber, fwVer);
		}
		else
		{
			snprintf(buf, 256, "Jag #%d firmware (%d) is not FIRST approved (must be at least version 101 of the FIRST approved firmware)", m_deviceNumber, fwVer);
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

    m_verified_values = CAN_EVERYTHING;

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
	, m_transactionSemaphore (NULL)
	, m_maxOutputVoltage (kApproxBusVoltage)
	, m_safetyHelper (NULL)
{
	InitCANJaguar();
}

CANJaguar::~CANJaguar()
{
	delete m_safetyHelper;
	m_safetyHelper = NULL;
	deleteMutex(m_transactionSemaphore);
	m_transactionSemaphore = NULL;
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
	setTransaction(messageID, dataBuffer, dataSize);
	if (m_safetyHelper) m_safetyHelper->Feed();
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
	uint8_t dataBuffer[8];
	uint8_t dataSize;

	switch(m_controlMode)
	{
	case kPercentVbus:
		if(getTransaction(LM_API_VOLT_SET, dataBuffer, &dataSize)) {
            m_value = unpackPercentage(dataBuffer);
        }
        break;

	case kSpeed:
        if(getTransaction(LM_API_SPD_SET, dataBuffer, &dataSize)) {
            m_value = unpackFXP16_16(dataBuffer);
        }
        break;

	case kPosition:
        if(getTransaction(LM_API_POS_SET, dataBuffer, &dataSize)) {
            m_value = unpackFXP16_16(dataBuffer);
        }
        break;

	case kCurrent:
        if(getTransaction(LM_API_ICTRL_SET, dataBuffer, &dataSize)) {
            m_value = unpackFXP8_8(dataBuffer);
        }
        break;

	case kVoltage:
        if(getTransaction(LM_API_VCOMP_SET, dataBuffer, &dataSize)) {
            m_value = unpackFXP8_8(dataBuffer);
        }
		break;
	}

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
 * Send a message on the CAN bus through the CAN driver in FRC_NetworkCommunication
 *
 * Trusted messages require a 2-byte token at the beginning of the data payload.
 * If the message being sent is trusted, make space for the token.
 *
 * @param messageID The messageID to be used on the CAN bus
 * @param data The up to 8 bytes of data to be sent with the message
 * @param dataSize Specify how much of the data in "data" to send
 * @return Status of send call
 */
int32_t CANJaguar::sendMessage(uint32_t messageID, const uint8_t *data, uint8_t dataSize)
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
			if (dataSize > 6)
			{
				// TODO: I would rather this not have to set the global error
				wpi_setGlobalWPIErrorWithContext(ParameterOutOfRange, "dataSize > 6");
				return 0;
			}
			for (uint8_t j=0; j < dataSize; j++)
			{
				dataBuffer[j + 2] = data[j];
			}

			FRC_NetworkCommunication_CANSessionMux_sendMessage(messageID, dataBuffer, dataSize + 2, kDefaultCANPeriod, &status);

			return status;
		}
	}

	FRC_NetworkCommunication_CANSessionMux_sendMessage(messageID, data, dataSize, kDefaultCANPeriod, &status);

    return status;
}

/**
 * Receive a message from the CAN bus through the CAN driver in FRC_NetworkCommunication
 *
 * @param messageID The messageID to read from the CAN bus
 * @param data The up to 8 bytes of data that was received with the message
 * @param dataSize Indicates how much data was received
 * @return Status of receive call
 */
int32_t CANJaguar::receiveMessage(uint32_t *messageID, uint8_t *data, uint8_t *dataSize)
{
	uint32_t timeStamp;
	int32_t status = 0;

	FRC_NetworkCommunication_CANSessionMux_receiveMessage(messageID, kFullMessageIDMask, data, dataSize, &timeStamp, &status);

	return status;
}

/**
 * Execute a transaction with a Jaguar that sets some property.
 *
 * Jaguar always acks when it receives a message.  If we don't wait for an ack,
 * the message object in the Jaguar could get overwritten before it is handled.
 *
 * @param messageID The messageID to be used on the CAN bus (device number is added internally)
 * @param data The up to 8 bytes of data to be sent with the message
 * @param dataSize Specify how much of the data in "data" to send
 */
void CANJaguar::setTransaction(uint32_t messageID, const uint8_t *data, uint8_t dataSize)
{
	int32_t localStatus = 0;

	// If there was an error on this object and it wasn't a timeout, refuse to talk to the device
	// Call ClearError() on the object to try again
	if (StatusIsFatal() && GetError().GetCode() != -44087)
		return;

	// Make sure we don't have more than one transaction with the same Jaguar outstanding.
	takeMutex(m_transactionSemaphore);

	// Send the message with the data.
	localStatus = sendMessage(messageID | m_deviceNumber, data, dataSize);
	wpi_setErrorWithContext(localStatus, "sendMessage");

	// Transaction complete.
	giveMutex(m_transactionSemaphore);
}

/**
 * Execute a transaction with a Jaguar that gets some property.
 *
 * Jaguar always generates a message with the same message ID when replying.
 *
 * @param messageID The messageID to read from the CAN bus (device number is added internally)
 * @param data The up to 8 bytes of data that was received with the message
 * @param dataSize Indicates how much data was received
 *
 * @return True if the message was found.  Otherwise, no new message is available,
 * so a cached value will be used.
 */
bool CANJaguar::getTransaction(uint32_t messageID, uint8_t *data, uint8_t *dataSize)
{

	uint32_t targetedMessageID = messageID | m_deviceNumber;
	int32_t localStatus = 0;
    bool messageFound = true;

	// If there was an error on this object and it wasn't a message not found,
	// refuse to talk to the device. Call ClearError() on the object to try again
	if (StatusIsFatal() && GetError().GetCode() != ERR_CANSessionMux_MessageNotFound)
	{
		if (dataSize != NULL)
			*dataSize = 0;
		return false;
	}

	// Make sure we don't have more than one transaction with the same Jaguar outstanding.
	takeMutex(m_transactionSemaphore);

	// Send the message requesting data.
	localStatus = sendMessage(targetedMessageID, NULL, 0);
	wpi_setErrorWithContext(localStatus, "sendMessage");
	// Caller may have set bit31 for remote frame transmission so clear invalid bits[31-29]
	targetedMessageID &= 0x1FFFFFFF;
	// Wait for the data.
	localStatus = receiveMessage(&targetedMessageID, data, dataSize);

    if(localStatus == ERR_CANSessionMux_MessageNotFound)
{
        messageFound = false;
    } else {
        wpi_setErrorWithContext(localStatus, "receiveMessage");
    }

	// Transaction complete.
	giveMutex(m_transactionSemaphore);

    return messageFound;
}

/**
 * Check if a CAN value has been set but not verified yet.
 */
bool CANJaguar::isUnverified(CANValue value) const
{
    return !(m_verified_values & value);
}

/**
 * Check all unverified values and make sure they're equal to their local
 * cached versions.
 *
 * If a value isn't available, it gets requested.  If a value doesn't match up,
 * it gets set again.
 */
void CANJaguar::verifyCANValues()
{
    if(isUnverified(CAN_VALUE)) {

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

	dataBuffer[0] = reference;
	setTransaction(LM_API_SPD_REF, dataBuffer, sizeof(uint8_t));
}

/**
 * Get the reference source device for speed controller mode.
 *
 * @return A SpeedReference indicating the currently selected reference device for speed controller mode.
 */
CANJaguar::SpeedReference CANJaguar::GetSpeedReference()
{
	uint8_t dataBuffer[8];
	uint8_t dataSize;

	if(getTransaction(LM_API_SPD_REF, dataBuffer, &dataSize))
    {
            m_speedReference = (SpeedReference)*dataBuffer;
    }

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

	dataBuffer[0] = reference;
	setTransaction(LM_API_POS_REF, dataBuffer, sizeof(uint8_t));
}

/**
 * Get the reference source device for position controller mode.
 *
 * @return A PositionReference indicating the currently selected reference device for position controller mode.
 */
CANJaguar::PositionReference CANJaguar::GetPositionReference()
{
	uint8_t dataBuffer[8];
	uint8_t dataSize;

	if(getTransaction(LM_API_POS_REF, dataBuffer, &dataSize))
	{
		m_positionReference = (PositionReference)*dataBuffer;
	}

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
		setTransaction(LM_API_SPD_PC, dataBuffer, dataSize);
		dataSize = packFXP16_16(dataBuffer, i);
		setTransaction(LM_API_SPD_IC, dataBuffer, dataSize);
		dataSize = packFXP16_16(dataBuffer, d);
		setTransaction(LM_API_SPD_DC, dataBuffer, dataSize);
		break;
	case kPosition:
		dataSize = packFXP16_16(dataBuffer, p);
		setTransaction(LM_API_POS_PC, dataBuffer, dataSize);
		dataSize = packFXP16_16(dataBuffer, i);
		setTransaction(LM_API_POS_IC, dataBuffer, dataSize);
		dataSize = packFXP16_16(dataBuffer, d);
		setTransaction(LM_API_POS_DC, dataBuffer, dataSize);
		break;
	case kCurrent:
		dataSize = packFXP16_16(dataBuffer, p);
		setTransaction(LM_API_ICTRL_PC, dataBuffer, dataSize);
		dataSize = packFXP16_16(dataBuffer, i);
		setTransaction(LM_API_ICTRL_IC, dataBuffer, dataSize);
		dataSize = packFXP16_16(dataBuffer, d);
		setTransaction(LM_API_ICTRL_DC, dataBuffer, dataSize);
		break;
	}
}

/**
 * Get the Proportional gain of the controller.
 *
 * @return The proportional gain.
 */
double CANJaguar::GetP()
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
		if(getTransaction(LM_API_SPD_PC, dataBuffer, &dataSize))
        {
            m_p = unpackFXP16_16(dataBuffer);
        }
        break;
	case kPosition:
        if(getTransaction(LM_API_POS_PC, dataBuffer, &dataSize))
        {
            m_p = unpackFXP16_16(dataBuffer);
        }
        break;
	case kCurrent:
        if(getTransaction(LM_API_ICTRL_PC, dataBuffer, &dataSize))
        {
            m_p = unpackFXP16_16(dataBuffer);
        }
        break;
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
    uint8_t dataBuffer[8];
    uint8_t dataSize;

    switch(m_controlMode)
    {
    case kPercentVbus:
    case kVoltage:
        wpi_setWPIErrorWithContext(IncompatibleMode, "PID constants only apply in Speed, Position, and Current mode");
        break;
    case kSpeed:
        if(getTransaction(LM_API_SPD_IC, dataBuffer, &dataSize))
        {
            m_i = unpackFXP16_16(dataBuffer);
        }
        break;
    case kPosition:
        if(getTransaction(LM_API_POS_IC, dataBuffer, &dataSize))
        {
            m_i = unpackFXP16_16(dataBuffer);
        }
        break;
    case kCurrent:
        if(getTransaction(LM_API_ICTRL_IC, dataBuffer, &dataSize))
        {
            m_i = unpackFXP16_16(dataBuffer);
        }
        break;
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
    uint8_t dataBuffer[8];
    uint8_t dataSize;

    switch(m_controlMode)
    {
    case kPercentVbus:
    case kVoltage:
        wpi_setWPIErrorWithContext(IncompatibleMode, "PID constants only apply in Speed, Position, and Current mode");
        break;
    case kSpeed:
        if(getTransaction(LM_API_SPD_DC, dataBuffer, &dataSize))
        {
            m_d = unpackFXP16_16(dataBuffer);
        }
        break;
    case kPosition:
        if(getTransaction(LM_API_POS_DC, dataBuffer, &dataSize))
        {
            m_d = unpackFXP16_16(dataBuffer);
        }
        break;
    case kCurrent:
        if(getTransaction(LM_API_ICTRL_DC, dataBuffer, &dataSize))
        {
            m_d = unpackFXP16_16(dataBuffer);
        }
        break;
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
		setTransaction(LM_API_VOLT_T_EN, dataBuffer, dataSize);
		break;
	case kSpeed:
		setTransaction(LM_API_SPD_T_EN, dataBuffer, dataSize);
		break;
	case kPosition:
		dataSize = packFXP16_16(dataBuffer, encoderInitialPosition);
		setTransaction(LM_API_POS_T_EN, dataBuffer, dataSize);
		break;
	case kCurrent:
		setTransaction(LM_API_ICTRL_T_EN, dataBuffer, dataSize);
		break;
	case kVoltage:
		setTransaction(LM_API_VCOMP_T_EN, dataBuffer, dataSize);
		break;
	}
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
		setTransaction(LM_API_VOLT_DIS, dataBuffer, dataSize);
		break;
	case kSpeed:
		setTransaction(LM_API_SPD_DIS, dataBuffer, dataSize);
		break;
	case kPosition:
		setTransaction(LM_API_POS_DIS, dataBuffer, dataSize);
		break;
	case kCurrent:
		setTransaction(LM_API_ICTRL_DIS, dataBuffer, dataSize);
		break;
	case kVoltage:
		setTransaction(LM_API_VCOMP_DIS, dataBuffer, dataSize);
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
	uint8_t dataBuffer[8];
	uint8_t dataSize;

	if(getTransaction(LM_API_STATUS_CMODE, dataBuffer, &dataSize))
    {
        m_controlMode = (ControlMode)dataBuffer[0];
    }

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

	if(getTransaction(LM_API_STATUS_VOLTBUS, dataBuffer, &dataSize))
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

    if(getTransaction(LM_API_STATUS_VOUT, dataBuffer, &dataSize))
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

    if(getTransaction(LM_API_STATUS_CURRENT, dataBuffer, &dataSize))
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

    if(getTransaction(LM_API_STATUS_TEMP, dataBuffer, &dataSize))
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

	if(getTransaction(LM_API_STATUS_POS, dataBuffer, &dataSize))
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

	if(getTransaction(LM_API_STATUS_SPD, dataBuffer, &dataSize))
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

	if(getTransaction(LM_API_STATUS_LIMIT, dataBuffer, &dataSize))
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

    if(getTransaction(LM_API_STATUS_LIMIT, dataBuffer, &dataSize))
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

	if(getTransaction(LM_API_STATUS_FAULT, dataBuffer, &dataSize))
    {
        m_faults = unpackint16_t(dataBuffer);;
    }

    return m_faults;
}

#if 0
/**
 * Check if the Jaguar's power has been cycled since this was last called.
 *
 * This should return true the first time called after a Jaguar power up,
 * and false after that.
 *
 * @return The Jaguar was power cycled since the last call to this function.
 */
bool CANJaguar::GetPowerCycled()
{
	uint8_t dataBuffer[8];
	uint8_t dataSize;

	getTransaction(LM_API_STATUS_POWER, dataBuffer, &dataSize);
	if (dataSize == sizeof(uint8_t))
	{
		bool powerCycled = (*dataBuffer != 0);

		// Clear the power cycled bit now that we've accessed it
		if (powerCycled)
		{
			dataBuffer[0] = 1;
			setTransaction(LM_API_STATUS_POWER, dataBuffer, sizeof(uint8_t));
		}

		return powerCycled;
	}
	return 0;
}
#endif

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

	switch(m_controlMode)
	{
	case kPercentVbus:
		dataSize = packPercentage(dataBuffer, rampRate / (m_maxOutputVoltage * kControllerRate));
		setTransaction(LM_API_VOLT_SET_RAMP, dataBuffer, dataSize);
		break;
	case kVoltage:
		dataSize = packFXP8_8(dataBuffer, rampRate / kControllerRate);
		setTransaction(LM_API_VCOMP_IN_RAMP, dataBuffer, dataSize);
		break;
	default:
		return;
	}
}

/**
 * Get the version of the firmware running on the Jaguar.
 *
 * @return The firmware version.  0 if the device did not respond.
 */
uint32_t CANJaguar::GetFirmwareVersion()
{
    uint8_t dataBuffer[8];
	uint8_t dataSize;

    // Set the MSB to tell the 2CAN that this is a remote message.
	if(getTransaction(0x80000000 | CAN_MSGID_API_FIRMVER, dataBuffer, &dataSize))
    {
        m_firmwareVersion = unpackint32_t(dataBuffer);
    }

    return m_firmwareVersion;
}

/**
 * Get the version of the Jaguar hardware.
 *
 * @return The hardware version. 1: Jaguar,  2: Black Jaguar
 */
uint8_t CANJaguar::GetHardwareVersion()
{
	uint8_t dataBuffer[8];
	uint8_t dataSize;

    // Only get once, since this shoudn't change.
    if(!m_hardwareVersion && getTransaction(LM_API_HWVER, dataBuffer, &dataSize))
    {
        m_hardwareVersion = *(dataBuffer+1);
    }

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

	dataBuffer[0] = mode;
	setTransaction(LM_API_CFG_BRAKE_COAST, dataBuffer, sizeof(uint8_t));
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

	dataSize = packint16_t(dataBuffer, codesPerRev);
	setTransaction(LM_API_CFG_ENC_LINES, dataBuffer, dataSize);
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

	dataSize = packint16_t(dataBuffer, turns);
	setTransaction(LM_API_CFG_POT_TURNS, dataBuffer, dataSize);
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
	uint8_t dataBuffer[8];
	uint8_t dataSize;

	dataSize = packFXP16_16(dataBuffer, forwardLimitPosition);
	dataBuffer[dataSize++] = forwardLimitPosition > reverseLimitPosition;
	setTransaction(LM_API_CFG_LIMIT_FWD, dataBuffer, dataSize);

	dataSize = packFXP16_16(dataBuffer, reverseLimitPosition);
	dataBuffer[dataSize++] = forwardLimitPosition <= reverseLimitPosition;
	setTransaction(LM_API_CFG_LIMIT_REV, dataBuffer, dataSize);

	dataBuffer[0] = kLimitMode_SoftPositionLimits;
	setTransaction(LM_API_CFG_LIMIT_MODE, dataBuffer, sizeof(uint8_t));
}

/**
 * Disable Soft Position Limits if previously enabled.
 *
 * Soft Position Limits are disabled by default.
 */
void CANJaguar::DisableSoftPositionLimits()
{
	uint8_t dataBuffer[8];

	dataBuffer[0] = kLimitMode_SwitchInputsOnly;
	setTransaction(LM_API_CFG_LIMIT_MODE, dataBuffer, sizeof(uint8_t));
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

	m_maxOutputVoltage = voltage;
	dataSize = packFXP8_8(dataBuffer, voltage);
	setTransaction(LM_API_CFG_MAX_VOUT, dataBuffer, dataSize);
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
	setTransaction(LM_API_CFG_FAULT_TIME, dataBuffer, dataSize);
}

/**
 * Update all the motors that have pending sets in the syncGroup.
 *
 * @param syncGroup A bitmask of groups to generate synchronous output.
 */
void CANJaguar::UpdateSyncGroup(uint8_t syncGroup)
{
	sendMessage(CAN_MSGID_API_SYNC, &syncGroup, sizeof(syncGroup));
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
