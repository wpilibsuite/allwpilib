#include "base.h"

	//------------------------------
	//	ANGLE UNITS
	//------------------------------

	/**
	 * @namespace	units::angle
	 * @brief		namespace for unit types and containers representing angle values
	 * @details		The SI unit for angle is `radians`, and the corresponding `base_unit` category is
	 *				`angle_unit`.
	 * @anchor		angleContainers
	 * @sa			See unit_t for more information on unit type containers.
	 */
#if !defined(DISABLE_PREDEFINED_UNITS) || defined(ENABLE_PREDEFINED_ANGLE_UNITS)
	UNIT_ADD_WITH_METRIC_PREFIXES(angle, radian, radians, rad, unit<std::ratio<1>, units::category::angle_unit>)
	UNIT_ADD(angle, degree, degrees, deg, unit<std::ratio<1, 180>, radians, std::ratio<1>>)
	UNIT_ADD(angle, arcminute, arcminutes, arcmin, unit<std::ratio<1, 60>, degrees>)
	UNIT_ADD(angle, arcsecond, arcseconds, arcsec, unit<std::ratio<1, 60>, arcminutes>)
	UNIT_ADD(angle, milliarcsecond, milliarcseconds, mas, milli<arcseconds>)
	UNIT_ADD(angle, turn, turns, tr, unit<std::ratio<2>, radians, std::ratio<1>>)
	UNIT_ADD(angle, gradian, gradians, gon, unit<std::ratio<1, 400>, turns>)

	UNIT_ADD_CATEGORY_TRAIT(angle)
#endif