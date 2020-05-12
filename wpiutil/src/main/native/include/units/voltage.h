#include "base.h"

	//------------------------------
	//	UNITS OF VOLTAGE
	//------------------------------

	/**
	 * @namespace	units::voltage
	 * @brief		namespace for unit types and containers representing voltage values
	 * @details		The SI unit for voltage is `volts`, and the corresponding `base_unit` category is
	 *				`voltage_unit`.
	 * @anchor		voltageContainers
	 * @sa			See unit_t for more information on unit type containers.
	 */
#if !defined(DISABLE_PREDEFINED_UNITS) || defined(ENABLE_PREDEFINED_VOLTAGE_UNITS)
	UNIT_ADD_WITH_METRIC_PREFIXES(voltage, volt, volts, V, unit<std::ratio<1>, units::category::voltage_unit>)
	UNIT_ADD(voltage, statvolt, statvolts, statV, unit<std::ratio<1000000, 299792458>, volts>)
	UNIT_ADD(voltage, abvolt, abvolts, abV, unit<std::ratio<1, 100000000>, volts>)

	UNIT_ADD_CATEGORY_TRAIT(voltage)
#endif