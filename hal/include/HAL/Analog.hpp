#pragma once

#ifdef __vxworks
#include <vxWorks.h>
#else
#include <stdint.h>
#endif

enum AnalogTriggerType
{
	kInWindow = 0,
	kState = 1,
	kRisingPulse = 2,
	kFallingPulse = 3
};

extern "C"
{
	// Analog output functions
	void* initializeAnalogOutputPort(void* port_pointer, int32_t *status);
	void setAnalogOutput(void* analog_port_pointer, double voltage, int32_t *status);
	double getAnalogOutput(void* analog_port_pointer, int32_t *status);
	bool checkAnalogOutputChannel(uint32_t pin);

	// Analog input functions
	void* initializeAnalogInputPort(void* port_pointer, int32_t *status);
	bool checkAnalogModule(uint8_t module);
	bool checkAnalogInputChannel(uint32_t pin);

	void setAnalogSampleRate(double samplesPerSecond, int32_t *status);
	float getAnalogSampleRate(int32_t *status);
	void setAnalogAverageBits(void* analog_port_pointer, uint32_t bits, int32_t *status);
	uint32_t getAnalogAverageBits(void* analog_port_pointer, int32_t *status);
	void setAnalogOversampleBits(void* analog_port_pointer, uint32_t bits, int32_t *status);
	uint32_t getAnalogOversampleBits(void* analog_port_pointer, int32_t *status);
	int16_t getAnalogValue(void* analog_port_pointer, int32_t *status);
	int32_t getAnalogAverageValue(void* analog_port_pointer, int32_t *status);
	int32_t getAnalogVoltsToValue(void* analog_port_pointer, double voltage, int32_t *status);
	float getAnalogVoltage(void* analog_port_pointer, int32_t *status);
	float getAnalogAverageVoltage(void* analog_port_pointer, int32_t *status);
	uint32_t getAnalogLSBWeight(void* analog_port_pointer, int32_t *status);
	int32_t getAnalogOffset(void* analog_port_pointer, int32_t *status);

	bool isAccumulatorChannel(void* analog_port_pointer, int32_t *status);
	void initAccumulator(void* analog_port_pointer, int32_t *status);
	void resetAccumulator(void* analog_port_pointer, int32_t *status);
	void setAccumulatorCenter(void* analog_port_pointer, int32_t center, int32_t *status);
	void setAccumulatorDeadband(void* analog_port_pointer, int32_t deadband, int32_t *status);
	int64_t getAccumulatorValue(void* analog_port_pointer, int32_t *status);
	uint32_t getAccumulatorCount(void* analog_port_pointer, int32_t *status);
	void getAccumulatorOutput(void* analog_port_pointer, int64_t *value, uint32_t *count,
			int32_t *status);

	void* initializeAnalogTrigger(void* port_pointer, uint32_t *index, int32_t *status);
	void cleanAnalogTrigger(void* analog_trigger_pointer, int32_t *status);
	void setAnalogTriggerLimitsRaw(void* analog_trigger_pointer, int32_t lower, int32_t upper,
			int32_t *status);
	void setAnalogTriggerLimitsVoltage(void* analog_trigger_pointer, double lower, double upper,
			int32_t *status);
	void setAnalogTriggerAveraged(void* analog_trigger_pointer, bool useAveragedValue,
			int32_t *status);
	void setAnalogTriggerFiltered(void* analog_trigger_pointer, bool useFilteredValue,
			int32_t *status);
	bool getAnalogTriggerInWindow(void* analog_trigger_pointer, int32_t *status);
	bool getAnalogTriggerTriggerState(void* analog_trigger_pointer, int32_t *status);
	bool getAnalogTriggerOutput(void* analog_trigger_pointer, AnalogTriggerType type,
			int32_t *status);

	//// Float JNA Hack
	// Float
	int getAnalogSampleRateIntHack(int32_t *status);
	int getAnalogVoltageIntHack(void* analog_port_pointer, int32_t *status);
	int getAnalogAverageVoltageIntHack(void* analog_port_pointer, int32_t *status);

	// Doubles
	void setAnalogSampleRateIntHack(int samplesPerSecond, int32_t *status);
	int32_t getAnalogVoltsToValueIntHack(void* analog_port_pointer, int voltage, int32_t *status);
	void setAnalogTriggerLimitsVoltageIntHack(void* analog_trigger_pointer, int lower, int upper,
			int32_t *status);
}
