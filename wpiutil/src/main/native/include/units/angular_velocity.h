#include "base.h"

	//------------------------------
	//	ANGULAR VELOCITY UNITS
	//------------------------------

	/**
	 * @namespace	units::angular_velocity
	 * @brief		namespace for unit types and containers representing angular velocity values
	 * @details		The SI unit for angular velocity is `radians_per_second`, and the corresponding `base_unit` category is
	 *				`angular_velocity_unit`.
	 * @anchor		angularVelocityContainers
	 * @sa			See unit_t for more information on unit type containers.
	 */
#if !defined(DISABLE_PREDEFINED_UNITS) || defined(ENABLE_PREDEFINED_ANGULAR_VELOCITY_UNITS)
	UNIT_ADD(angular_velocity, radians_per_second, radians_per_second, rad_per_s, unit<std::ratio<1>, units::category::angular_velocity_unit>)
	UNIT_ADD(angular_velocity, degrees_per_second, degrees_per_second, deg_per_s, compound_unit<angle::degrees, inverse<time::seconds>>)
	UNIT_ADD(angular_velocity, revolutions_per_minute, revolutions_per_minute, rpm, unit<std::ratio<2, 60>, radians_per_second, std::ratio<1>>)
	UNIT_ADD(angular_velocity, milliarcseconds_per_year, milliarcseconds_per_year, mas_per_yr, compound_unit<angle::milliarcseconds, inverse<time::year>>)

	UNIT_ADD_CATEGORY_TRAIT(angular_velocity)
#endif