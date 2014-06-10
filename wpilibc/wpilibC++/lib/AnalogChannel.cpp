/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "AnalogChannel.h"
#include "AnalogModule.h"
//#include "NetworkCommunication/UsageReporting.h"
#include "Resource.h"
#include "WPIErrors.h"
#include "LiveWindow/LiveWindow.h"

static Resource *channels = NULL;

const uint8_t AnalogChannel::kAccumulatorModuleNumber;
const uint32_t AnalogChannel::kAccumulatorNumChannels;
const uint32_t AnalogChannel::kAccumulatorChannels[] = {0, 1};

/**
 * Common initialization.
 */
void AnalogChannel::InitChannel(uint8_t moduleNumber, uint32_t channel)
{
	m_table = NULL;
	char buf[64];
	Resource::CreateResourceObject(&channels, kAnalogModules * kAnalogChannels);
	if (!checkAnalogModule(moduleNumber))
	{
		snprintf(buf, 64, "Analog Module %d", moduleNumber);
		wpi_setWPIErrorWithContext(ModuleIndexOutOfRange, buf);
		return;
	}
	if (!checkAnalogChannel(channel))
	{
		snprintf(buf, 64, "Analog Channel %d", channel);
		wpi_setWPIErrorWithContext(ChannelIndexOutOfRange, buf);
		return;
	}

	snprintf(buf, 64, "Analog Input %d (Module: %d)", channel, moduleNumber);
	if (channels->Allocate((moduleNumber - 1) * kAnalogChannels + channel, buf) == ~0ul)
	{
		CloneError(channels);
		return;
	}
	m_channel = channel;
	m_module = moduleNumber;
	
	void* port = getPortWithModule(moduleNumber, channel);
	int32_t status = 0;
	m_port = initializeAnalogPort(port, &status);
	wpi_setErrorWithContext(status, getHALErrorMessage(status));

	LiveWindow::GetInstance()->AddSensor("AnalogChannel",channel, GetModuleNumber(), this);
	HALReport(HALUsageReporting::kResourceType_AnalogChannel, channel, GetModuleNumber() - 1);
}

/**
 * Construct an analog channel on a specified module.
 * 
 * @param moduleNumber The analog module (1 or 2).
 * @param channel The channel number to represent.
 */
AnalogChannel::AnalogChannel(uint8_t moduleNumber, uint32_t channel)
{
	InitChannel(moduleNumber, channel);
}

/**
 * Construct an analog channel on the default module.
 * 
 * @param channel The channel number to represent.
 */
AnalogChannel::AnalogChannel(uint32_t channel)
{
	InitChannel(GetDefaultAnalogModule(), channel);
}

/**
 * Channel destructor.
 */
AnalogChannel::~AnalogChannel()
{
	channels->Free((m_module - 1) * kAnalogChannels + m_channel);
}

/**
 * Get the analog module that this channel is on.
 * @return A pointer to the AnalogModule that this channel is on.
 */
AnalogModule *AnalogChannel::GetModule()
{
	if (StatusIsFatal()) return NULL;
	return AnalogModule::GetInstance(m_module);
}

/**
 * Get a sample straight from this channel on the module.
 * The sample is a 12-bit value representing the -10V to 10V range of the A/D converter in the module.
 * The units are in A/D converter codes.  Use GetVoltage() to get the analog value in calibrated units.
 * @return A sample straight from this channel on the module.
 */
int16_t AnalogChannel::GetValue()
{
	if (StatusIsFatal()) return 0;
	int32_t status = 0;
	int16_t value = getAnalogValue(m_port, &status);
	wpi_setErrorWithContext(status, getHALErrorMessage(status));
	return value;
}

/**
 * Get a sample from the output of the oversample and average engine for this channel.
 * The sample is 12-bit + the value configured in SetOversampleBits().
 * The value configured in SetAverageBits() will cause this value to be averaged 2**bits number of samples.
 * This is not a sliding window.  The sample will not change until 2**(OversamplBits + AverageBits) samples
 * have been acquired from the module on this channel.
 * Use GetAverageVoltage() to get the analog value in calibrated units.
 * @return A sample from the oversample and average engine for this channel.
 */
int32_t AnalogChannel::GetAverageValue()
{
	if (StatusIsFatal()) return 0;
	int32_t status = 0;
	int32_t value = getAnalogAverageValue(m_port, &status);
	wpi_setErrorWithContext(status, getHALErrorMessage(status));
	return value;
}

/**
 * Get a scaled sample straight from this channel on the module.
 * The value is scaled to units of Volts using the calibrated scaling data from GetLSBWeight() and GetOffset().
 * @return A scaled sample straight from this channel on the module.
 */
float AnalogChannel::GetVoltage()
{
	if (StatusIsFatal()) return 0.0f;
	int32_t status = 0;
	float voltage = getAnalogVoltage(m_port, &status);
	wpi_setErrorWithContext(status, getHALErrorMessage(status));
	return voltage;
}

/**
 * Get a scaled sample from the output of the oversample and average engine for this channel.
 * The value is scaled to units of Volts using the calibrated scaling data from GetLSBWeight() and GetOffset().
 * Using oversampling will cause this value to be higher resolution, but it will update more slowly.
 * Using averaging will cause this value to be more stable, but it will update more slowly.
 * @return A scaled sample from the output of the oversample and average engine for this channel.
 */
float AnalogChannel::GetAverageVoltage()
{
	if (StatusIsFatal()) return 0.0f;
	int32_t status = 0;
	float voltage = getAnalogAverageVoltage(m_port, &status);
	wpi_setErrorWithContext(status, getHALErrorMessage(status));
	return voltage;
}

/**
 * Get the factory scaling least significant bit weight constant.
 * The least significant bit weight constant for the channel that was calibrated in
 * manufacturing and stored in an eeprom in the module.
 * 
 * Volts = ((LSB_Weight * 1e-9) * raw) - (Offset * 1e-9)
 * 
 * @return Least significant bit weight.
 */
uint32_t AnalogChannel::GetLSBWeight()
{
	if (StatusIsFatal()) return 0;
	int32_t status = 0;
	int32_t lsbWeight = getAnalogLSBWeight(m_port, &status);
	wpi_setErrorWithContext(status, getHALErrorMessage(status));
	return lsbWeight;
}

/**
 * Get the factory scaling offset constant.
 * The offset constant for the channel that was calibrated in manufacturing and stored
 * in an eeprom in the module.
 * 
 * Volts = ((LSB_Weight * 1e-9) * raw) - (Offset * 1e-9)
 * 
 * @return Offset constant.
 */
int32_t AnalogChannel::GetOffset()
{
	if (StatusIsFatal()) return 0;
	int32_t status = 0;
	int32_t offset = getAnalogOffset(m_port, &status);
	wpi_setErrorWithContext(status, getHALErrorMessage(status));
	return offset;
}

/**
 * Get the channel number.
 * @return The channel number.
 */
uint32_t AnalogChannel::GetChannel()
{
	if (StatusIsFatal()) return 0;
	return m_channel;
}

/**
 * Get the module number.
 * @return The module number.
 */
uint8_t AnalogChannel::GetModuleNumber()
{
	if (StatusIsFatal()) return 0;
	return m_module;
}

/**
 * Set the number of averaging bits.
 * This sets the number of averaging bits. The actual number of averaged samples is 2**bits.
 * Use averaging to improve the stability of your measurement at the expense of sampling rate.
 * The averaging is done automatically in the FPGA.
 * 
 * @param bits Number of bits of averaging.
 */
void AnalogChannel::SetAverageBits(uint32_t bits)
{
	if (StatusIsFatal()) return;
	int32_t status = 0;
	setAnalogAverageBits(m_port, bits, &status);
	wpi_setErrorWithContext(status, getHALErrorMessage(status));
}

/**
 * Get the number of averaging bits previously configured.
 * This gets the number of averaging bits from the FPGA. The actual number of averaged samples is 2**bits.
 * The averaging is done automatically in the FPGA.
 * 
 * @return Number of bits of averaging previously configured.
 */
uint32_t AnalogChannel::GetAverageBits()
{
	int32_t status = 0;
	int32_t averageBits = getAnalogAverageBits(m_port, &status);
	wpi_setErrorWithContext(status, getHALErrorMessage(status));
	return averageBits;
}

/**
 * Set the number of oversample bits.
 * This sets the number of oversample bits. The actual number of oversampled values is 2**bits.
 * Use oversampling to improve the resolution of your measurements at the expense of sampling rate.
 * The oversampling is done automatically in the FPGA.
 * 
 * @param bits Number of bits of oversampling.
 */
void AnalogChannel::SetOversampleBits(uint32_t bits)
{
	if (StatusIsFatal()) return;
	int32_t status = 0;
	setAnalogOversampleBits(m_port, bits, &status);
	wpi_setErrorWithContext(status, getHALErrorMessage(status));
}

/**
 * Get the number of oversample bits previously configured.
 * This gets the number of oversample bits from the FPGA. The actual number of oversampled values is
 * 2**bits. The oversampling is done automatically in the FPGA.
 * 
 * @return Number of bits of oversampling previously configured.
 */
uint32_t AnalogChannel::GetOversampleBits()
{
	if (StatusIsFatal()) return 0;
	int32_t status = 0;
	int32_t oversampleBits = getAnalogOversampleBits(m_port, &status);
	wpi_setErrorWithContext(status, getHALErrorMessage(status));
	return oversampleBits;
}

/**
 * Is the channel attached to an accumulator.
 * 
 * @return The analog channel is attached to an accumulator.
 */
bool AnalogChannel::IsAccumulatorChannel()
{
	if (StatusIsFatal()) return false;
	int32_t status = 0;
	bool isAccum = isAccumulatorChannel(m_port, &status);
	wpi_setErrorWithContext(status, getHALErrorMessage(status));
	return isAccum;
}

/**
 * Initialize the accumulator.
 */
void AnalogChannel::InitAccumulator()
{
	if (StatusIsFatal()) return;
	m_accumulatorOffset = 0;
	int32_t status = 0;
	initAccumulator(m_port, &status);
	wpi_setErrorWithContext(status, getHALErrorMessage(status));
}


/**
 * Set an inital value for the accumulator.
 * 
 * This will be added to all values returned to the user.
 * @param initialValue The value that the accumulator should start from when reset.
 */
void AnalogChannel::SetAccumulatorInitialValue(int64_t initialValue)
{
	if (StatusIsFatal()) return;
	m_accumulatorOffset = initialValue;
}

/**
 * Resets the accumulator to the initial value.
 */
void AnalogChannel::ResetAccumulator()
{
	if (StatusIsFatal()) return;
	int32_t status = 0;
	resetAccumulator(m_port, &status);
	wpi_setErrorWithContext(status, getHALErrorMessage(status));
}

/**
 * Set the center value of the accumulator.
 * 
 * The center value is subtracted from each A/D value before it is added to the accumulator. This
 * is used for the center value of devices like gyros and accelerometers to make integration work
 * and to take the device offset into account when integrating.
 * 
 * This center value is based on the output of the oversampled and averaged source from channel 1.
 * Because of this, any non-zero oversample bits will affect the size of the value for this field.
 */
void AnalogChannel::SetAccumulatorCenter(int32_t center)
{
	if (StatusIsFatal()) return;
	int32_t status = 0;
	setAccumulatorCenter(m_port, center, &status);
	wpi_setErrorWithContext(status, getHALErrorMessage(status));
}

/**
 * Set the accumulator's deadband.
 */
void AnalogChannel::SetAccumulatorDeadband(int32_t deadband)
{
	if (StatusIsFatal()) return;
	int32_t status = 0;
	setAccumulatorDeadband(m_port, deadband, &status);
	wpi_setErrorWithContext(status, getHALErrorMessage(status));
}

/**
 * Read the accumulated value.
 * 
 * Read the value that has been accumulating on channel 1.
 * The accumulator is attached after the oversample and average engine.
 * 
 * @return The 64-bit value accumulated since the last Reset().
 */
int64_t AnalogChannel::GetAccumulatorValue()
{
	if (StatusIsFatal()) return 0;
	int32_t status = 0;
	int64_t value = getAccumulatorValue(m_port, &status);
	wpi_setErrorWithContext(status, getHALErrorMessage(status));
	return value + m_accumulatorOffset;
}

/**
 * Read the number of accumulated values.
 * 
 * Read the count of the accumulated values since the accumulator was last Reset().
 * 
 * @return The number of times samples from the channel were accumulated.
 */
uint32_t AnalogChannel::GetAccumulatorCount()
{
	if (StatusIsFatal()) return 0;
	int32_t status = 0;
	uint32_t count = getAccumulatorCount(m_port, &status);
	wpi_setErrorWithContext(status, getHALErrorMessage(status));
	return count;
}


/**
 * Read the accumulated value and the number of accumulated values atomically.
 * 
 * This function reads the value and count from the FPGA atomically.
 * This can be used for averaging.
 * 
 * @param value Pointer to the 64-bit accumulated output.
 * @param count Pointer to the number of accumulation cycles.
 */
void AnalogChannel::GetAccumulatorOutput(int64_t *value, uint32_t *count)
{
	if (StatusIsFatal()) return;
	int32_t status = 0;
	getAccumulatorOutput(m_port, value, count, &status);
	wpi_setErrorWithContext(status, getHALErrorMessage(status));
	*value += m_accumulatorOffset;
}

/**
 * Get the Average value for the PID Source base object.
 * 
 * @return The average voltage.
 */
double AnalogChannel::PIDGet() 
{
	if (StatusIsFatal()) return 0.0;
	return GetAverageValue();
}

void AnalogChannel::UpdateTable() {
	if (m_table != NULL) {
		m_table->PutNumber("Value", GetAverageVoltage());
	}
}

void AnalogChannel::StartLiveWindowMode() {
	
}

void AnalogChannel::StopLiveWindowMode() {
	
}

std::string AnalogChannel::GetSmartDashboardType() {
	return "Analog Input";
}

void AnalogChannel::InitTable(ITable *subTable) {
	m_table = subTable;
	UpdateTable();
}

ITable * AnalogChannel::GetTable() {
	return m_table;
}


