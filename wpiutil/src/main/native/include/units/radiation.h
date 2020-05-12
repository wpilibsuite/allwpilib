#include "radiation.h"

	//------------------------------
	//	UNITS OF RADIATION
	//------------------------------

	/**
	 * @namespace	units::radiation
	 * @brief		namespace for unit types and containers representing radiation values
	 * @details		The SI units for radiation are:
	 *				- source activity:	becquerel
	 *				- absorbed dose:	gray
	 *				- equivalent dose:	sievert
	 * @anchor		radiationContainers
	 * @sa			See unit_t for more information on unit type containers.
	 */
#if !defined(DISABLE_PREDEFINED_UNITS) || defined(ENABLE_PREDEFINED_RADIATION_UNITS)
	UNIT_ADD_WITH_METRIC_PREFIXES(radiation, becquerel, becquerels, Bq, unit<std::ratio<1>, units::frequency::hertz>)
	UNIT_ADD_WITH_METRIC_PREFIXES(radiation, gray, grays, Gy, compound_unit<energy::joules, inverse<mass::kilogram>>)
	UNIT_ADD_WITH_METRIC_PREFIXES(radiation, sievert, sieverts, Sv, unit<std::ratio<1>, grays>)
	UNIT_ADD(radiation, curie, curies, Ci, unit<std::ratio<37>, gigabecquerels>)
	UNIT_ADD(radiation, rutherford, rutherfords, rd, unit<std::ratio<1>, megabecquerels>)
	UNIT_ADD(radiation, rad, rads, rads, unit<std::ratio<1>, centigrays>)

	UNIT_ADD_CATEGORY_TRAIT(radioactivity)
#endif