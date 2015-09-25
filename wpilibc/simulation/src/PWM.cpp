/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "PWM.h"

//#include "NetworkCommunication/UsageReporting.h"
#include "Utility.h"
#include "WPIErrors.h"

const float PWM::kDefaultPwmPeriod = 5.05;
const float PWM::kDefaultPwmCenter = 1.5;
const int32_t PWM::kDefaultPwmStepsDown = 1000;
const int32_t PWM::kPwmDisabled = 0;

/**
 * Allocate a PWM given a channel number.
 *
 * Checks channel value range and allocates the appropriate channel.
 * The allocation is only done to help users ensure that they don't double
 * assign channels.
 * @param channel The PWM channel number. 0-9 are on-board, 10-19 are on the MXP
 * port
 */
PWM::PWM(uint32_t channel)
{
	char buf[64];

	if (!CheckPWMChannel(channel))
	{
		snprintf(buf, 64, "PWM Channel %d", channel);
		wpi_setWPIErrorWithContext(ChannelIndexOutOfRange, buf);
		return;
	}

	sprintf(buf, "pwm/%d", channel);
	impl = new SimContinuousOutput(buf);
	m_channel = channel;
	m_eliminateDeadband = false;

	m_centerPwm = kPwmDisabled; // In simulation, the same thing.
}

PWM::~PWM() {
	if (m_table != nullptr) m_table->RemoveTableListener(this);
}

/**
 * Optionally eliminate the deadband from a speed controller.
 * @param eliminateDeadband If true, set the motor curve on the Jaguar to eliminate
 * the deadband in the middle of the range. Otherwise, keep the full range without
 * modifying any values.
 */
void PWM::EnableDeadbandElimination(bool eliminateDeadband)
{
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
	// Nothing to do in simulation.
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
	// Nothing to do in simulation.
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
	if (pos < 0.0)
	{
		pos = 0.0;
	}
	else if (pos > 1.0)
	{
		pos = 1.0;
	}

	impl->Set(pos);
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
float PWM::GetPosition() const
{
	float value = impl->Get();
	if (value < 0.0)
	{
		return 0.0;
	}
	else if (value > 1.0)
	{
		return 1.0;
	}
	else
	{
		return value;
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
	// clamp speed to be in the range 1.0 >= speed >= -1.0
	if (speed < -1.0)
	{
		speed = -1.0;
	}
	else if (speed > 1.0)
	{
		speed = 1.0;
	}

	impl->Set(speed);
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
float PWM::GetSpeed() const
{
	float value = impl->Get();
	if (value > 1.0)
	{
		return 1.0;
	}
	else if (value < -1.0)
	{
		return -1.0;
	}
	else
	{
		return value;
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
	wpi_assert(value == kPwmDisabled);
	impl->Set(0);
}

/**
 * Slow down the PWM signal for old devices.
 *
 * @param mult The period multiplier to apply to this channel
 */
void PWM::SetPeriodMultiplier(PeriodMultiplier mult)
{
	// Do nothing in simulation.
}


void PWM::ValueChanged(ITable* source, llvm::StringRef key,
                       std::shared_ptr<nt::Value> value, bool isNew) {
  if (!value->IsDouble()) return;
  SetSpeed(value->GetDouble());
}

void PWM::UpdateTable() {
	if (m_table != nullptr) {
		m_table->PutNumber("Value", GetSpeed());
	}
}

void PWM::StartLiveWindowMode() {
	SetSpeed(0);
	if (m_table != nullptr) {
		m_table->AddTableListener("Value", this, true);
	}
}

void PWM::StopLiveWindowMode() {
	SetSpeed(0);
	if (m_table != nullptr) {
		m_table->RemoveTableListener(this);
	}
}

std::string PWM::GetSmartDashboardType() const {
	return "Speed Controller";
}

void PWM::InitTable(std::shared_ptr<ITable> subTable) {
	m_table = subTable;
	UpdateTable();
}

std::shared_ptr<ITable> PWM::GetTable() const {
	return m_table;
}
