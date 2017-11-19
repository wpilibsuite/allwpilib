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
	 * The Mutator interface is the last stage of a simulation pipeline. 
	 * A Mutator is designed to take a (now-immutable) state from Model instances
	 * and set some form of (now-mutable) data with it. For example, it may take in
	 * the state of a Motor, and set the data of an Encoder channel with it.
	 */
	template<typename STATE, typename DATA>
	class Mutator {
	public:
		virtual ~Mutator() = default;

		/**
		 * Mutate the data given an immutable state. This function should not modify the state (or any
		 * external values), and should modify only the passed data instance.
		 *
		 * The passed data instance may not be reallocated, only modified.
		 */
		virtual void mutate(STATE const state, DATA &data) const = 0;
	};
} // namespace sim
} // namespace frc