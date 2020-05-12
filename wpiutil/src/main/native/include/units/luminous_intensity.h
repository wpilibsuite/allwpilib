#include "base.h"

	//------------------------------
	//	UNITS OF LUMINOUS INTENSITY
	//------------------------------

	/**
	 * @namespace	units::luminous_intensity
	 * @brief		namespace for unit types and containers representing luminous_intensity values
	 * @details		The SI unit for luminous_intensity is `candelas`, and the corresponding `base_unit` category is
	 *				`luminous_intensity_unit`.
	 * @anchor		luminousIntensityContainers
	 * @sa			See unit_t for more information on unit type containers.
	 */
#if !defined(DISABLE_PREDEFINED_UNITS) || defined(ENABLE_PREDEFINED_LUMINOUS_INTENSITY_UNITS)
	UNIT_ADD_WITH_METRIC_PREFIXES(luminous_intensity, candela, candelas, cd, unit<std::ratio<1>, units::category::luminous_intensity_unit>)

	UNIT_ADD_CATEGORY_TRAIT(luminous_intensity)
#endif