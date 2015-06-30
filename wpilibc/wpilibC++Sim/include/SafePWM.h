/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/
#pragma once

#include "MotorSafety.h"
#include "PWM.h"
#include "MotorSafetyHelper.h"
#include <memory>

/**
 * A safe version of the PWM class.
 * It is safe because it implements the MotorSafety interface that provides timeouts
 * in the event that the motor value is not updated before the expiration time.
 * This delegates the actual work to a MotorSafetyHelper object that is used for all
 * objects that implement MotorSafety.
 */
class SafePWM : public PWM, public MotorSafety
{
public:
	explicit SafePWM(uint32_t channel);
	virtual ~SafePWM() = default;

	void SetExpiration(float timeout);
	float GetExpiration() const;
	bool IsAlive() const;
	void StopMotor();
	bool IsSafetyEnabled() const;
	void SetSafetyEnabled(bool enabled);
	void GetDescription(std::ostringstream& desc) const;

	virtual void SetSpeed(float speed);
private:
	std::unique_ptr<MotorSafetyHelper> m_safetyHelper;
};
