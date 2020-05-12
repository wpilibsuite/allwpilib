#include "base.h"

	//----------------------------------------
	//	UNITS OF MAGNETIC FIELD STRENGTH
	//----------------------------------------

	/**
	 * @namespace	units::magnetic_field_strength
	 * @brief		namespace for unit types and containers representing magnetic_field_strength values
	 * @details		The SI unit for magnetic_field_strength is `teslas`, and the corresponding `base_unit` category is
	 *				`magnetic_field_strength_unit`.
	 * @anchor		magneticFieldStrengthContainers
	 * @sa			See unit_t for more information on unit type containers.
	 */
	// Unfortunately `_T` is a WINAPI macro, so we have to use `_Te` as the tesla abbreviation.
#if !defined(DISABLE_PREDEFINED_UNITS) || defined(ENABLE_PREDEFINED_MAGNETIC_FIELD_STRENGTH_UNITS)
	UNIT_ADD_WITH_METRIC_PREFIXES(magnetic_field_strength, tesla, teslas, Te, unit<std::ratio<1>, units::category::magnetic_field_strength_unit>)
	UNIT_ADD(magnetic_field_strength, gauss, gauss, G, compound_unit<magnetic_flux::maxwell, inverse<squared<length::centimeter>>>)

	UNIT_ADD_CATEGORY_TRAIT(magnetic_field_strength)
#endif