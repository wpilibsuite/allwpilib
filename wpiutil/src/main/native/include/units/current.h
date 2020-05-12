#include "base.h"

	//------------------------------
	//	UNITS OF CURRENT
	//------------------------------
	/**
	 * @namespace	units::current
	 * @brief		namespace for unit types and containers representing current values
	 * @details		The SI unit for current is `amperes`, and the corresponding `base_unit` category is
	 *				`current_unit`.
	 * @anchor		currentContainers
	 * @sa			See unit_t for more information on unit type containers.
	 */
#if !defined(DISABLE_PREDEFINED_UNITS) || defined(ENABLE_PREDEFINED_CURRENT_UNITS)
	UNIT_ADD_WITH_METRIC_PREFIXES(current, ampere, amperes, A, unit<std::ratio<1>, units::category::current_unit>)

	UNIT_ADD_CATEGORY_TRAIT(current)
#endif