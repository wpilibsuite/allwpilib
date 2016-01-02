/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "SensorBase.h"
#include "simulation/SimContinuousOutput.h"
#include "LiveWindow/LiveWindowSendable.h"
#include "tables/ITableListener.h"

#include <memory>

/**
 * Class implements the PWM generation in the FPGA.
 *
 * The values supplied as arguments for PWM outputs range from -1.0 to 1.0. They are mapped
 * to the hardware dependent values, in this case 0-255 for the FPGA.
 * Changes are immediately sent to the FPGA, and the update occurs at the next
 * FPGA cycle. There is no delay.
 *
 * As of revision 0.1.10 of the FPGA, the FPGA interprets the 0-255 values as follows:
 *   - 255 = full "forward"
 *   - 254 to 129 = linear scaling from "full forward" to "center"
 *   - 128 = center value
 *   - 127 to 2 = linear scaling from "center" to "full reverse"
 *   - 1 = full "reverse"
 *   - 0 = disabled (i.e. PWM output is held low)
 */
class PWM : public SensorBase, public ITableListener, public LiveWindowSendable
{
public:
	enum PeriodMultiplier
	{
		kPeriodMultiplier_1X = 1,
		kPeriodMultiplier_2X = 2,
		kPeriodMultiplier_4X = 4
	};

	explicit PWM(uint32_t channel);
	virtual ~PWM();
	virtual void SetRaw(unsigned short value);
	void SetPeriodMultiplier(PeriodMultiplier mult);
	void EnableDeadbandElimination(bool eliminateDeadband);
	void SetBounds(int32_t max, int32_t deadbandMax, int32_t center, int32_t deadbandMin,
			int32_t min);
	void SetBounds(double max, double deadbandMax, double center, double deadbandMin, double min);
	uint32_t GetChannel() const
	{
		return m_channel;
	}

protected:
	/**
	 * kDefaultPwmPeriod is in ms
	 *
	 * - 20ms periods (50 Hz) are the "safest" setting in that this works for all devices
	 * - 20ms periods seem to be desirable for Vex Motors
	 * - 20ms periods are the specified period for HS-322HD servos, but work reliably down
	 *      to 10.0 ms; starting at about 8.5ms, the servo sometimes hums and get hot;
	 *      by 5.0ms the hum is nearly continuous
	 * - 10ms periods work well for Victor 884
	 * - 5ms periods allows higher update rates for Luminary Micro Jaguar speed controllers.
	 *      Due to the shipping firmware on the Jaguar, we can't run the update period less
	 *      than 5.05 ms.
	 *
	 * kDefaultPwmPeriod is the 1x period (5.05 ms).  In hardware, the period scaling is implemented as an
	 * output squelch to get longer periods for old devices.
	 */
	static const float kDefaultPwmPeriod;
	/**
	 * kDefaultPwmCenter is the PWM range center in ms
	 */
	static const float kDefaultPwmCenter;
	/**
	 * kDefaultPWMStepsDown is the number of PWM steps below the centerpoint
	 */
	static const int32_t kDefaultPwmStepsDown;
	static const int32_t kPwmDisabled;

	virtual void SetPosition(float pos);
	virtual float GetPosition() const;
	virtual void SetSpeed(float speed);
	virtual float GetSpeed() const;

	bool m_eliminateDeadband;
	int32_t m_centerPwm;

	void ValueChanged(ITable* source, llvm::StringRef key,
                    std::shared_ptr<nt::Value> value, bool isNew) override;
	void UpdateTable() override;
	void StartLiveWindowMode() override;
	void StopLiveWindowMode() override;
	std::string GetSmartDashboardType() const override;
	void InitTable(std::shared_ptr<ITable> subTable) override;
	std::shared_ptr<ITable> GetTable() const override;

	std::shared_ptr<ITable> m_table;

private:
	uint32_t m_channel;
	SimContinuousOutput* impl;
};
