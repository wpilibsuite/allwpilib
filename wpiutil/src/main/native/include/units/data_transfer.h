#include "base.h"

	//------------------------------
	//	UNITS OF DATA TRANSFER
	//------------------------------

	/**
	* @namespace	units::data_transfer_rate
	* @brief		namespace for unit types and containers representing data values
	* @details		The base unit for data is `bytes`, and the corresponding `base_unit` category is
	*				`data_unit`.
	* @anchor		dataContainers
	* @sa			See unit_t for more information on unit type containers.
	*/
#if !defined(DISABLE_PREDEFINED_UNITS) || defined(ENABLE_PREDEFINED_DATA_TRANSFER_RATE_UNITS)
	UNIT_ADD_WITH_METRIC_AND_BINARY_PREFIXES(data_transfer_rate, bytes_per_second, bytes_per_second, Bps, unit<std::ratio<1>, units::category::data_transfer_rate_unit>)
	UNIT_ADD(data_transfer_rate, exabytes_per_second, exabytes_per_second, EBps, unit<std::ratio<1000>, petabytes_per_second>)
	UNIT_ADD_WITH_METRIC_AND_BINARY_PREFIXES(data_transfer_rate, bits_per_second, bits_per_second, bps, unit<std::ratio<1, 8>, bytes_per_second>)
	UNIT_ADD(data_transfer_rate, exabits_per_second, exabits_per_second, Ebps, unit<std::ratio<1000>, petabits_per_second>)

	UNIT_ADD_CATEGORY_TRAIT(data_transfer_rate)
#endif