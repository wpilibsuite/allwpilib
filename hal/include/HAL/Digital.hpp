#pragma once
#ifdef __vxworks
#include <vxWorks.h>
#else
#include <stdint.h>
#endif

enum Mode
{
	kTwoPulse = 0,
	kSemiperiod = 1,
	kPulseLength = 2,
	kExternalDirection = 3
};
enum tSPIConstants
{
	kReceiveFIFODepth = 512,
	kTransmitFIFODepth = 512
};
enum tFrameMode
{
	kChipSelect,
	kPreLatchPulse,
	kPostLatchPulse,
	kPreAndPostLatchPulse
};
extern "C"
{
	void* initializeDigitalPort(void* port_pointer, int32_t *status);
	bool checkDigitalModule(uint8_t module);
	bool checkPWMChannel(void* digital_port_pointer);
	bool checkRelayChannel(void* digital_port_pointer);

	void setPWM(void* digital_port_pointer, unsigned short value, int32_t *status);
	unsigned short getPWM(void* digital_port_pointer, int32_t *status);
	void setPWMPeriodScale(void* digital_port_pointer, uint32_t squelchMask, int32_t *status);
	void* allocatePWM(int32_t *status);
	void* allocatePWMWithModule(uint8_t module, int32_t *status);
	void freePWM(void* pwmGenerator, int32_t *status);
	void freePWMWithModule(uint8_t module, void* pwmGenerator, int32_t *status);
	void setPWMRate(double rate, int32_t *status);
	void setPWMRateWithModule(uint8_t module, double rate, int32_t *status);
	void setPWMDutyCycle(void* pwmGenerator, double dutyCycle, int32_t *status);
	void setPWMDutyCycleWithModule(uint8_t module, void* pwmGenerator, double dutyCycle,
			int32_t *status);
	void setPWMOutputChannel(void* pwmGenerator, uint32_t pin, int32_t *status);
	void setPWMOutputChannelWithModule(uint8_t module, void* pwmGenerator, uint32_t pin,
			int32_t *status);

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
	bool isAnyPulsingWithModule(uint8_t module, int32_t *status);

	void* initializeCounter(Mode mode, uint32_t *index, int32_t *status);
	void freeCounter(void* counter_pointer, int32_t *status);
	void setCounterAverageSize(void* counter_pointer, int32_t size, int32_t *status);
	void setCounterUpSourceWithModule(void* counter_pointer, uint8_t module, uint32_t pin,
			bool analogTrigger, int32_t *status); // TODO: Without Module
	void setCounterUpSourceEdge(void* counter_pointer, bool risingEdge, bool fallingEdge,
			int32_t *status);
	void clearCounterUpSource(void* counter_pointer, int32_t *status);
	void setCounterDownSourceWithModule(void* counter_pointer, uint8_t module, uint32_t pin,
			bool analogTrigger, int32_t *status); // TODO: Without Module
	void setCounterDownSourceEdge(void* counter_pointer, bool risingEdge, bool fallingEdge,
			int32_t *status);
	void clearCounterDownSource(void* counter_pointer, int32_t *status);
	void setCounterUpDownMode(void* counter_pointer, int32_t *status);
	void setCounterExternalDirectionMode(void* counter_pointer, int32_t *status);
	void setCounterSemiPeriodMode(void* counter_pointer, bool highSemiPeriod, int32_t *status);
	void setCounterPulseLengthMode(void* counter_pointer, double threshold, int32_t *status);
	int32_t getCounterSamplesToAverage(void* counter_pointer, int32_t *status);
	void setCounterSamplesToAverage(void* counter_pointer, int samplesToAverage, int32_t *status);
	void startCounter(void* counter_pointer, int32_t *status);
	void stopCounter(void* counter_pointer, int32_t *status);
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
	void startEncoder(void* encoder_pointer, int32_t *status);
	void stopEncoder(void* encoder_pointer, int32_t *status);
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

	uint16_t getLoopTiming(int32_t *status);
	uint16_t getLoopTimingWithModule(uint8_t module, int32_t *status);

	void* initializeSPI(uint8_t sclk_routing_module, uint32_t sclk_routing_pin,
			uint8_t mosi_routing_module, uint32_t mosi_routing_pin, uint8_t miso_routing_module,
			uint32_t miso_routing_pin, int32_t *status);
	void cleanSPI(void* spi_pointer, int32_t *status);
	void setSPIBitsPerWord(void* spi_pointer, uint32_t bits, int32_t *status);
	uint32_t getSPIBitsPerWord(void* spi_pointer, int32_t *status);
	void setSPIClockRate(void* spi_pointer, double hz, int32_t *status);
	void setSPIMSBFirst(void* spi_pointer, int32_t *status);
	void setSPILSBFirst(void* spi_pointer, int32_t *status);
	void setSPISampleDataOnFalling(void* spi_pointer, int32_t *status);
	void setSPISampleDataOnRising(void* spi_pointer, int32_t *status);
	void setSPISlaveSelect(void* spi_pointer, uint8_t ss_routing_module, uint32_t ss_routing_pin,
			int32_t *status);
	void setSPILatchMode(void* spi_pointer, tFrameMode mode, int32_t *status);
	tFrameMode getSPILatchMode(void* spi_pointer, int32_t *status);
	void setSPIFramePolarity(void* spi_pointer, bool activeLow, int32_t *status);
	bool getSPIFramePolarity(void* spi_pointer, int32_t *status);
	void setSPIClockActiveLow(void* spi_pointer, int32_t *status);
	void setSPIClockActiveHigh(void* spi_pointer, int32_t *status);
	void applySPIConfig(void* spi_pointer, int32_t *status);
	uint16_t getSPIOutputFIFOAvailable(void* spi_pointer, int32_t *status);
	uint16_t getSPINumReceived(void* spi_pointer, int32_t *status);
	bool isSPIDone(void* spi_pointer, int32_t *status);
	bool hadSPIReceiveOverflow(void* spi_pointer, int32_t *status);
	void writeSPI(void* spi_pointer, uint32_t data, int32_t *status);
	uint32_t readSPI(void* spi_pointer, bool initiate, int32_t *status);
	void resetSPI(void* spi_pointer, int32_t *status);
	void clearSPIReceivedData(void* spi_pointer, int32_t *status);

	void i2CInitialize(uint8_t port, int32_t *status);
	int i2CTransaction(uint8_t port, uint8_t deviceAddress, uint8_t *dataToSend, uint8_t sendSize, uint8_t *dataReceived, uint8_t receiveSize);
	int i2CWrite(uint8_t port, uint8_t deviceAddress, uint8_t *dataToSend, uint8_t sendSize);
	int i2CRead(uint8_t port, uint8_t deviceAddress, uint8_t *buffer, uint8_t count);
	void i2CClose(uint8_t port);

	//// Float JNA Hack
	// double
	void setPWMRateIntHack(int rate, int32_t *status);
	void setPWMRateWithModuleIntHack(uint8_t module, int32_t rate, int32_t *status);
	void setPWMDutyCycleIntHack(void* pwmGenerator, int32_t dutyCycle, int32_t *status);
	void setPWMDutyCycleWithModuleIntHack(uint8_t module, void* pwmGenerator, int32_t dutyCycle,
			int32_t *status);
}
