#include "base.h"

	//------------------------------
	//	UNITS OF AMOUNT OF SUBSTANCE
	//------------------------------

	/**
	 * @namespace	units::substance
	 * @brief		namespace for unit types and containers representing substance values
	 * @details		The SI unit for substance is `moles`, and the corresponding `base_unit` category is
	 *				`substance_unit`.
	 * @anchor		substanceContainers
	 * @sa			See unit_t for more information on unit type containers.
	 */
#if !defined(DISABLE_PREDEFINED_UNITS) || defined(ENABLE_PREDEFINED_SUBSTANCE_UNITS)
	UNIT_ADD(substance, mole, moles, mol, unit<std::ratio<1>, units::category::substance_unit>)

	UNIT_ADD_CATEGORY_TRAIT(substance)
#endif