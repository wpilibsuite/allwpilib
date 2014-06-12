#ifndef Claw_H
#define Claw_H

#include "Commands/Subsystem.h"
#include "WPILib.h"

/**
 * The claw subsystem is a simple system with a motor for opening and closing.
 * If using stronger motors, you should probably use a sensor so that the
 * motors don't stall.
 */
class Claw: public Subsystem {
private:
	SpeedController* motor;
	DigitalInput* contact;

public:
	Claw();
	void InitDefaultCommand() {}

    /**
     * Set the claw motor to move in the open direction.
     */
	void Open();

    /**
     * Set the claw motor to move in the close direction.
     */
	void Close();

    /**
     * Stops the claw motor from moving.
     */
	void Stop();

    /**
     * Return true when the robot is grabbing an object hard enough
     * to trigger the limit switch.
     */
	bool IsGripping();

	void Log() {}
};

#endif
