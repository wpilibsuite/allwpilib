#include "base.h"

	//------------------------------
	//	UNITS OF CONCENTRATION
	//------------------------------

	/**
	 * @namespace	units::concentration
	 * @brief		namespace for unit types and containers representing concentration values
	 * @details		The SI unit for concentration is `parts_per_million`, and the corresponding `base_unit` category is
	 *				`scalar_unit`.
	 * @anchor		concentrationContainers
	 * @sa			See unit_t for more information on unit type containers.
	 */
#if !defined(DISABLE_PREDEFINED_UNITS) || defined(ENABLE_PREDEFINED_CONCENTRATION_UNITS)
	UNIT_ADD(concentration, ppm, parts_per_million, ppm, unit<std::ratio<1, 1000000>, units::category::scalar_unit>)
	UNIT_ADD(concentration, ppb, parts_per_billion, ppb, unit<std::ratio<1, 1000>, parts_per_million>)
	UNIT_ADD(concentration, ppt, parts_per_trillion, ppt, unit<std::ratio<1, 1000>, parts_per_billion>)
	UNIT_ADD(concentration, percent, percent, pct, unit<std::ratio<1, 100>, units::category::scalar_unit>)

	UNIT_ADD_CATEGORY_TRAIT(concentration)
#endif