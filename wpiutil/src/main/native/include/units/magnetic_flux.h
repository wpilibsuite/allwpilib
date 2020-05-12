#include "base.h"

	//------------------------------
	//	UNITS OF MAGNETIC FLUX
	//------------------------------

	/**
	 * @namespace	units::magnetic_flux
	 * @brief		namespace for unit types and containers representing magnetic_flux values
	 * @details		The SI unit for magnetic_flux is `webers`, and the corresponding `base_unit` category is
	 *				`magnetic_flux_unit`.
	 * @anchor		magneticFluxContainers
	 * @sa			See unit_t for more information on unit type containers.
	 */
#if !defined(DISABLE_PREDEFINED_UNITS) || defined(ENABLE_PREDEFINED_MAGNETIC_FLUX_UNITS)
	UNIT_ADD_WITH_METRIC_PREFIXES(magnetic_flux, weber, webers, Wb, unit<std::ratio<1>, units::category::magnetic_flux_unit>)
	UNIT_ADD(magnetic_flux, maxwell, maxwells, Mx, unit<std::ratio<1, 100000000>, webers>)

	UNIT_ADD_CATEGORY_TRAIT(magnetic_flux)
#endif