#include "base.h"

	//------------------------------
	//	UNITS OF CAPACITANCE
	//------------------------------

	/**
	 * @namespace	units::capacitance
	 * @brief		namespace for unit types and containers representing capacitance values
	 * @details		The SI unit for capacitance is `farads`, and the corresponding `base_unit` category is
	 *				`capacitance_unit`.
	 * @anchor		capacitanceContainers
	 * @sa			See unit_t for more information on unit type containers.
	 */
#if !defined(DISABLE_PREDEFINED_UNITS) || defined(ENABLE_PREDEFINED_CAPACITANCE_UNITS)
	UNIT_ADD_WITH_METRIC_PREFIXES(capacitance, farad, farads, F, unit<std::ratio<1>, units::category::capacitance_unit>)

	UNIT_ADD_CATEGORY_TRAIT(capacitance)
#endif