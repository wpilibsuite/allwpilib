##include "base.h"

	//------------------------------
	//	UNITS OF POWER
	//------------------------------

	/**
	 * @namespace	units::power
	 * @brief		namespace for unit types and containers representing power values
	 * @details		The SI unit for power is `watts`, and the corresponding `base_unit` category is
	 *				`power_unit`.
	 * @anchor		powerContainers
	 * @sa			See unit_t for more information on unit type containers.
	 */
#if !defined(DISABLE_PREDEFINED_UNITS) || defined(ENABLE_PREDEFINED_POWER_UNITS)
	UNIT_ADD_WITH_METRIC_PREFIXES(power, watt, watts, W, unit<std::ratio<1>, units::category::power_unit>)
	UNIT_ADD(power, horsepower, horsepower, hp, unit<std::ratio<7457, 10>, watts>)
	UNIT_ADD_DECIBEL(power, watt, dBW)
	UNIT_ADD_DECIBEL(power, milliwatt, dBm)

	UNIT_ADD_CATEGORY_TRAIT(power)
#endif