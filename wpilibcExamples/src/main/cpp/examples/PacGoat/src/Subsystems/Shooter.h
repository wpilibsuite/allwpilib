/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <Commands/Subsystem.h>
#include <DigitalInput.h>
#include <DoubleSolenoid.h>
#include <Solenoid.h>

/**
 * The Shooter subsystem handles shooting. The mechanism for shooting is
 * slightly complicated because it has to pneumatic cylinders for shooting, and
 * a third latch to allow the pressure to partially build up and reduce the
 * effect of the airflow. For shorter shots, when full power isn't needed, only
 * one cylinder fires.
 *
 * NOTE: Simulation currently approximates this as as single pneumatic cylinder
 * and ignores the latch.
 */
class Shooter : public frc::Subsystem {
public:
	Shooter();
	void InitDefaultCommand() override;

	/**
	 * Extend both solenoids to shoot.
	 */
	void ExtendBoth();

	/**
	 * Retract both solenoids to prepare to shoot.
	 */
	void RetractBoth();

	/**
	 * Extend solenoid 1 to shoot.
	 */
	void Extend1();

	/**
	 * Retract solenoid 1 to prepare to shoot.
	 */
	void Retract1();

	/**
	 * Extend solenoid 2 to shoot.
	 */
	void Extend2();

	/**
	 * Retract solenoid 2 to prepare to shoot.
	 */
	void Retract2();

	/**
	 * Turns off the piston1 double solenoid. This won't actuate anything
	 * because double solenoids preserve their state when turned off. This
	 * should be called in order to reduce the amount of time that the coils
	 * are
	 * powered.
	 */
	void Off1();

	/**
	 * Turns off the piston1 double solenoid. This won't actuate anything
	 * because double solenoids preserve their state when turned off. This
	 * should be called in order to reduce the amount of time that the coils
	 * are
	 * powered.
	 */
	void Off2();

	/**
	 * Release the latch so that we can shoot
	 */
	void Unlatch();

	/**
	 * Latch so that pressure can build up and we aren't limited by air
	 * flow.
	 */
	void Latch();

	/**
	 * Toggles the latch postions
	 */
	void ToggleLatchPosition();

	/**
	 * @return Whether or not piston 1 is fully extended.
	 */
	bool Piston1IsExtended();

	/**
	 * @return Whether or not piston 1 is fully retracted.
	 */
	bool Piston1IsRetracted();

	/**
	 * Turns off all double solenoids. Double solenoids hold their position
	 * when
	 * they are turned off. We should turn them off whenever possible to
	 * extend
	 * the life of the coils
	 */
	void OffBoth();

	/**
	 * @return Whether or not the goal is hot as read by the banner sensor
	 */
	bool GoalIsHot();

private:
	// Devices
	frc::DoubleSolenoid m_piston1{3, 4};
	frc::DoubleSolenoid m_piston2{5, 6};
	frc::Solenoid m_latchPiston{1, 2};
	frc::DigitalInput m_piston1ReedSwitchFront{9};
	frc::DigitalInput m_piston1ReedSwitchBack{11};
	frc::DigitalInput m_hotGoalSensor{
			7};  // NOTE: Currently ignored in simulation
};
