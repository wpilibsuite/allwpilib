/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2017 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

namespace frc {
namespace sim {

	/**
	 * The Model class is an interface used to represent a physical system during simulation.
	 * This interface is to be used with a known 'state', containing all the instance-relevant 
	 * information (like speed, position, etc), and a 'data' instance containing immutable information
	 * about the robot (like PWM channel values).
	 *
	 * Each Model instance represents a type of mechanism or device (like a motor), but not necessarily
	 * one instance per device on the robot. Instead, each physical device has its own state instance
	 * that is passed to this class. 
	 *
	 * This class is intended to be pure with no side effects. All mutable data should be handled in the
	 * state instance exposed by the class template.
	 */
	template<typename STATE, typename DATA>
	class Model {
	public:
		virtual ~Model() = default;

		/**
		 * Calculate the state for the model using the last known state, immutable data, as well as 
		 * the time difference between when the last state was recorded. This function is pure, with
		 * no side effects.
		 *
		 * The state parameter should not be modified outside of this function.
		 */
		virtual STATE calculate(STATE state, const DATA data, const double dt) const = 0;
	};
} // namespace sim
} // namespace frc