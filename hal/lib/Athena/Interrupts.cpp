#include "HAL/Interrupts.hpp"
#include "ChipObject.h"

struct Interrupt // FIXME: why is this internal?
{
	tInterrupt *anInterrupt;
	tInterruptManager *manager;
};

void* initializeInterrupts(uint32_t interruptIndex, bool watcher, int32_t *status)
{
	Interrupt* anInterrupt = new Interrupt();
	// Expects the calling leaf class to allocate an interrupt index.
	anInterrupt->anInterrupt = tInterrupt::create(interruptIndex, status);
	anInterrupt->anInterrupt->writeConfig_WaitForAck(false, status);
	anInterrupt->manager = new tInterruptManager(1 << interruptIndex, watcher, status);
	return anInterrupt;
}

void cleanInterrupts(void* interrupt_pointer, int32_t *status)
{
	Interrupt* anInterrupt = (Interrupt*)interrupt_pointer;
	delete anInterrupt->anInterrupt;
	delete anInterrupt->manager;
	anInterrupt->anInterrupt = NULL;
	anInterrupt->manager = NULL;
}

/**
 * In synchronous mode, wait for the defined interrupt to occur.
 * @param timeout Timeout in seconds
 */
void waitForInterrupt(void* interrupt_pointer, double timeout, int32_t *status)
{
	Interrupt* anInterrupt = (Interrupt*)interrupt_pointer;
	anInterrupt->manager->watch((int32_t)(timeout * 1e3), status);
}

/**
 * Enable interrupts to occur on this input.
 * oInterrupts are disabled when the RequestInterrupt call is made. This gives time to do the
 * setup of the other options before starting to field interrupts.
 */
void enableInterrupts(void* interrupt_pointer, int32_t *status)
{
	Interrupt* anInterrupt = (Interrupt*)interrupt_pointer;
	anInterrupt->manager->enable(status);
}

/**
 * Disable Interrupts without without deallocating structures.
 */
void disableInterrupts(void* interrupt_pointer, int32_t *status)
{

	Interrupt* anInterrupt = (Interrupt*)interrupt_pointer;
	anInterrupt->manager->disable(status);

}

/**
 * Return the timestamp for the interrupt that occurred most recently.
 * This is in the same time domain as GetClock().
 * @return Timestamp in seconds since boot.
 */
double readInterruptTimestamp(void* interrupt_pointer, int32_t *status)
{
	Interrupt* anInterrupt = (Interrupt*)interrupt_pointer;
	uint32_t timestamp = anInterrupt->anInterrupt->readTimeStamp(status);
	return timestamp * 1e-6;
}

void requestInterrupts(void* interrupt_pointer, uint8_t routing_module, uint32_t routing_pin,
		bool routing_analog_trigger, int32_t *status)
{
	Interrupt* anInterrupt = (Interrupt*)interrupt_pointer;
	anInterrupt->anInterrupt->writeConfig_WaitForAck(false, status);
	anInterrupt->anInterrupt->writeConfig_Source_AnalogTrigger(routing_analog_trigger, status);
	anInterrupt->anInterrupt->writeConfig_Source_Channel(routing_pin, status);
	anInterrupt->anInterrupt->writeConfig_Source_Module(routing_module, status);
}

void attachInterruptHandler(void* interrupt_pointer, InterruptHandlerFunction handler, void* param,
		int32_t *status)
{
	Interrupt* anInterrupt = (Interrupt*)interrupt_pointer;
	anInterrupt->manager->registerHandler(handler, param, status);
}

void setInterruptUpSourceEdge(void* interrupt_pointer, bool risingEdge, bool fallingEdge,
		int32_t *status)
{
	Interrupt* anInterrupt = (Interrupt*)interrupt_pointer;
	anInterrupt->anInterrupt->writeConfig_RisingEdge(risingEdge, status);
	anInterrupt->anInterrupt->writeConfig_FallingEdge(fallingEdge, status);
}
