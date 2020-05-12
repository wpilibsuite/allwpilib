#include "temperature.h"

	//------------------------------
	//	UNITS OF TEMPERATURE
	//------------------------------

	// NOTE: temperature units have special conversion overloads, since they
	// require translations and aren't a reversible transform.

	/**
	 * @namespace	units::temperature
	 * @brief		namespace for unit types and containers representing temperature values
	 * @details		The SI unit for temperature is `kelvin`, and the corresponding `base_unit` category is
	 *				`temperature_unit`.
	 * @anchor		temperatureContainers
	 * @sa			See unit_t for more information on unit type containers.
	 */
#if !defined(DISABLE_PREDEFINED_UNITS) || defined(ENABLE_PREDEFINED_TEMPERATURE_UNITS)
	UNIT_ADD(temperature, kelvin, kelvin, K, unit<std::ratio<1>, units::category::temperature_unit>)
	UNIT_ADD(temperature, celsius, celsius, degC, unit<std::ratio<1>, kelvin, std::ratio<0>, std::ratio<27315, 100>>)
	UNIT_ADD(temperature, fahrenheit, fahrenheit, degF, unit<std::ratio<5, 9>, celsius, std::ratio<0>, std::ratio<-160, 9>>)
	UNIT_ADD(temperature, reaumur, reaumur, Re, unit<std::ratio<10, 8>, celsius>)
	UNIT_ADD(temperature, rankine, rankine, Ra, unit<std::ratio<5, 9>, kelvin>)

	UNIT_ADD_CATEGORY_TRAIT(temperature)
#endif