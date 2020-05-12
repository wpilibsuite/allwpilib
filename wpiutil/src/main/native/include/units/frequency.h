#include "base.h"

	//------------------------------
	//	FREQUENCY UNITS
	//------------------------------

	/**
	 * @namespace	units::frequency
	 * @brief		namespace for unit types and containers representing frequency values
	 * @details		The SI unit for frequency is `hertz`, and the corresponding `base_unit` category is
	 *				`frequency_unit`.
	 * @anchor		frequencyContainers
	 * @sa			See unit_t for more information on unit type containers.
	 */
#if !defined(DISABLE_PREDEFINED_UNITS) || defined(ENABLE_PREDEFINED_FREQUENCY_UNITS)
	UNIT_ADD_WITH_METRIC_PREFIXES(frequency, hertz, hertz, Hz, unit<std::ratio<1>, units::category::frequency_unit>)

	UNIT_ADD_CATEGORY_TRAIT(frequency)
#endif