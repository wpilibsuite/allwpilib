#pragma once
#ifdef __vxworks
#include <vxWorks.h>
#else
#include <stdint.h>
#endif
#include "HAL/cpp/Synchronized.hpp"

enum Mode
{
	kTwoPulse = 0,
	kSemiperiod = 1,
	kPulseLength = 2,
	kExternalDirection = 3
};

extern "C"
{
	void* initializeDigitalPort(void* port_pointer, int32_t *status);
	bool checkPWMChannel(void* digital_port_pointer);
	bool checkRelayChannel(void* digital_port_pointer);

	void setPWM(void* digital_port_pointer, unsigned short value, int32_t *status);
	bool allocatePWMChannel(void* digital_port_pointer, int32_t *status);
	void freePWMChannel(void* digital_port_pointer, int32_t *status);
	unsigned short getPWM(void* digital_port_pointer, int32_t *status);
	void latchPWMZero(void* digital_port_pointer, int32_t *status);
	void setPWMPeriodScale(void* digital_port_pointer, uint32_t squelchMask, int32_t *status);
	void* allocatePWM(int32_t *status);
	void freePWM(void* pwmGenerator, int32_t *status);
	void setPWMRate(double rate, int32_t *status);
	void setPWMDutyCycle(void* pwmGenerator, double dutyCycle, int32_t *status);
	void setPWMOutputChannel(void* pwmGenerator, uint32_t pin, int32_t *status);

	void setRelayForward(void* digital_port_pointer, bool on, int32_t *status);
	void setRelayReverse(void* digital_port_pointer, bool on, int32_t *status);
	bool getRelayForward(void* digital_port_pointer, int32_t *status);
	bool getRelayReverse(void* digital_port_pointer, int32_t *status);

	bool allocateDIO(void* digital_port_pointer, bool input, int32_t *status);
	void freeDIO(void* digital_port_pointer, int32_t *status);
	void setDIO(void* digital_port_pointer, short value, int32_t *status);
	bool getDIO(void* digital_port_pointer, int32_t *status);
	bool getDIODirection(void* digital_port_pointer, int32_t *status);
	void pulse(void* digital_port_pointer, double pulseLength, int32_t *status);
	bool isPulsing(void* digital_port_pointer, int32_t *status);
	bool isAnyPulsing(int32_t *status);

	void* initializeCounter(Mode mode, uint32_t *index, int32_t *status);
	void freeCounter(void* counter_pointer, int32_t *status);
	void setCounterAverageSize(void* counter_pointer, int32_t size, int32_t *status);
	void setCounterUpSource(void* counter_pointer, uint32_t pin, bool analogTrigger, int32_t *status);
	void setCounterUpSourceEdge(void* counter_pointer, bool risingEdge, bool fallingEdge,
			int32_t *status);
	void clearCounterUpSource(void* counter_pointer, int32_t *status);
	void setCounterDownSource(void* counter_pointer, uint32_t pin, bool analogTrigger, int32_t *status);
	void setCounterDownSourceEdge(void* counter_pointer, bool risingEdge, bool fallingEdge,
			int32_t *status);
	void clearCounterDownSource(void* counter_pointer, int32_t *status);
	void setCounterUpDownMode(void* counter_pointer, int32_t *status);
	void setCounterExternalDirectionMode(void* counter_pointer, int32_t *status);
	void setCounterSemiPeriodMode(void* counter_pointer, bool highSemiPeriod, int32_t *status);
	void setCounterPulseLengthMode(void* counter_pointer, double threshold, int32_t *status);
	int32_t getCounterSamplesToAverage(void* counter_pointer, int32_t *status);
	void setCounterSamplesToAverage(void* counter_pointer, int samplesToAverage, int32_t *status);
	void resetCounter(void* counter_pointer, int32_t *status);
	int32_t getCounter(void* counter_pointer, int32_t *status);
	double getCounterPeriod(void* counter_pointer, int32_t *status);
	void setCounterMaxPeriod(void* counter_pointer, double maxPeriod, int32_t *status);
	void setCounterUpdateWhenEmpty(void* counter_pointer, bool enabled, int32_t *status);
	bool getCounterStopped(void* counter_pointer, int32_t *status);
	bool getCounterDirection(void* counter_pointer, int32_t *status);
	void setCounterReverseDirection(void* counter_pointer, bool reverseDirection, int32_t *status);

	void* initializeEncoder(uint8_t port_a_module, uint32_t port_a_pin, bool port_a_analog_trigger,
			uint8_t port_b_module, uint32_t port_b_pin, bool port_b_analog_trigger,
			bool reverseDirection, int32_t *index, int32_t *status); // TODO: fix routing
	void freeEncoder(void* encoder_pointer, int32_t *status);
	void resetEncoder(void* encoder_pointer, int32_t *status);
	int32_t getEncoder(void* encoder_pointer, int32_t *status); // Raw value
	double getEncoderPeriod(void* encoder_pointer, int32_t *status);
	void setEncoderMaxPeriod(void* encoder_pointer, double maxPeriod, int32_t *status);
	bool getEncoderStopped(void* encoder_pointer, int32_t *status);
	bool getEncoderDirection(void* encoder_pointer, int32_t *status);
	void setEncoderReverseDirection(void* encoder_pointer, bool reverseDirection, int32_t *status);
	void setEncoderSamplesToAverage(void* encoder_pointer, uint32_t samplesToAverage,
			int32_t *status);
	uint32_t getEncoderSamplesToAverage(void* encoder_pointer, int32_t *status);
	void setEncoderIndexSource(void *encoder_pointer, uint32_t pin, bool analogTrigger, bool activeHigh,
			bool edgeSensitive, int32_t *status);

	uint16_t getLoopTiming(int32_t *status);

	void spiInitialize(uint8_t port, int32_t *status);
	int32_t spiTransaction(uint8_t port, uint8_t *dataToSend, uint8_t *dataReceived, uint8_t size);
	int32_t spiWrite(uint8_t port, uint8_t* dataToSend, uint8_t sendSize);
	int32_t spiRead(uint8_t port, uint8_t *buffer, uint8_t count);
	void spiClose(uint8_t port);
	void spiSetSpeed(uint8_t port, uint32_t speed);
	void spiSetBitsPerWord(uint8_t port, uint8_t bpw);
	void spiSetOpts(uint8_t port, int msb_first, int sample_on_trailing, int clk_idle_high);
	void spiSetChipSelectActiveHigh(uint8_t port, int32_t *status);
	void spiSetChipSelectActiveLow(uint8_t port, int32_t *status);
	int32_t spiGetHandle(uint8_t port);
	void spiSetHandle(uint8_t port, int32_t handle);
	MUTEX_ID spiGetSemaphore(uint8_t port);
	void spiSetSemaphore(uint8_t port, MUTEX_ID semaphore);

	void i2CInitialize(uint8_t port, int32_t *status);
	int32_t i2CTransaction(uint8_t port, uint8_t deviceAddress, uint8_t *dataToSend, uint8_t sendSize, uint8_t *dataReceived, uint8_t receiveSize);
	int32_t i2CWrite(uint8_t port, uint8_t deviceAddress, uint8_t *dataToSend, uint8_t sendSize);
	int32_t i2CRead(uint8_t port, uint8_t deviceAddress, uint8_t *buffer, uint8_t count);
	void i2CClose(uint8_t port);

	//// Float JNA Hack
	// double
	void setPWMRateIntHack(int rate, int32_t *status);
	void setPWMDutyCycleIntHack(void* pwmGenerator, int32_t dutyCycle, int32_t *status);
}
