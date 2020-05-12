#include "base.h"

	//------------------------------
	//	UNITS OF PRESSURE
	//------------------------------

	/**
	 * @namespace	units::pressure
	 * @brief		namespace for unit types and containers representing pressure values
	 * @details		The SI unit for pressure is `pascals`, and the corresponding `base_unit` category is
	 *				`pressure_unit`.
	 * @anchor		pressureContainers
	 * @sa			See unit_t for more information on unit type containers.
	 */
#if !defined(DISABLE_PREDEFINED_UNITS) || defined(ENABLE_PREDEFINED_PRESSURE_UNITS)
	UNIT_ADD_WITH_METRIC_PREFIXES(pressure, pascal, pascals, Pa, unit<std::ratio<1>, units::category::pressure_unit>)
	UNIT_ADD(pressure, bar, bars, bar, unit<std::ratio<100>, kilo<pascals>>)
	UNIT_ADD(pressure, mbar, mbars, mbar, unit<std::ratio<1>, milli<bar>>)
	UNIT_ADD(pressure, atmosphere, atmospheres, atm, unit<std::ratio<101325>, pascals>)
	UNIT_ADD(pressure, pounds_per_square_inch, pounds_per_square_inch, psi, compound_unit<force::pounds, inverse<squared<length::inch>>>)
	UNIT_ADD(pressure, torr, torrs, torr, unit<std::ratio<1, 760>, atmospheres>)

	UNIT_ADD_CATEGORY_TRAIT(pressure)
#endif