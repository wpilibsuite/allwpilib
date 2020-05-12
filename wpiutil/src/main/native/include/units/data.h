#include "data.h"

	//------------------------------
	//	UNITS OF DATA
	//------------------------------

	/**
	 * @namespace	units::data
	 * @brief		namespace for unit types and containers representing data values
	 * @details		The base unit for data is `bytes`, and the corresponding `base_unit` category is
	 *				`data_unit`.
	 * @anchor		dataContainers
	 * @sa			See unit_t for more information on unit type containers.
	 */
#if !defined(DISABLE_PREDEFINED_UNITS) || defined(ENABLE_PREDEFINED_DATA_UNITS)
	UNIT_ADD_WITH_METRIC_AND_BINARY_PREFIXES(data, byte, bytes, B, unit<std::ratio<1>, units::category::data_unit>)
	UNIT_ADD(data, exabyte, exabytes, EB, unit<std::ratio<1000>, petabytes>)
	UNIT_ADD_WITH_METRIC_AND_BINARY_PREFIXES(data, bit, bits, b, unit<std::ratio<1, 8>, byte>)
	UNIT_ADD(data, exabit, exabits, Eb, unit<std::ratio<1000>, petabits>)

	UNIT_ADD_CATEGORY_TRAIT(data)
#endif
