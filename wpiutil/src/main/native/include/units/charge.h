#include "base.h"

	//------------------------------
	//	UNITS OF CHARGE
	//------------------------------

	/**
	 * @namespace	units::charge
	 * @brief		namespace for unit types and containers representing charge values
	 * @details		The SI unit for charge is `coulombs`, and the corresponding `base_unit` category is
	 *				`charge_unit`.
	 * @anchor		chargeContainers
	 * @sa			See unit_t for more information on unit type containers.
	 */
#if !defined(DISABLE_PREDEFINED_UNITS) || defined(ENABLE_PREDEFINED_CHARGE_UNITS)
	UNIT_ADD_WITH_METRIC_PREFIXES(charge, coulomb, coulombs, C, unit<std::ratio<1>, units::category::charge_unit>)
	UNIT_ADD_WITH_METRIC_PREFIXES(charge, ampere_hour, ampere_hours, Ah, compound_unit<current::ampere, time::hours>)

	UNIT_ADD_CATEGORY_TRAIT(charge)
#endif