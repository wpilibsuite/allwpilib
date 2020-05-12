#include "base.h"

	//------------------------------
	//	UNITS OF ACCELERATION
	//------------------------------

	/**
	 * @namespace	units::acceleration
	 * @brief		namespace for unit types and containers representing acceleration values
	 * @details		The SI unit for acceleration is `meters_per_second_squared`, and the corresponding `base_unit` category is
	 *				`acceleration_unit`.
	 * @anchor		accelerationContainers
	 * @sa			See unit_t for more information on unit type containers.
	 */
#if !defined(DISABLE_PREDEFINED_UNITS) || defined(ENABLE_PREDEFINED_ACCELERATION_UNITS)
	UNIT_ADD(acceleration, meters_per_second_squared, meters_per_second_squared, mps_sq, unit<std::ratio<1>, units::category::acceleration_unit>)
	UNIT_ADD(acceleration, feet_per_second_squared, feet_per_second_squared, fps_sq, compound_unit<length::feet, inverse<squared<time::seconds>>>)
	UNIT_ADD(acceleration, standard_gravity, standard_gravity, SG, unit<std::ratio<980665, 100000>, meters_per_second_squared>)

	UNIT_ADD_CATEGORY_TRAIT(acceleration)
#endif