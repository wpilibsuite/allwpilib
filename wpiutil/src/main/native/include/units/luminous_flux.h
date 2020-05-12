#include "base.h"

	//------------------------------
	//	UNITS OF LUMINOUS FLUX
	//------------------------------

	/**
	 * @namespace	units::luminous_flux
	 * @brief		namespace for unit types and containers representing luminous_flux values
	 * @details		The SI unit for luminous_flux is `lumens`, and the corresponding `base_unit` category is
	 *				`luminous_flux_unit`.
	 * @anchor		luminousFluxContainers
	 * @sa			See unit_t for more information on unit type containers.
	 */
#if !defined(DISABLE_PREDEFINED_UNITS) || defined(ENABLE_PREDEFINED_LUMINOUS_FLUX_UNITS)
	UNIT_ADD_WITH_METRIC_PREFIXES(luminous_flux, lumen, lumens, lm, unit<std::ratio<1>, units::category::luminous_flux_unit>)

	UNIT_ADD_CATEGORY_TRAIT(luminous_flux)
#endif