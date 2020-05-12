#include "base.h"

	//------------------------------
	//	UNITS OF ILLUMINANCE
	//------------------------------

	/**
	 * @namespace	units::illuminance
	 * @brief		namespace for unit types and containers representing illuminance values
	 * @details		The SI unit for illuminance is `luxes`, and the corresponding `base_unit` category is
	 *				`illuminance_unit`.
	 * @anchor		illuminanceContainers
	 * @sa			See unit_t for more information on unit type containers.
	 */
#if !defined(DISABLE_PREDEFINED_UNITS) || defined(ENABLE_PREDEFINED_ILLUMINANCE_UNITS)
	UNIT_ADD_WITH_METRIC_PREFIXES(illuminance, lux, luxes, lx, unit<std::ratio<1>, units::category::illuminance_unit>)
	UNIT_ADD(illuminance, footcandle, footcandles, fc, compound_unit<luminous_flux::lumen, inverse<squared<length::foot>>>)
	UNIT_ADD(illuminance, lumens_per_square_inch, lumens_per_square_inch, lm_per_in_sq, compound_unit<luminous_flux::lumen, inverse<squared<length::inch>>>)
	UNIT_ADD(illuminance, phot, phots, ph, compound_unit<luminous_flux::lumens, inverse<squared<length::centimeter>>>)

	UNIT_ADD_CATEGORY_TRAIT(illuminance)
#endif