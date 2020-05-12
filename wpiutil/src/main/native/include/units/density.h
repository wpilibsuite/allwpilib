#include "base.h"

	//------------------------------
	//	UNITS OF DENSITY
	//------------------------------

	/**
	 * @namespace	units::density
	 * @brief		namespace for unit types and containers representing density values
	 * @details		The SI unit for density is `kilograms_per_cubic_meter`, and the corresponding `base_unit` category is
	 *				`density_unit`.
	 * @anchor		densityContainers
	 * @sa			See unit_t for more information on unit type containers.
	 */
#if !defined(DISABLE_PREDEFINED_UNITS) || defined(ENABLE_PREDEFINED_DENSITY_UNITS)
	UNIT_ADD(density, kilograms_per_cubic_meter, kilograms_per_cubic_meter, kg_per_cu_m, unit<std::ratio<1>, units::category::density_unit>)
	UNIT_ADD(density, grams_per_milliliter, grams_per_milliliter, g_per_mL, compound_unit<mass::grams, inverse<volume::milliliter>>)
	UNIT_ADD(density, kilograms_per_liter, kilograms_per_liter, kg_per_L, unit<std::ratio<1>, compound_unit<mass::grams, inverse<volume::milliliter>>>)
	UNIT_ADD(density, ounces_per_cubic_foot, ounces_per_cubic_foot, oz_per_cu_ft, compound_unit<mass::ounces, inverse<volume::cubic_foot>>)
	UNIT_ADD(density, ounces_per_cubic_inch, ounces_per_cubic_inch, oz_per_cu_in, compound_unit<mass::ounces, inverse<volume::cubic_inch>>)
	UNIT_ADD(density, ounces_per_gallon, ounces_per_gallon, oz_per_gal, compound_unit<mass::ounces, inverse<volume::gallon>>)
	UNIT_ADD(density, pounds_per_cubic_foot, pounds_per_cubic_foot, lb_per_cu_ft, compound_unit<mass::pounds, inverse<volume::cubic_foot>>)
	UNIT_ADD(density, pounds_per_cubic_inch, pounds_per_cubic_inch, lb_per_cu_in, compound_unit<mass::pounds, inverse<volume::cubic_inch>>)
	UNIT_ADD(density, pounds_per_gallon, pounds_per_gallon, lb_per_gal, compound_unit<mass::pounds, inverse<volume::gallon>>)
	UNIT_ADD(density, slugs_per_cubic_foot, slugs_per_cubic_foot, slug_per_cu_ft, compound_unit<mass::slugs, inverse<volume::cubic_foot>>)

	UNIT_ADD_CATEGORY_TRAIT(density)
#endif