
#ifdef __vxworks
#include <vxWorks.h>
#else
#include <stdint.h>
#endif

#ifndef HAL_INTERRUPTS_H
#define HAL_INTERRUPTS_H

extern "C" {
  typedef void (*InterruptHandlerFunction)(uint32_t interruptAssertedMask, void *param);
  
  void* initializeInterrupts(uint32_t interruptIndex, bool watcher, int32_t *status);
  void cleanInterrupts(void* interrupt_pointer, int32_t *status);
  
  void waitForInterrupt(void* interrupt_pointer, double timeout, int32_t *status);
  void enableInterrupts(void* interrupt_pointer, int32_t *status);
  void disableInterrupts(void* interrupt_pointer, int32_t *status);
  double readInterruptTimestamp(void* interrupt_pointer, int32_t *status);
  void requestInterrupts(void* interrupt_pointer, uint8_t routing_module, uint32_t routing_pin,
						 bool routing_analog_trigger, int32_t *status);
  void attachInterruptHandler(void* interrupt_pointer, InterruptHandlerFunction handler,
							  void* param, int32_t *status);
  void setInterruptUpSourceEdge(void* interrupt_pointer, bool risingEdge, bool fallingEdge, int32_t *status);
}
#endif
