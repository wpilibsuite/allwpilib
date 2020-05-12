#include "base.h"


	//------------------------------
	//	TIME UNITS
	//------------------------------

	/**
	 * @namespace	units::time
	 * @brief		namespace for unit types and containers representing time values
	 * @details		The SI unit for time is `seconds`, and the corresponding `base_unit` category is
	 *				`time_unit`.
	 * @anchor		timeContainers
	 * @sa			See unit_t for more information on unit type containers.
	 */
#if !defined(DISABLE_PREDEFINED_UNITS) || defined(ENABLE_PREDEFINED_TIME_UNITS)
	UNIT_ADD_WITH_METRIC_PREFIXES(time, second, seconds, s, unit<std::ratio<1>, units::category::time_unit>)
	UNIT_ADD(time, minute, minutes, min, unit<std::ratio<60>, seconds>)
	UNIT_ADD(time, hour, hours, hr, unit<std::ratio<60>, minutes>)
	UNIT_ADD(time, day, days, d, unit<std::ratio<24>, hours>)
	UNIT_ADD(time, week, weeks, wk, unit<std::ratio<7>, days>)
	UNIT_ADD(time, year, years, yr, unit<std::ratio<365>, days>)
	UNIT_ADD(time, julian_year, julian_years, a_j,	unit<std::ratio<31557600>, seconds>)
	UNIT_ADD(time, gregorian_year, gregorian_years, a_g, unit<std::ratio<31556952>, seconds>)

	UNIT_ADD_CATEGORY_TRAIT(time)
#endif