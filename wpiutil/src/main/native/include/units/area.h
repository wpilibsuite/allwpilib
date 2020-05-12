#include "base.h"

	//------------------------------
	//	AREA UNITS
	//------------------------------

	/**
	 * @namespace	units::area
	 * @brief		namespace for unit types and containers representing area values
	 * @details		The SI unit for area is `square_meters`, and the corresponding `base_unit` category is
	 *				`area_unit`.
	 * @anchor		areaContainers
	 * @sa			See unit_t for more information on unit type containers.
	 */
#if !defined(DISABLE_PREDEFINED_UNITS) || defined(ENABLE_PREDEFINED_AREA_UNITS)
	UNIT_ADD(area, square_meter, square_meters, sq_m, unit<std::ratio<1>, units::category::area_unit>)
	UNIT_ADD(area, square_foot, square_feet, sq_ft, squared<length::feet>)
	UNIT_ADD(area, square_inch, square_inches, sq_in, squared<length::inch>)
	UNIT_ADD(area, square_mile, square_miles, sq_mi, squared<length::miles>)
	UNIT_ADD(area, square_kilometer, square_kilometers, sq_km, squared<length::kilometers>)
	UNIT_ADD(area, hectare, hectares, ha, unit<std::ratio<10000>, square_meters>)
	UNIT_ADD(area, acre, acres, acre, unit<std::ratio<43560>, square_feet>)

	UNIT_ADD_CATEGORY_TRAIT(area)
#endif