#include "base.h"

	//------------------------------
	//	UNITS OF INDUCTANCE
	//------------------------------

	/**
	 * @namespace	units::inductance
	 * @brief		namespace for unit types and containers representing inductance values
	 * @details		The SI unit for inductance is `henrys`, and the corresponding `base_unit` category is
	 *				`inductance_unit`.
	 * @anchor		inductanceContainers
	 * @sa			See unit_t for more information on unit type containers.
	 */
#if !defined(DISABLE_PREDEFINED_UNITS) || defined(ENABLE_PREDEFINED_INDUCTANCE_UNITS)
	UNIT_ADD_WITH_METRIC_PREFIXES(inductance, henry, henries, H, unit<std::ratio<1>, units::category::inductance_unit>)

	UNIT_ADD_CATEGORY_TRAIT(inductance)
#endif