#include "base.h"

	//------------------------------
	//	UNITS OF IMPEDANCE
	//------------------------------

	/**
	 * @namespace	units::impedance
	 * @brief		namespace for unit types and containers representing impedance values
	 * @details		The SI unit for impedance is `ohms`, and the corresponding `base_unit` category is
	 *				`impedance_unit`.
	 * @anchor		impedanceContainers
	 * @sa			See unit_t for more information on unit type containers.
	 */
#if !defined(DISABLE_PREDEFINED_UNITS) || defined(ENABLE_PREDEFINED_IMPEDANCE_UNITS)
	UNIT_ADD_WITH_METRIC_PREFIXES(impedance, ohm, ohms, Ohm, unit<std::ratio<1>, units::category::impedance_unit>)

	UNIT_ADD_CATEGORY_TRAIT(impedance)
#endif