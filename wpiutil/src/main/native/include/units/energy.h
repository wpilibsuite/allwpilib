#include "base.h"

	//------------------------------
	//	UNITS OF ENERGY
	//------------------------------

	/**
	 * @namespace	units::energy
	 * @brief		namespace for unit types and containers representing energy values
	 * @details		The SI unit for energy is `joules`, and the corresponding `base_unit` category is
	 *				`energy_unit`.
	 * @anchor		energyContainers
	 * @sa			See unit_t for more information on unit type containers.
	 */
#if !defined(DISABLE_PREDEFINED_UNITS) || defined(ENABLE_PREDEFINED_ENERGY_UNITS)
	UNIT_ADD_WITH_METRIC_PREFIXES(energy, joule, joules, J, unit<std::ratio<1>, units::category::energy_unit>)
	UNIT_ADD_WITH_METRIC_PREFIXES(energy, calorie, calories, cal, unit<std::ratio<4184, 1000>, joules>)
	UNIT_ADD(energy, kilowatt_hour, kilowatt_hours, kWh, unit<std::ratio<36, 10>, megajoules>)
	UNIT_ADD(energy, watt_hour, watt_hours, Wh, unit<std::ratio<1, 1000>, kilowatt_hours>)
	UNIT_ADD(energy, british_thermal_unit, british_thermal_units, BTU, unit<std::ratio<105505585262, 100000000>, joules>)
	UNIT_ADD(energy, british_thermal_unit_iso, british_thermal_units_iso, BTU_iso, unit<std::ratio<1055056, 1000>, joules>)
	UNIT_ADD(energy, british_thermal_unit_59, british_thermal_units_59, BTU59, unit<std::ratio<1054804, 1000>, joules>)
	UNIT_ADD(energy, therm, therms, thm, unit<std::ratio<100000>, british_thermal_units_59>)
	UNIT_ADD(energy, foot_pound, foot_pounds, ftlbf, unit<std::ratio<13558179483314004, 10000000000000000>, joules>)

	UNIT_ADD_CATEGORY_TRAIT(energy)
#endif