/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "PWM.h"

#include "DigitalModule.h"
//#include "NetworkCommunication/UsageReporting.h"
#include "Resource.h"
#include "Utility.h"
#include "WPIErrors.h"

constexpr float PWM::kDefaultPwmPeriod;
constexpr float PWM::kDefaultPwmCenter;
const int32_t PWM::kDefaultPwmStepsDown;
const int32_t PWM::kPwmDisabled;
static Resource *allocated = NULL;

/**
 * Initialize PWMs given a channel.
 *
 * This method is private and is the common path for all the constructors for creating PWM
 * instances. Checks channel value range and allocates the appropriate channel.
 * The allocation is only done to help users ensure that they don't double assign channels.
 */
void PWM::InitPWM(uint32_t channel)
{
	m_table = NULL;
	char buf[64];
	Resource::CreateResourceObject(&allocated, kPwmChannels);

	if (!CheckPWMChannel(channel))
	{
		snprintf(buf, 64, "PWM Channel %d", channel);
		wpi_setWPIErrorWithContext(ChannelIndexOutOfRange, buf);
		return;
	}

	snprintf(buf, 64, "PWM %d", channel);
	if (allocated->Allocate(channel, buf) == ~0ul)
	{
		CloneError(allocated);
		return;
	}
	m_channel = channel;
	m_module = DigitalModule::GetInstance(1);
	m_module->SetPWM(m_channel, kPwmDisabled);
	m_eliminateDeadband = false;

	HALReport(HALUsageReporting::kResourceType_PWM, channel);
}

/**
 * Allocate a PWM given a channel number.
 *
 * @param channel The PWM channel.
 */
PWM::PWM(uint32_t channel)
	: m_module(NULL)
{
	InitPWM(channel);
}

/**
 * Free the PWM channel.
 *
 * Free the resource associated with the PWM channel and set the value to 0.
 */
PWM::~PWM()
{
	if (m_module)
	{
		m_module->SetPWM(m_channel, kPwmDisabled);
		allocated->Free(m_channel);
	}
}

/**
 * Optionally eliminate the deadband from a speed controller.
 * @param eliminateDeadband If true, set the motor curve on the Jaguar to eliminate
 * the deadband in the middle of the range. Otherwise, keep the full range without
 * modifying any values.
 */
void PWM::EnableDeadbandElimination(bool eliminateDeadband)
{
	if (StatusIsFatal()) return;
	m_eliminateDeadband = eliminateDeadband;
}

/**
 * Set the bounds on the PWM values.
 * This sets the bounds on the PWM values for a particular each type of controller. The values
 * determine the upper and lower speeds as well as the deadband bracket.
 * @param max The Minimum pwm value
 * @param deadbandMax The high end of the deadband range
 * @param center The center speed (off)
 * @param deadbandMin The low end of the deadband range
 * @param min The minimum pwm value
 */
void PWM::SetBounds(int32_t max, int32_t deadbandMax, int32_t center, int32_t deadbandMin, int32_t min)
{
	if (StatusIsFatal()) return;
	m_maxPwm = max;
	m_deadbandMaxPwm = deadbandMax;
	m_centerPwm = center;
	m_deadbandMinPwm = deadbandMin;
	m_minPwm = min;
}


/**
 * Set the bounds on the PWM pulse widths.
 * This sets the bounds on the PWM values for a particular type of controller. The values
 * determine the upper and lower speeds as well as the deadband bracket.
 * @param max The max PWM pulse width in ms
 * @param deadbandMax The high end of the deadband range pulse width in ms
 * @param center The center (off) pulse width in ms
 * @param deadbandMin The low end of the deadband pulse width in ms
 * @param min The minimum pulse width in ms
 */
void PWM::SetBounds(double max, double deadbandMax, double center, double deadbandMin, double min)
{
	// calculate the loop time in milliseconds
	double loopTime = m_module->GetLoopTiming()/(kSystemClockTicksPerMicrosecond*1e3);
	if (StatusIsFatal()) return;

    m_maxPwm = (int32_t)((max-kDefaultPwmCenter)/loopTime+kDefaultPwmStepsDown-1);
    m_deadbandMaxPwm = (int32_t)((deadbandMax-kDefaultPwmCenter)/loopTime+kDefaultPwmStepsDown-1);
    m_centerPwm = (int32_t)((center-kDefaultPwmCenter)/loopTime+kDefaultPwmStepsDown-1);
    m_deadbandMinPwm = (int32_t)((deadbandMin-kDefaultPwmCenter)/loopTime+kDefaultPwmStepsDown-1);
    m_minPwm = (int32_t)((min-kDefaultPwmCenter)/loopTime+kDefaultPwmStepsDown-1);
//    printf("Calculated m_minPwm: %d min: %lf  loopTime: %lf  defaultStepsDown: %d\n", m_minPwm, min, loopTime, kDefaultPwmStepsDown);
}

/**
 * Set the PWM value based on a position.
 *
 * This is intended to be used by servos.
 *
 * @pre SetMaxPositivePwm() called.
 * @pre SetMinNegativePwm() called.
 *
 * @param pos The position to set the servo between 0.0 and 1.0.
 */
void PWM::SetPosition(float pos)
{
	if (StatusIsFatal()) return;
	if (pos < 0.0)
	{
		pos = 0.0;
	}
	else if (pos > 1.0)
	{
		pos = 1.0;
	}

	// note, need to perform the multiplication below as floating point before converting to int
	unsigned short rawValue = (int32_t)( (pos * (float) GetFullRangeScaleFactor()) + GetMinNegativePwm());
//	printf("MinNegPWM: %d FullRangeScaleFactor: %d Raw value: %5d   Input value: %4.4f\n", GetMinNegativePwm(), GetFullRangeScaleFactor(), rawValue, pos);

//	wpi_assert((rawValue >= GetMinNegativePwm()) && (rawValue <= GetMaxPositivePwm()));
	wpi_assert(rawValue != kPwmDisabled);

	// send the computed pwm value to the FPGA
	SetRaw((unsigned short)rawValue);
}

/**
 * Get the PWM value in terms of a position.
 *
 * This is intended to be used by servos.
 *
 * @pre SetMaxPositivePwm() called.
 * @pre SetMinNegativePwm() called.
 *
 * @return The position the servo is set to between 0.0 and 1.0.
 */
float PWM::GetPosition()
{
	if (StatusIsFatal()) return 0.0;
	int32_t value = GetRaw();
	if (value < GetMinNegativePwm())
	{
		return 0.0;
	}
	else if (value > GetMaxPositivePwm())
	{
		return 1.0;
	}
	else
	{
		return (float)(value - GetMinNegativePwm()) / (float)GetFullRangeScaleFactor();
	}
}

/**
 * Set the PWM value based on a speed.
 *
 * This is intended to be used by speed controllers.
 *
 * @pre SetMaxPositivePwm() called.
 * @pre SetMinPositivePwm() called.
 * @pre SetCenterPwm() called.
 * @pre SetMaxNegativePwm() called.
 * @pre SetMinNegativePwm() called.
 *
 * @param speed The speed to set the speed controller between -1.0 and 1.0.
 */
void PWM::SetSpeed(float speed)
{
	if (StatusIsFatal()) return;
	// clamp speed to be in the range 1.0 >= speed >= -1.0
	if (speed < -1.0)
	{
		speed = -1.0;
	}
	else if (speed > 1.0)
	{
		speed = 1.0;
	}

	// calculate the desired output pwm value by scaling the speed appropriately
	int32_t rawValue;
	if (speed == 0.0)
	{
		rawValue = GetCenterPwm();
	}
	else if (speed > 0.0)
	{
		rawValue = (int32_t)(speed * ((float)GetPositiveScaleFactor()) +
									((float) GetMinPositivePwm()) + 0.5);
	}
	else
	{
		rawValue = (int32_t)(speed * ((float)GetNegativeScaleFactor()) +
									((float) GetMaxNegativePwm()) + 0.5);
	}

	// the above should result in a pwm_value in the valid range
	wpi_assert((rawValue >= GetMinNegativePwm()) && (rawValue <= GetMaxPositivePwm()));
	wpi_assert(rawValue != kPwmDisabled);

	// send the computed pwm value to the FPGA
	SetRaw(rawValue);
}

/**
 * Get the PWM value in terms of speed.
 *
 * This is intended to be used by speed controllers.
 *
 * @pre SetMaxPositivePwm() called.
 * @pre SetMinPositivePwm() called.
 * @pre SetMaxNegativePwm() called.
 * @pre SetMinNegativePwm() called.
 *
 * @return The most recently set speed between -1.0 and 1.0.
 */
float PWM::GetSpeed()
{
	if (StatusIsFatal()) return 0.0;
	int32_t value = GetRaw();
	if (value == PWM::kPwmDisabled)
	{
		return 0.0;
	}
	else if (value > GetMaxPositivePwm())
	{
		return 1.0;
	}
	else if (value < GetMinNegativePwm())
	{
		return -1.0;
	}
	else if (value > GetMinPositivePwm())
	{
		return (float)(value - GetMinPositivePwm()) / (float)GetPositiveScaleFactor();
	}
	else if (value < GetMaxNegativePwm())
	{
		return (float)(value - GetMaxNegativePwm()) / (float)GetNegativeScaleFactor();
	}
	else
	{
		return 0.0;
	}
}

/**
 * Set the PWM value directly to the hardware.
 *
 * Write a raw value to a PWM channel.
 *
 * @param value Raw PWM value.
 */
void PWM::SetRaw(unsigned short value)
{
	if (StatusIsFatal()) return;
	m_module->SetPWM(m_channel, value);
}

/**
 * Get the PWM value directly from the hardware.
 *
 * Read a raw value from a PWM channel.
 *
 * @return Raw PWM control value.
 */
unsigned short PWM::GetRaw()
{
	if (StatusIsFatal()) return 0;
	return m_module->GetPWM(m_channel);
}

/**
 * Slow down the PWM signal for old devices.
 *
 * @param mult The period multiplier to apply to this channel
 */
void PWM::SetPeriodMultiplier(PeriodMultiplier mult)
{
	if (StatusIsFatal()) return;
	switch(mult)
	{
	case kPeriodMultiplier_4X:
		m_module->SetPWMPeriodScale(m_channel, 3); // Squelch 3 out of 4 outputs
		break;
	case kPeriodMultiplier_2X:
		m_module->SetPWMPeriodScale(m_channel, 1); // Squelch 1 out of 2 outputs
		break;
	case kPeriodMultiplier_1X:
		m_module->SetPWMPeriodScale(m_channel, 0); // Don't squelch any outputs
		break;
	default:
		wpi_assert(false);
	}
}


void PWM::ValueChanged(ITable* source, const std::string& key, EntryValue value, bool isNew) {
	SetSpeed(value.f);
}

void PWM::UpdateTable() {
	if (m_table != NULL) {
		m_table->PutNumber("Value", GetSpeed());
	}
}

void PWM::StartLiveWindowMode() {
	SetSpeed(0);
	if (m_table != NULL) {
		m_table->AddTableListener("Value", this, true);
	}
}

void PWM::StopLiveWindowMode() {
	SetSpeed(0);
	if (m_table != NULL) {
		m_table->RemoveTableListener(this);
	}
}

std::string PWM::GetSmartDashboardType() {
	return "Speed Controller";
}

void PWM::InitTable(ITable *subTable) {
	m_table = subTable;
	UpdateTable();
}

ITable * PWM::GetTable() {
	return m_table;
}
