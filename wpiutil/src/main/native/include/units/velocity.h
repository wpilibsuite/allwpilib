#include "base.h"

	//------------------------------
	//	VELOCITY UNITS
	//------------------------------

	/**
	 * @namespace	units::velocity
	 * @brief		namespace for unit types and containers representing velocity values
	 * @details		The SI unit for velocity is `meters_per_second`, and the corresponding `base_unit` category is
	 *				`velocity_unit`.
	 * @anchor		velocityContainers
	 * @sa			See unit_t for more information on unit type containers.
	 */
#if !defined(DISABLE_PREDEFINED_UNITS) || defined(ENABLE_PREDEFINED_VELOCITY_UNITS)
	UNIT_ADD(velocity, meters_per_second, meters_per_second, mps, unit<std::ratio<1>, units::category::velocity_unit>)
	UNIT_ADD(velocity, feet_per_second, feet_per_second, fps, compound_unit<length::feet, inverse<time::seconds>>)
	UNIT_ADD(velocity, miles_per_hour, miles_per_hour, mph, compound_unit<length::miles, inverse<time::hour>>)
	UNIT_ADD(velocity, kilometers_per_hour, kilometers_per_hour, kph, compound_unit<length::kilometers, inverse<time::hour>>)
	UNIT_ADD(velocity, knot, knots, kts, compound_unit<length::nauticalMiles, inverse<time::hour>>)

	UNIT_ADD_CATEGORY_TRAIT(velocity)
#endif