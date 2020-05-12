#include "base.h"

	//------------------------------
	//	UNITS OF TORQUE
	//------------------------------

	/**
	 * @namespace	units::torque
	 * @brief		namespace for unit types and containers representing torque values
	 * @details		The SI unit for torque is `newton_meters`, and the corresponding `base_unit` category is
	 *				`torque_units`.
	 * @anchor		torqueContainers
	 * @sa			See unit_t for more information on unit type containers.
	 */
#if !defined(DISABLE_PREDEFINED_UNITS) || defined(ENABLE_PREDEFINED_TORQUE_UNITS)
	UNIT_ADD(torque, newton_meter, newton_meters, Nm, unit<std::ratio<1>, units::energy::joule>)
	UNIT_ADD(torque, foot_pound, foot_pounds, ftlb, compound_unit<length::foot, force::pounds>)
	UNIT_ADD(torque, foot_poundal, foot_poundals, ftpdl, compound_unit<length::foot, force::poundal>)
	UNIT_ADD(torque, inch_pound, inch_pounds, inlb, compound_unit<length::inch, force::pounds>)
	UNIT_ADD(torque, meter_kilogram, meter_kilograms, mkgf, compound_unit<length::meter, force::kiloponds>)

	UNIT_ADD_CATEGORY_TRAIT(torque)
#endif