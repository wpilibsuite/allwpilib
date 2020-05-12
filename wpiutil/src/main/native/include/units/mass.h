#include "base.h"

	//------------------------------
	//	MASS UNITS
	//------------------------------

	/**
	 * @namespace	units::mass
	 * @brief		namespace for unit types and containers representing mass values
	 * @details		The SI unit for mass is `kilograms`, and the corresponding `base_unit` category is
	 *				`mass_unit`.
	 * @anchor		massContainers
	 * @sa			See unit_t for more information on unit type containers.
	 */
#if !defined(DISABLE_PREDEFINED_UNITS) || defined(ENABLE_PREDEFINED_MASS_UNITS)
	UNIT_ADD_WITH_METRIC_PREFIXES(mass, gram, grams, g, unit<std::ratio<1, 1000>, units::category::mass_unit>)
	UNIT_ADD(mass, metric_ton, metric_tons, t, unit<std::ratio<1000>, kilograms>)
	UNIT_ADD(mass, pound, pounds, lb, unit<std::ratio<45359237, 100000000>, kilograms>)
	UNIT_ADD(mass, long_ton, long_tons, ln_t, unit<std::ratio<2240>, pounds>)
	UNIT_ADD(mass, short_ton, short_tons, sh_t, unit<std::ratio<2000>, pounds>)
	UNIT_ADD(mass, stone, stone, st, unit<std::ratio<14>, pounds>)
	UNIT_ADD(mass, ounce, ounces, oz, unit<std::ratio<1, 16>, pounds>)
	UNIT_ADD(mass, carat, carats, ct, unit<std::ratio<200>, milligrams>)
	UNIT_ADD(mass, slug, slugs, slug, unit<std::ratio<145939029, 10000000>, kilograms>)

	UNIT_ADD_CATEGORY_TRAIT(mass)
#endif
