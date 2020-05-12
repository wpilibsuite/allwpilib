#include "base.h"

	//------------------------------
	//	UNITS OF FORCE
	//------------------------------

	/**
	 * @namespace	units::force
	 * @brief		namespace for unit types and containers representing force values
	 * @details		The SI unit for force is `newtons`, and the corresponding `base_unit` category is
	 *				`force_unit`.
	 * @anchor		forceContainers
	 * @sa			See unit_t for more information on unit type containers.
	 */
#if !defined(DISABLE_PREDEFINED_UNITS) || defined(ENABLE_PREDEFINED_FORCE_UNITS)
	UNIT_ADD_WITH_METRIC_PREFIXES(force, newton, newtons, N, unit<std::ratio<1>, units::category::force_unit>)
	UNIT_ADD(force, pound, pounds, lbf, compound_unit<mass::slug, length::foot, inverse<squared<time::seconds>>>)
	UNIT_ADD(force, dyne, dynes, dyn, unit<std::ratio<1, 100000>, newtons>)
	UNIT_ADD(force, kilopond, kiloponds, kp, compound_unit<acceleration::standard_gravity, mass::kilograms>)
	UNIT_ADD(force, poundal, poundals, pdl, compound_unit<mass::pound, length::foot, inverse<squared<time::seconds>>>)

	UNIT_ADD_CATEGORY_TRAIT(force)
#endif