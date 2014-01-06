
#include "HAL/Digital.h"

#include "Port.h"
#include "HAL/HAL.h"
#include "ChipObject.h"
#include "HAL/cpp/Synchronized.h"
#include "HAL/cpp/Resource.h"
#include "NetworkCommunication/LoadOut.h"
#include <stdio.h>
#include <math.h>

static const uint32_t kExpectedLoopTiming = 40;
static const uint32_t kDigitalPins = 14;
static const uint32_t kPwmPins = 10;
static const uint32_t kRelayPins = 8;
/**
 * kDefaultPwmPeriod is in ms
 *
 * - 20ms periods (50 Hz) are the "safest" setting in that this works for all devices
 * - 20ms periods seem to be desirable for Vex Motors
 * - 20ms periods are the specified period for HS-322HD servos, but work reliably down
 *      to 10.0 ms; starting at about 8.5ms, the servo sometimes hums and get hot;
 *      by 5.0ms the hum is nearly continuous
 * - 10ms periods work well for Victor 884
 * - 5ms periods allows higher update rates for Luminary Micro Jaguar speed controllers.
 *      Due to the shipping firmware on the Jaguar, we can't run the update period less
 *      than 5.05 ms.
 *
 * kDefaultPwmPeriod is the 1x period (5.05 ms).  In hardware, the period scaling is implemented as an
 * output squelch to get longer periods for old devices.
 */
static const float kDefaultPwmPeriod = 5.05;
/**
 * kDefaultPwmCenter is the PWM range center in ms
 */
static const float kDefaultPwmCenter = 1.5;
/**
 * kDefaultPWMStepsDown is the number of PWM steps below the centerpoint
 */
static const int32_t kDefaultPwmStepsDown = 1000;
static const int32_t kPwmDisabled = 0;

struct digital_port_t {
  Port port;
  uint32_t PWMGeneratorID;
};
typedef struct digital_port_t DigitalPort;

// XXX: Set these back to static once we figure out the memory clobbering issue
MUTEX_ID digitalDIOSemaphore = NULL;
MUTEX_ID digitalRelaySemaphore = NULL;
MUTEX_ID digitalPwmSemaphore = NULL;
MUTEX_ID digitalI2CSemaphore = NULL;
tDIO* digitalSystem = NULL;
tRelay* relaySystem = NULL;
tPWM* pwmSystem = NULL;
Resource *DIOChannels = NULL;
Resource *DO_PWMGenerators = NULL;

bool digitalSystemsInitialized = false;

/**
 * Initialize the digital modules.
 */
void initializeDigital(int32_t *status) {
  if (digitalSystemsInitialized) return;

  // Create a semaphore to protect changes to the digital output values
  digitalDIOSemaphore = initializeMutexRecursive();

  // Create a semaphore to protect changes to the relay values
  digitalRelaySemaphore = initializeMutexRecursive();

  // Create a semaphore to protect changes to the DO PWM config
  digitalPwmSemaphore = initializeMutexRecursive();

  digitalI2CSemaphore = initializeMutexRecursive();
  
  Resource::CreateResourceObject(&DIOChannels, tDIO::kNumSystems * kDigitalPins);
  Resource::CreateResourceObject(&DO_PWMGenerators, tDIO::kNumPWMDutyCycleElements);
  digitalSystem = tDIO::create(status);

  // Relay Setup
  relaySystem = tRelay::create(status);
    
  // Turn off all relay outputs.
  relaySystem->writeValue_Forward(0, status);
  relaySystem->writeValue_Reverse(0, status);

  // PWM Setup
  pwmSystem = tPWM::create(status);
      
  // Make sure that the 9403 IONode has had a chance to initialize before continuing.
  while(pwmSystem->readLoopTiming(status) == 0) delayTicks(1);

  if (pwmSystem->readLoopTiming(status) != kExpectedLoopTiming) {
	// TODO: char err[128];
	// TODO: sprintf(err, "DIO LoopTiming: %d, expecting: %lu\n", digitalModules[port->module-1]->readLoopTiming(status), kExpectedLoopTiming);
	*status = LOOP_TIMING_ERROR; // NOTE: Doesn't display the error
  }

  //Calculate the length, in ms, of one DIO loop
  double loopTime = pwmSystem->readLoopTiming(status)/(kSystemClockTicksPerMicrosecond*1e3);
    
  pwmSystem->writeConfig_Period((uint16_t) (kDefaultPwmPeriod/loopTime + .5), status);
  uint16_t minHigh = (uint16_t) ((kDefaultPwmCenter-kDefaultPwmStepsDown*loopTime)/loopTime + .5);
  pwmSystem->writeConfig_MinHigh(minHigh, status);
//  printf("MinHigh: %d\n", minHigh);
  // Ensure that PWM output values are set to OFF
  for (uint32_t pwm_index = 1; pwm_index <= kPwmPins; pwm_index++) {
	// Initialize port structure
	DigitalPort* digital_port = new DigitalPort();
	digital_port->port.pin = pwm_index;
      
	setPWM(digital_port, kPwmDisabled, status);
	setPWMPeriodScale(digital_port, 3, status); // Set all to 4x by default.
  }
  
  digitalSystemsInitialized = true;
}

/**
 * Create a new instance of an digital module.
 * Create an instance of the digital module object. Initialize all the parameters
 * to reasonable values on start.
 * Setting a global value on an digital module can be done only once unless subsequent
 * values are set the previously set value.
 * Digital modules are a singleton, so the constructor is never called outside of this class.
 *
 * @param moduleNumber The digital module to create (1 or 2).
 */
void* initializeDigitalPort(void* port_pointer, int32_t *status) {
  initializeDigital(status);
  Port* port = (Port*) port_pointer;

  // Initialize port structure
  DigitalPort* digital_port = new DigitalPort();
  digital_port->port = *port;

  return digital_port;
}

bool checkDigitalModule(uint8_t module) {
  if (nLoadOut::getModulePresence(nLoadOut::kModuleType_Digital, module - 1))
    return true;
  return false;
}

bool checkPWMChannel(void* digital_port_pointer) {
  DigitalPort* port = (DigitalPort*) digital_port_pointer;
  if (port->port.pin > 0 && port->port.pin <= kPwmPins)
    return true;
  return false;
}

bool checkRelayChannel(void* digital_port_pointer) {
  DigitalPort* port = (DigitalPort*) digital_port_pointer;
  if (port->port.pin > 0 && port->port.pin <= kRelayPins)
    return true;
  return false;
}

uint8_t remapDigitalChannel(uint32_t pin, int32_t *status) {
  return pin; // TODO: No mapping needed anymore
}

uint8_t unmapDigitalChannel(uint32_t pin, int32_t *status) {
  return pin; // TODO: No mapping needed anymore
}

/**
 * Set a PWM channel to the desired value. The values range from 0 to 255 and the period is controlled
 * by the PWM Period and MinHigh registers.
 * 
 * @param channel The PWM channel to set.
 * @param value The PWM value to set.
 */  
void setPWM(void* digital_port_pointer, unsigned short value, int32_t *status) {
  DigitalPort* port = (DigitalPort*) digital_port_pointer;
  checkPWMChannel(port);
//  printf("Value:%d\n", value);
  pwmSystem->writeHdr(port->port.pin - 1, value, status); // XXX: Support MXP
}

/**
 * Get a value from a PWM channel. The values range from 0 to 255.
 * 
 * @param channel The PWM channel to read from.
 * @return The raw PWM value.
 */
unsigned short getPWM(void* digital_port_pointer, int32_t *status) {
  DigitalPort* port = (DigitalPort*) digital_port_pointer;
  checkPWMChannel(port);
  return pwmSystem->readHdr(port->port.pin - 1, status); // XXX: Support MXP
}

/**
 * Set how how often the PWM signal is squelched, thus scaling the period.
 * 
 * @param channel The PWM channel to configure.
 * @param squelchMask The 2-bit mask of outputs to squelch.
 */
void setPWMPeriodScale(void* digital_port_pointer, uint32_t squelchMask, int32_t *status) {
  DigitalPort* port = (DigitalPort*) digital_port_pointer;
  checkPWMChannel(port);
  pwmSystem->writePeriodScaleHdr(port->port.pin - 1, squelchMask, status); // XXX: Support MXP
}

/**
 * Allocate a DO PWM Generator.
 * Allocate PWM generators so that they are not accidently reused.
 * 
 * @return PWM Generator refnum
 */
void* allocatePWM(int32_t *status) {
  return allocatePWMWithModule(0, status);
}

/**
 * Allocate a DO PWM Generator.
 * Allocate PWM generators so that they are not accidently reused.
 * 
 * @return PWM Generator refnum
 */
void* allocatePWMWithModule(uint8_t module, int32_t *status) {
  char buf[64];
  snprintf(buf, 64, "DO_PWM (Module: %d)", module);
  uint32_t* val = NULL;
  *val = DO_PWMGenerators->Allocate(buf);
  return val;
}

/**
 * Free the resource associated with a DO PWM generator.
 * 
 * @param pwmGenerator The pwmGen to free that was allocated with AllocateDO_PWM()
 */
void freePWM(void* pwmGenerator, int32_t *status) {
  freePWMWithModule(0, pwmGenerator, status);
}

/**
 * Free the resource associated with a DO PWM generator.
 * 
 * @param pwmGenerator The pwmGen to free that was allocated with AllocateDO_PWM()
 */
void freePWMWithModule(uint8_t module, void* pwmGenerator, int32_t *status) {
  uint32_t id = *((uint32_t*) pwmGenerator);
  if (id == ~0ul) return;
  DO_PWMGenerators->Free(id);
}

/**
 * Change the frequency of the DO PWM generator.
 * 
 * The valid range is from 0.6 Hz to 19 kHz.  The frequency resolution is logarithmic.
 * 
 * @param rate The frequency to output all digital output PWM signals on this module.
 */
void setPWMRate(double rate, int32_t *status) {
  setPWMRateWithModule(0, rate, status);
}

/**
 * Change the frequency of the DO PWM generator.
 * 
 * The valid range is from 0.6 Hz to 19 kHz.  The frequency resolution is logarithmic.
 * 
 * @param rate The frequency to output all digital output PWM signals on this module.
 */
void setPWMRateWithModule(uint8_t module, double rate, int32_t *status) {
  // Currently rounding in the log rate domain... heavy weight toward picking a higher freq.
  // TODO: Round in the linear rate domain.
  uint8_t pwmPeriodPower = (uint8_t)(log(1.0 / (pwmSystem->readLoopTiming(status) * 0.25E-6 * rate))/log(2.0) + 0.5);
  digitalSystem->writePWMConfig_PeriodPower(pwmPeriodPower, status);
}

/**
 * Configure the duty-cycle of the PWM generator
 * 
 * @param pwmGenerator The generator index reserved by AllocateDO_PWM()
 * @param dutyCycle The percent duty cycle to output [0..1].
 */
void setPWMDutyCycle(void* pwmGenerator, double dutyCycle, int32_t *status) {
  setPWMDutyCycleWithModule(0, pwmGenerator, dutyCycle, status);
}

/**
 * Configure the duty-cycle of the PWM generator
 * 
 * @param pwmGenerator The generator index reserved by AllocateDO_PWM()
 * @param dutyCycle The percent duty cycle to output [0..1].
 */
void setPWMDutyCycleWithModule(uint8_t module, void* pwmGenerator, double dutyCycle, int32_t *status) {
  uint32_t id = *((uint32_t*) pwmGenerator);
  if (id == ~0ul) return;
  if (dutyCycle > 1.0) dutyCycle = 1.0;
  if (dutyCycle < 0.0) dutyCycle = 0.0;
  float rawDutyCycle = 256.0 * dutyCycle;
  if (rawDutyCycle > 255.5) rawDutyCycle = 255.5;
  {
    Synchronized sync(digitalPwmSemaphore);
    uint8_t pwmPeriodPower = digitalSystem->readPWMConfig_PeriodPower(status);
    if (pwmPeriodPower < 4) {
	  // The resolution of the duty cycle drops close to the highest frequencies.
	  rawDutyCycle = rawDutyCycle / pow(2.0, 4 - pwmPeriodPower);
	}
    digitalSystem->writePWMDutyCycle(id, (uint8_t)rawDutyCycle, status);
  }
}

/**
 * Configure which DO channel the PWM siganl is output on
 * 
 * @param pwmGenerator The generator index reserved by AllocateDO_PWM()
 * @param channel The Digital Output channel to output on
 */
void setPWMOutputChannel(void* pwmGenerator, uint32_t pin, int32_t *status) {
  setPWMOutputChannelWithModule(0, pwmGenerator, pin, status);
}

/**
 * Configure which DO channel the PWM siganl is output on
 * 
 * @param pwmGenerator The generator index reserved by AllocateDO_PWM()
 * @param channel The Digital Output channel to output on
 */
void setPWMOutputChannelWithModule(uint8_t module, void* pwmGenerator, uint32_t pin, int32_t *status) {
  uint32_t id = *((uint32_t*) pwmGenerator);
  if (id == ~0ul) return;
  switch(id) {
  case 0:
    digitalSystem->writePWMConfig_OutputSelect_0(remapDigitalChannel(pin - 1, status), status);
    break;
  case 1:
    digitalSystem->writePWMConfig_OutputSelect_1(remapDigitalChannel(pin - 1, status), status);
    break;
  case 2:
    digitalSystem->writePWMConfig_OutputSelect_2(remapDigitalChannel(pin - 1, status), status);
    break;
  case 3:
    digitalSystem->writePWMConfig_OutputSelect_3(remapDigitalChannel(pin - 1, status), status);
    break;
  }
}

/**
 * Set the state of a relay.
 * Set the state of a relay output to be forward. Relays have two outputs and each is
 * independently set to 0v or 12v.
 */
void setRelayForward(void* digital_port_pointer, bool on, int32_t *status) {
  DigitalPort* port = (DigitalPort*) digital_port_pointer;
  checkRelayChannel(port);
  {
    Synchronized sync(digitalRelaySemaphore);
    uint8_t forwardRelays = relaySystem->readValue_Forward(status);
    if (on)
      forwardRelays |= 1 << (port->port.pin - 1);
    else
      forwardRelays &= ~(1 << (port->port.pin - 1));
    relaySystem->writeValue_Forward(forwardRelays, status);
  }
}

/**
 * Set the state of a relay.
 * Set the state of a relay output to be reverse. Relays have two outputs and each is
 * independently set to 0v or 12v.
 */
void setRelayReverse(void* digital_port_pointer, bool on, int32_t *status) {
  DigitalPort* port = (DigitalPort*) digital_port_pointer;
  checkRelayChannel(port);
  {
    Synchronized sync(digitalRelaySemaphore);
    uint8_t reverseRelays = relaySystem->readValue_Reverse(status);
    if (on)
      reverseRelays |= 1 << (port->port.pin - 1);
    else
      reverseRelays &= ~(1 << (port->port.pin - 1));
    relaySystem->writeValue_Reverse(reverseRelays, status);
  }
}

/**
 * Get the current state of the forward relay channel
 */
bool getRelayForward(void* digital_port_pointer, int32_t *status) {
  DigitalPort* port = (DigitalPort*) digital_port_pointer;
  uint8_t forwardRelays = relaySystem->readValue_Forward(status);
  return (forwardRelays & (1 << (port->port.pin - 1))) != 0;
}

/**
 * Get the current state of the reverse relay channel
 */
bool getRelayReverse(void* digital_port_pointer, int32_t *status) {
  DigitalPort* port = (DigitalPort*) digital_port_pointer;
  uint8_t reverseRelays = relaySystem->readValue_Reverse(status);
  return (reverseRelays & (1 << (port->port.pin - 1))) != 0;
}

/**
 * Allocate Digital I/O channels.
 * Allocate channels so that they are not accidently reused. Also the direction is set at the
 * time of the allocation.
 * 
 * @param channel The Digital I/O channel
 * @param input If true open as input; if false open as output
 * @return Was successfully allocated
 */
bool allocateDIO(void* digital_port_pointer, bool input, int32_t *status) {
  DigitalPort* port = (DigitalPort*) digital_port_pointer;
  char buf[64];
  snprintf(buf, 64, "DIO %d (Module %d)", port->port.pin, port->port.module);
  if (DIOChannels->Allocate(kDigitalPins * (port->port.module - 1) + port->port.pin - 1, buf) == ~0ul) return false;
  {
    Synchronized sync(digitalDIOSemaphore);
    uint32_t bitToSet = 1 << (remapDigitalChannel(port->port.pin - 1, status));
    tDIO::tOutputEnable outputEnable = digitalSystem->readOutputEnable(status);
    if (input) {
      outputEnable.Headers = outputEnable.Headers & (~bitToSet); // clear the bit for read
    } else {
      outputEnable.Headers = outputEnable.Headers | bitToSet; // set the bit for write
    }
    digitalSystem->writeOutputEnable(outputEnable, status);
  }
  return true;
}

/**
 * Free the resource associated with a digital I/O channel.
 * 
 * @param channel The Digital I/O channel to free
 */
void freeDIO(void* digital_port_pointer, int32_t *status) {
  DigitalPort* port = (DigitalPort*) digital_port_pointer;
  DIOChannels->Free(kDigitalPins * (port->port.module - 1) + port->port.pin - 1);
}

/**
 * Write a digital I/O bit to the FPGA.
 * Set a single value on a digital I/O channel.
 * 
 * @param channel The Digital I/O channel
 * @param value The state to set the digital channel (if it is configured as an output)
 */
void setDIO(void* digital_port_pointer, short value, int32_t *status) {
  DigitalPort* port = (DigitalPort*) digital_port_pointer;
  if (value != 0 && value != 1) {
    if (value != 0)
      value = 1;
  }
  {
    Synchronized sync(digitalDIOSemaphore);
    tDIO::tDO currentDIO = digitalSystem->readDO(status);
    if(value == 0) {
      currentDIO.Headers = currentDIO.Headers & ~(1 << remapDigitalChannel(port->port.pin - 1, status));
    } else if (value == 1) {
      currentDIO.Headers = currentDIO.Headers | (1 << remapDigitalChannel(port->port.pin - 1, status));
    } 
    digitalSystem->writeDO(currentDIO, status);
  }
}

/**
 * Read a digital I/O bit from the FPGA.
 * Get a single value from a digital I/O channel.
 * 
 * @param channel The digital I/O channel
 * @return The state of the specified channel
 */
bool getDIO(void* digital_port_pointer, int32_t *status) {
  DigitalPort* port = (DigitalPort*) digital_port_pointer;
  tDIO::tDI currentDIO = digitalSystem->readDI(status);
  //Shift 00000001 over channel-1 places.
  //AND it against the currentDIO
  //if it == 0, then return false
  //else return true

  return ((currentDIO.Headers >> remapDigitalChannel(port->port.pin - 1, status)) & 1) != 0;
}

/**
 * Read the direction of a the Digital I/O lines
 * A 1 bit means output and a 0 bit means input.
 * 
 * @param channel The digital I/O channel
 * @return The direction of the specified channel
 */
bool getDIODirection(void* digital_port_pointer, int32_t *status) {
  DigitalPort* port = (DigitalPort*) digital_port_pointer;
  tDIO::tOutputEnable currentOutputEnable = digitalSystem->readOutputEnable(status);
	
  //Shift 00000001 over port->port.pin-1 places.
  //AND it against the currentOutputEnable
  //if it == 0, then return false
  //else return true
  return ((currentOutputEnable.Headers >> remapDigitalChannel(port->port.pin - 1, status)) & 1) != 0;
}

/**
 * Generate a single pulse.
 * Write a pulse to the specified digital output channel. There can only be a single pulse going at any time.
 * 
 * @param channel The Digital Output channel that the pulse should be output on
 * @param pulseLength The active length of the pulse (in seconds)
 */
void pulse(void* digital_port_pointer, double pulseLength, int32_t *status) {
  DigitalPort* port = (DigitalPort*) digital_port_pointer;
  tDIO::tPulse pulse;
  pulse.Headers = 1 << remapDigitalChannel(port->port.pin - 1, status);
  digitalSystem->writePulseLength((uint8_t)(1.0e9 * pulseLength / (pwmSystem->readLoopTiming(status) * 25)), status);
  digitalSystem->writePulse(pulse, status);
}

/**
 * Check a DIO line to see if it is currently generating a pulse.
 * 
 * @return A pulse is in progress
 */
bool isPulsing(void* digital_port_pointer, int32_t *status) {
  DigitalPort* port = (DigitalPort*) digital_port_pointer;
  uint16_t mask = 1 << remapDigitalChannel(port->port.pin - 1, status);
  tDIO::tPulse pulseRegister = digitalSystem->readPulse(status);
  return (pulseRegister.Headers & mask) != 0;
}

/**
 * Check if any DIO line is currently generating a pulse.
 * 
 * @return A pulse on some line is in progress
 */
bool isAnyPulsing(int32_t *status) {
  return isAnyPulsingWithModule(1, status) || isAnyPulsingWithModule(2, status);
}

/**
 * Check if any DIO line is currently generating a pulse.
 * 
 * @return A pulse on some line is in progress
 */
bool isAnyPulsingWithModule(uint8_t module, int32_t *status) {
  tDIO::tPulse pulseRegister = digitalSystem->readPulse(status);
  return pulseRegister.Headers != 0;
}



struct counter_t {
  tCounter* counter;
  uint32_t index;
};
typedef struct counter_t Counter;

static Resource *counters = NULL;

void* initializeCounter(Mode mode, uint32_t *index, int32_t *status) {
	Resource::CreateResourceObject(&counters, tCounter::kNumSystems);
	*index = counters->Allocate("Counter");
	if (*index == ~0ul) {
		*status = NO_AVAILABLE_RESOURCES;
		return NULL;
	}
	Counter* counter = new Counter();
	counter->counter = tCounter::create(*index, status);
	counter->counter->writeConfig_Mode(mode, status);
	counter->counter->writeTimerConfig_AverageSize(1, status);
	counter->index = *index;
	return counter;
}

void freeCounter(void* counter_pointer, int32_t *status) {
  if (counter_pointer != NULL) {
	  Counter* counter = (Counter*) counter_pointer;
	  delete counter->counter;
	  counters->Free(counter->index);
  } else {
	  *status = NULL_PARAMETER;
  }
}

void setCounterAverageSize(void* counter_pointer, int32_t size, int32_t *status) {
  Counter* counter = (Counter*) counter_pointer;
  counter->counter->writeTimerConfig_AverageSize(size, status);
}

/**
 * Set the source object that causes the counter to count up.
 * Set the up counting DigitalSource.
 */
void setCounterUpSourceWithModule(void* counter_pointer, uint8_t module, uint32_t pin,
								  bool analogTrigger, int32_t *status) {
  Counter* counter = (Counter*) counter_pointer;
  
  counter->counter->writeConfig_UpSource_Module(module, status);
  counter->counter->writeConfig_UpSource_Channel(pin, status);
  counter->counter->writeConfig_UpSource_AnalogTrigger(analogTrigger, status);
	
  if(counter->counter->readConfig_Mode(status) == kTwoPulse ||
	 counter->counter->readConfig_Mode(status) == kExternalDirection) {
	setCounterUpSourceEdge(counter_pointer, true, false, status);
  }
  counter->counter->strobeReset(status);
}

/**
 * Set the edge sensitivity on an up counting source.
 * Set the up source to either detect rising edges or falling edges.
 */
void setCounterUpSourceEdge(void* counter_pointer, bool risingEdge, bool fallingEdge, int32_t *status) {
  Counter* counter = (Counter*) counter_pointer;
  counter->counter->writeConfig_UpRisingEdge(risingEdge, status);
  counter->counter->writeConfig_UpFallingEdge(fallingEdge, status);
}

/**
 * Disable the up counting source to the counter.
 */
void clearCounterUpSource(void* counter_pointer, int32_t *status) {
  Counter* counter = (Counter*) counter_pointer;
  bool state = counter->counter->readConfig_Enable(status);
  counter->counter->writeConfig_Enable(false, status);
  counter->counter->writeConfig_UpFallingEdge(false, status);
  counter->counter->writeConfig_UpRisingEdge(false, status);
  // Index 0 of digital is always 0.
  counter->counter->writeConfig_UpSource_Channel(0, status);
  counter->counter->writeConfig_UpSource_AnalogTrigger(false, status);
  counter->counter->writeConfig_Enable(state, status);
}

/**
 * Set the source object that causes the counter to count down.
 * Set the down counting DigitalSource.
 */
void setCounterDownSourceWithModule(void* counter_pointer, uint8_t module, uint32_t pin,
									bool analogTrigger, int32_t *status) {
  Counter* counter = (Counter*) counter_pointer;
  unsigned char mode = counter->counter->readConfig_Mode(status);
  if (mode != kTwoPulse && mode != kExternalDirection) {
	// TODO: wpi_setWPIErrorWithContext(ParameterOutOfRange, "Counter only supports DownSource in TwoPulse and ExternalDirection modes.");
	*status = PARAMETER_OUT_OF_RANGE;
	return;
  }
  counter->counter->writeConfig_DownSource_Module(module, status);
  counter->counter->writeConfig_DownSource_Channel(pin, status);
  counter->counter->writeConfig_DownSource_AnalogTrigger(analogTrigger, status);
	
  setCounterDownSourceEdge(counter_pointer, true, false, status);
  counter->counter->strobeReset(status);
}

/**
 * Set the edge sensitivity on a down counting source.
 * Set the down source to either detect rising edges or falling edges.
 */
void setCounterDownSourceEdge(void* counter_pointer, bool risingEdge, bool fallingEdge, int32_t *status) {
  Counter* counter = (Counter*) counter_pointer;
  counter->counter->writeConfig_DownRisingEdge(risingEdge, status);
  counter->counter->writeConfig_DownFallingEdge(fallingEdge, status);
}

/**
 * Disable the down counting source to the counter.
 */
void clearCounterDownSource(void* counter_pointer, int32_t *status) {
  Counter* counter = (Counter*) counter_pointer;
  bool state = counter->counter->readConfig_Enable(status);
  counter->counter->writeConfig_Enable(false, status);
  counter->counter->writeConfig_DownFallingEdge(false, status);
  counter->counter->writeConfig_DownRisingEdge(false, status);
  // Index 0 of digital is always 0.
  counter->counter->writeConfig_DownSource_Channel(0, status);
  counter->counter->writeConfig_DownSource_AnalogTrigger(false, status);
  counter->counter->writeConfig_Enable(state, status);
}

/**
 * Set standard up / down counting mode on this counter.
 * Up and down counts are sourced independently from two inputs.
 */
void setCounterUpDownMode(void* counter_pointer, int32_t *status) {
  Counter* counter = (Counter*) counter_pointer;
  counter->counter->writeConfig_Mode(kTwoPulse, status);
}

/**
 * Set external direction mode on this counter.
 * Counts are sourced on the Up counter input.
 * The Down counter input represents the direction to count.
 */
void setCounterExternalDirectionMode(void* counter_pointer, int32_t *status) {
  Counter* counter = (Counter*) counter_pointer;
  counter->counter->writeConfig_Mode(kExternalDirection, status);
}

/**
 * Set Semi-period mode on this counter.
 * Counts up on both rising and falling edges. 
 */
void setCounterSemiPeriodMode(void* counter_pointer, bool highSemiPeriod, int32_t *status) {
  Counter* counter = (Counter*) counter_pointer;
  counter->counter->writeConfig_Mode(kSemiperiod, status);
  counter->counter->writeConfig_UpRisingEdge(highSemiPeriod, status);
  setCounterUpdateWhenEmpty(counter_pointer, false, status);
}

/**
 * Configure the counter to count in up or down based on the length of the input pulse.
 * This mode is most useful for direction sensitive gear tooth sensors.
 * @param threshold The pulse length beyond which the counter counts the opposite direction.  Units are seconds.
 */
void setCounterPulseLengthMode(void* counter_pointer, double threshold, int32_t *status) {
  Counter* counter = (Counter*) counter_pointer;
  counter->counter->writeConfig_Mode(kPulseLength, status);
  counter->counter->writeConfig_PulseLengthThreshold((uint32_t)(threshold * 1.0e6) * kSystemClockTicksPerMicrosecond, status);
}

/**
 * Get the Samples to Average which specifies the number of samples of the timer to 
 * average when calculating the period. Perform averaging to account for 
 * mechanical imperfections or as oversampling to increase resolution.
 * @return SamplesToAverage The number of samples being averaged (from 1 to 127)
 */
int32_t getCounterSamplesToAverage(void* counter_pointer, int32_t *status) {
  Counter* counter = (Counter*) counter_pointer;
  return counter->counter->readTimerConfig_AverageSize(status);
}

/**
 * Set the Samples to Average which specifies the number of samples of the timer to 
 * average when calculating the period. Perform averaging to account for 
 * mechanical imperfections or as oversampling to increase resolution.
 * @param samplesToAverage The number of samples to average from 1 to 127.
 */
void setCounterSamplesToAverage(void* counter_pointer, int samplesToAverage, int32_t *status) {
  Counter* counter = (Counter*) counter_pointer;
  if (samplesToAverage < 1 || samplesToAverage > 127) {
	*status = PARAMETER_OUT_OF_RANGE;
  }
  counter->counter->writeTimerConfig_AverageSize(samplesToAverage, status);
}

/**
 * Start the Counter counting.
 * This enables the counter and it starts accumulating counts from the associated
 * input channel. The counter value is not reset on starting, and still has the previous value.
 */
void startCounter(void* counter_pointer, int32_t *status) {
  Counter* counter = (Counter*) counter_pointer;
  counter->counter->writeConfig_Enable(1, status);
}

/**
 * Stop the Counter.
 * Stops the counting but doesn't effect the current value.
 */
void stopCounter(void* counter_pointer, int32_t *status) {
  Counter* counter = (Counter*) counter_pointer;
  counter->counter->writeConfig_Enable(0, status);
}

/**
 * Reset the Counter to zero.
 * Set the counter value to zero. This doesn't effect the running state of the counter, just sets
 * the current value to zero.
 */
void resetCounter(void* counter_pointer, int32_t *status) {
  Counter* counter = (Counter*) counter_pointer;
  counter->counter->strobeReset(status);
}

/**
 * Read the current counter value.
 * Read the value at this instant. It may still be running, so it reflects the current value. Next
 * time it is read, it might have a different value.
 */
int32_t getCounter(void* counter_pointer, int32_t *status) {
  Counter* counter = (Counter*) counter_pointer;
  int32_t value = counter->counter->readOutput_Value(status);
  return value;
}

/*
 * Get the Period of the most recent count.
 * Returns the time interval of the most recent count. This can be used for velocity calculations
 * to determine shaft speed.
 * @returns The period of the last two pulses in units of seconds.
 */
double getCounterPeriod(void* counter_pointer, int32_t *status) {
  Counter* counter = (Counter*) counter_pointer;
  tCounter::tTimerOutput output = counter->counter->readTimerOutput(status);
  double period;
  if (output.Stalled)	{
	// Return infinity
	double zero = 0.0;
	period = 1.0 / zero;
  } else {
	// output.Period is a fixed point number that counts by 2 (24 bits, 25 integer bits)
	period = (double)(output.Period << 1) / (double)output.Count;
  }
  return period * 1.0e-6;
}

/**
 * Set the maximum period where the device is still considered "moving".
 * Sets the maximum period where the device is considered moving. This value is used to determine
 * the "stopped" state of the counter using the GetStopped method.
 * @param maxPeriod The maximum period where the counted device is considered moving in
 * seconds.
 */
void setCounterMaxPeriod(void* counter_pointer, double maxPeriod, int32_t *status) {
  Counter* counter = (Counter*) counter_pointer;
  counter->counter->writeTimerConfig_StallPeriod((uint32_t)(maxPeriod * 1.0e6), status);
}

/**
 * Select whether you want to continue updating the event timer output when there are no samples captured.
 * The output of the event timer has a buffer of periods that are averaged and posted to
 * a register on the FPGA.  When the timer detects that the event source has stopped
 * (based on the MaxPeriod) the buffer of samples to be averaged is emptied.  If you
 * enable the update when empty, you will be notified of the stopped source and the event
 * time will report 0 samples.  If you disable update when empty, the most recent average
 * will remain on the output until a new sample is acquired.  You will never see 0 samples
 * output (except when there have been no events since an FPGA reset) and you will likely not
 * see the stopped bit become true (since it is updated at the end of an average and there are
 * no samples to average).
 */
void setCounterUpdateWhenEmpty(void* counter_pointer, bool enabled, int32_t *status) {
  Counter* counter = (Counter*) counter_pointer;
  counter->counter->writeTimerConfig_UpdateWhenEmpty(enabled, status);
}

/**
 * Determine if the clock is stopped.
 * Determine if the clocked input is stopped based on the MaxPeriod value set using the
 * SetMaxPeriod method. If the clock exceeds the MaxPeriod, then the device (and counter) are
 * assumed to be stopped and it returns true.
 * @return Returns true if the most recent counter period exceeds the MaxPeriod value set by
 * SetMaxPeriod.
 */
bool getCounterStopped(void* counter_pointer, int32_t *status) {
  Counter* counter = (Counter*) counter_pointer;
  return counter->counter->readTimerOutput_Stalled(status);
}

/**
 * The last direction the counter value changed.
 * @return The last direction the counter value changed.
 */
bool getCounterDirection(void* counter_pointer, int32_t *status) {
  Counter* counter = (Counter*) counter_pointer;
  bool value = counter->counter->readOutput_Direction(status);
  return value;
}

/**
 * Set the Counter to return reversed sensing on the direction.
 * This allows counters to change the direction they are counting in the case of 1X and 2X
 * quadrature encoding only. Any other counter mode isn't supported.
 * @param reverseDirection true if the value counted should be negated.
 */
void setCounterReverseDirection(void* counter_pointer, bool reverseDirection, int32_t *status) {
  Counter* counter = (Counter*) counter_pointer;
  if (counter->counter->readConfig_Mode(status) == kExternalDirection) {
	if (reverseDirection)
	  setCounterDownSourceEdge(counter_pointer, true, true, status);
	else
	  setCounterDownSourceEdge(counter_pointer, false, true, status);
  }
}

struct encoder_t {
  tEncoder* encoder;
  uint32_t index;
};
typedef struct encoder_t Encoder;

static const double DECODING_SCALING_FACTOR = 0.25;
static Resource *quadEncoders = NULL;

void* initializeEncoder(uint8_t port_a_module, uint32_t port_a_pin, bool port_a_analog_trigger,
						uint8_t port_b_module, uint32_t port_b_pin, bool port_b_analog_trigger,
						bool reverseDirection, int32_t *index, int32_t *status) {

  // Initialize encoder structure
  Encoder* encoder = new Encoder();
  
  Resource::CreateResourceObject(&quadEncoders, tEncoder::kNumSystems);
  encoder->index = quadEncoders->Allocate("4X Encoder");
  *index = encoder->index;
  // TODO: if (index == ~0ul) { CloneError(quadEncoders); return; }
  encoder->encoder = tEncoder::create(encoder->index, status);
  encoder->encoder->writeConfig_ASource_Module(port_a_module, status);
  encoder->encoder->writeConfig_ASource_Channel(port_a_pin, status);
  encoder->encoder->writeConfig_ASource_AnalogTrigger(port_a_analog_trigger, status);
  encoder->encoder->writeConfig_BSource_Module(port_b_module, status);
  encoder->encoder->writeConfig_BSource_Channel(port_b_pin, status);
  encoder->encoder->writeConfig_BSource_AnalogTrigger(port_b_analog_trigger, status);
  encoder->encoder->strobeReset(status);
  encoder->encoder->writeConfig_Reverse(reverseDirection, status);
  encoder->encoder->writeTimerConfig_AverageSize(4, status);

  return encoder;
}

void freeEncoder(void* encoder_pointer, int32_t *status) {
  Encoder* encoder = (Encoder*) encoder_pointer;
  quadEncoders->Free(encoder->index);
  delete encoder->encoder;
}

/**
 * Start the Encoder.
 * Starts counting pulses on the Encoder device.
 */
void startEncoder(void* encoder_pointer, int32_t *status) {
  Encoder* encoder = (Encoder*) encoder_pointer;
  encoder->encoder->writeConfig_Enable(1, status);
}

/**
 * Stops counting pulses on the Encoder device. The value is not changed.
 */
void stopEncoder(void* encoder_pointer, int32_t *status) {
  Encoder* encoder = (Encoder*) encoder_pointer;
  encoder->encoder->writeConfig_Enable(0, status);
}

/**
 * Reset the Encoder distance to zero.
 * Resets the current count to zero on the encoder.
 */
void resetEncoder(void* encoder_pointer, int32_t *status) {
  Encoder* encoder = (Encoder*) encoder_pointer;
  encoder->encoder->strobeReset(status);
}

/**
 * Gets the raw value from the encoder.
 * The raw value is the actual count unscaled by the 1x, 2x, or 4x scale
 * factor.
 * @return Current raw count from the encoder
 */
int32_t getEncoder(void* encoder_pointer, int32_t *status) {
  Encoder* encoder = (Encoder*) encoder_pointer;
  return encoder->encoder->readOutput_Value(status);
}

/**
 * Returns the period of the most recent pulse.
 * Returns the period of the most recent Encoder pulse in seconds.
 * This method compenstates for the decoding type.
 * 
 * @deprecated Use GetRate() in favor of this method.  This returns unscaled periods and GetRate() scales using value from SetDistancePerPulse().
 *
 * @return Period in seconds of the most recent pulse.
 */
double getEncoderPeriod(void* encoder_pointer, int32_t *status) {
  Encoder* encoder = (Encoder*) encoder_pointer;
  tEncoder::tTimerOutput output = encoder->encoder->readTimerOutput(status);
  double value;
  if (output.Stalled) {
	// Return infinity
	double zero = 0.0;
	value = 1.0 / zero;
  } else {
	// output.Period is a fixed point number that counts by 2 (24 bits, 25 integer bits)
	value = (double)(output.Period << 1) / (double)output.Count;
  }
  double measuredPeriod = value * 1.0e-6;
  return measuredPeriod / DECODING_SCALING_FACTOR;
}

/**
 * Sets the maximum period for stopped detection.
 * Sets the value that represents the maximum period of the Encoder before it will assume
 * that the attached device is stopped. This timeout allows users to determine if the wheels or
 * other shaft has stopped rotating.
 * This method compensates for the decoding type.
 * 
 * @deprecated Use SetMinRate() in favor of this method.  This takes unscaled periods and SetMinRate() scales using value from SetDistancePerPulse().
 * 
 * @param maxPeriod The maximum time between rising and falling edges before the FPGA will
 * report the device stopped. This is expressed in seconds.
 */
void setEncoderMaxPeriod(void* encoder_pointer, double maxPeriod, int32_t *status) {
  Encoder* encoder = (Encoder*) encoder_pointer;
  encoder->encoder->writeTimerConfig_StallPeriod((uint32_t)(maxPeriod * 1.0e6 * DECODING_SCALING_FACTOR), status);
}

/**
 * Determine if the encoder is stopped.
 * Using the MaxPeriod value, a boolean is returned that is true if the encoder is considered
 * stopped and false if it is still moving. A stopped encoder is one where the most recent pulse
 * width exceeds the MaxPeriod.
 * @return True if the encoder is considered stopped.
 */
bool getEncoderStopped(void* encoder_pointer, int32_t *status) {
  Encoder* encoder = (Encoder*) encoder_pointer;
  return encoder->encoder->readTimerOutput_Stalled(status) != 0;
}

/**
 * The last direction the encoder value changed.
 * @return The last direction the encoder value changed.
 */
bool getEncoderDirection(void* encoder_pointer, int32_t *status) {
  Encoder* encoder = (Encoder*) encoder_pointer;
  return encoder->encoder->readOutput_Direction(status);
}

/**
 * Set the direction sensing for this encoder.
 * This sets the direction sensing on the encoder so that it could count in the correct
 * software direction regardless of the mounting.
 * @param reverseDirection true if the encoder direction should be reversed
 */
void setEncoderReverseDirection(void* encoder_pointer, bool reverseDirection, int32_t *status) {
  Encoder* encoder = (Encoder*) encoder_pointer;
  encoder->encoder->writeConfig_Reverse(reverseDirection, status);
}

/**
 * Set the Samples to Average which specifies the number of samples of the timer to 
 * average when calculating the period. Perform averaging to account for 
 * mechanical imperfections or as oversampling to increase resolution.
 * @param samplesToAverage The number of samples to average from 1 to 127.
 */
void setEncoderSamplesToAverage(void* encoder_pointer, uint32_t samplesToAverage, int32_t *status) {
  Encoder* encoder = (Encoder*) encoder_pointer;
  if (samplesToAverage < 1 || samplesToAverage > 127) {
	*status = PARAMETER_OUT_OF_RANGE;
  }
  encoder->encoder->writeTimerConfig_AverageSize(samplesToAverage, status);
}

/**
 * Get the Samples to Average which specifies the number of samples of the timer to 
 * average when calculating the period. Perform averaging to account for 
 * mechanical imperfections or as oversampling to increase resolution.
 * @return SamplesToAverage The number of samples being averaged (from 1 to 127)
 */
uint32_t getEncoderSamplesToAverage(void* encoder_pointer, int32_t *status) {
  Encoder* encoder = (Encoder*) encoder_pointer;
  return encoder->encoder->readTimerConfig_AverageSize(status);
}

/**
 * Get the loop timing of the Digital Module
 * 
 * @return The loop time
 */  
uint16_t getLoopTiming(int32_t *status) {
  return getLoopTimingWithModule(1, status);
}

/**
 * Get the loop timing of the Digital Module
 * 
 * @return The loop time
 */  
uint16_t getLoopTimingWithModule(uint8_t module, int32_t *status) {
  return pwmSystem->readLoopTiming(status);
}

// XXX: What happened to SPI?
// struct spi_t {
//   tSPI* spi;
//   tSPI::tConfig config;
//   tSPI::tChannels channels;
//   MUTEX_ID semaphore;
// };
// typedef struct spi_t SPI;

// void* initializeSPI(uint8_t sclk_routing_module, uint32_t sclk_routing_pin,
// 					uint8_t mosi_routing_module, uint32_t mosi_routing_pin,
// 					uint8_t miso_routing_module, uint32_t miso_routing_pin, int32_t *status) {
//   SPI* spi = new SPI();
  
//   spi->semaphore = initializeMutex(SEMAPHORE_Q_PRIORITY | SEMAPHORE_DELETE_SAFE | SEMAPHORE_INVERSION_SAFE);
  
//   spi->spi = tSPI::create(status);

//   spi->config.BusBitWidth = 8;
//   spi->config.ClockHalfPeriodDelay = 0;
//   spi->config.MSBfirst = 0;
//   spi->config.DataOnFalling = 0;
//   spi->config.LatchFirst = 0;
//   spi->config.LatchLast = 0;
//   spi->config.FramePolarity = 0;
//   spi->config.WriteOnly = miso_routing_pin ? 0 : 1;
//   spi->config.ClockPolarity = 0;

//   spi->channels.SCLK_Channel = sclk_routing_pin;
//   spi->channels.SCLK_Module = sclk_routing_module;
//   spi->channels.SS_Channel = 0;
//   spi->channels.SS_Module = 0;

//   if (mosi_routing_pin) {
// 	spi->channels.MOSI_Channel = mosi_routing_pin;
// 	spi->channels.MOSI_Module = mosi_routing_module;
//   } else {
// 	spi->channels.MOSI_Channel = 0;
// 	spi->channels.MOSI_Module = 0;
//   }

//   if (miso_routing_pin) {
// 	spi->channels.MISO_Channel = miso_routing_pin;
// 	spi->channels.MISO_Module = miso_routing_module;
//   } else {
// 	spi->channels.MISO_Channel = 0;
// 	spi->channels.MISO_Module = 0;
//   }
//   return spi;
// }

// void cleanSPI(void* spi_pointer, int32_t *status) {
//   SPI* spi = (SPI*) spi_pointer;
//   delete spi->spi;
//   delete spi;
// }

// /**
//  * Configure the number of bits from each word that the slave transmits
//  * or receives.
//  *
//  * @param bits	The number of bits in one frame (1 to 32 bits).
//  */
// void setSPIBitsPerWord(void* spi_pointer, uint32_t bits, int32_t *status) {
//   SPI* spi = (SPI*) spi_pointer;
//   spi->config.BusBitWidth = bits;
// }

// /**
//  * Get the number of bits from each word that the slave transmits
//  * or receives.
//  *
//  * @return The number of bits in one frame (1 to 32 bits).
//  */
// uint32_t getSPIBitsPerWord(void* spi_pointer, int32_t *status) {
//   SPI* spi = (SPI*) spi_pointer;
//   return spi->config.BusBitWidth;
// }

// /**
//  * Configure the rate of the generated clock signal.
//  * The default and maximum value is 76,628.4 Hz.
//  *
//  * @param hz	The clock rate in Hertz.
//  */
// void setSPIClockRate(void* spi_pointer, double hz, int32_t *status) {
//   SPI* spi = (SPI*) spi_pointer;
//   int delay = 0;
//   int loopTiming = getLoopTimingWithModule(spi->spi->readChannels_SCLK_Module(status), status);
//   double v = (1.0 / hz) / (2 * loopTiming / (kSystemClockTicksPerMicrosecond * 1e6));
//   if (v < 1) {
// 	// TODO: wpi_setWPIErrorWithContext(ParameterOutOfRange, "SPI Clock too high");
//   }
//   delay = (int) (v + .5);
//   if (delay > 255) {
// 	// TODO: wpi_setWPIErrorWithContext(ParameterOutOfRange, "SPI Clock too low");
//   }

//   spi->config.ClockHalfPeriodDelay = delay;
// }

// /**
//  * Configure the order that bits are sent and received on the wire
//  * to be most significant bit first.
//  */
// void setSPIMSBFirst(void* spi_pointer, int32_t *status) {
//   SPI* spi = (SPI*) spi_pointer;
//   spi->config.MSBfirst = 1;
// }

// /**
//  * Configure the order that bits are sent and received on the wire
//  * to be least significant bit first.
//  */
// void setSPILSBFirst(void* spi_pointer, int32_t *status) {
//   SPI* spi = (SPI*) spi_pointer;
//   spi->config.MSBfirst = 0;
// }

// /**
//  * Configure that the data is stable on the falling edge and the data
//  * changes on the rising edge.
//  */
// void setSPISampleDataOnFalling(void* spi_pointer, int32_t *status) {
//   SPI* spi = (SPI*) spi_pointer;
//   spi->config.DataOnFalling = 1;
// }

// /**
//  * Configure that the data is stable on the rising edge and the data
//  * changes on the falling edge.
//  */
// void setSPISampleDataOnRising(void* spi_pointer, int32_t *status) {
//   SPI* spi = (SPI*) spi_pointer;
//   spi->config.DataOnFalling = 0;
// }

// void setSPISlaveSelect(void* spi_pointer, uint8_t ss_routing_module, uint32_t ss_routing_pin,
// 					   int32_t *status) {
//   SPI* spi = (SPI*) spi_pointer;
//   spi->channels.SS_Channel = ss_routing_pin;
//   spi->channels.SS_Module = ss_routing_module;
// }

// void setSPILatchMode(void* spi_pointer, tFrameMode mode, int32_t *status) {
//   SPI* spi = (SPI*) spi_pointer;
//   switch (mode) {
//   case kChipSelect:
// 	spi->config.LatchFirst = 0;
// 	spi->config.LatchLast = 0;
// 	break;
//   case kPreLatchPulse:
// 	spi->config.LatchFirst = 1;
// 	spi->config.LatchLast = 0;
// 	break;
//   case kPostLatchPulse:
// 	spi->config.LatchFirst = 0;
// 	spi->config.LatchLast = 1;
// 	break;
//   case kPreAndPostLatchPulse:
// 	spi->config.LatchFirst = 1;
// 	spi->config.LatchLast = 1;
// 	break;
//   }
// }

// tFrameMode getSPILatchMode(void* spi_pointer, int32_t *status) {
//   SPI* spi = (SPI*) spi_pointer;
//   return (tFrameMode) (spi->config.LatchFirst | (spi->config.LatchLast << 1));
// }

// void setSPIFramePolarity(void* spi_pointer, bool activeLow, int32_t *status) {
//   SPI* spi = (SPI*) spi_pointer;
//   spi->config.FramePolarity = activeLow ? 1 : 0;
// }

// bool getSPIFramePolarity(void* spi_pointer, int32_t *status) {
//   SPI* spi = (SPI*) spi_pointer;
//   return spi->config.FramePolarity != 0;
// }

// /**
//  * Configure the clock output line to be active low.
//  * This is sometimes called clock polarity high.
//  */
// void setSPIClockActiveLow(void* spi_pointer, int32_t *status) {
//   SPI* spi = (SPI*) spi_pointer;
//   spi->config.ClockPolarity = 1;
// }

// /**
//  * Configure the clock output line to be active high.
//  * This is sometimes called clock polarity low.
//  */
// void setSPIClockActiveHigh(void* spi_pointer, int32_t *status) {
//   SPI* spi = (SPI*) spi_pointer;
//   spi->config.ClockPolarity = 0;
// }


// /**
//  * Apply configuration settings and reset the SPI logic.
//  */
// void applySPIConfig(void* spi_pointer, int32_t *status) {
//   SPI* spi = (SPI*) spi_pointer;
//   Synchronized sync(spi->semaphore);

//   spi->spi->writeConfig(spi->config, status);
//   spi->spi->writeChannels(spi->channels, status);
//   spi->spi->strobeReset(status);
// }

// /**
//  * Get the number of words that can currently be stored before being
//  * transmitted to the device.
//  *
//  * @return The number of words available to be written.
//  */
// uint16_t getSPIOutputFIFOAvailable(void* spi_pointer, int32_t *status) {
//   SPI* spi = (SPI*) spi_pointer;
//   uint16_t result = spi->spi->readAvailableToLoad(status);
//   return result;
// }

// /**
//  * Get the number of words received and currently available to be read from
//  * the receive FIFO.
//  *
//  * @return The number of words available to read.
//  */
// uint16_t getSPINumReceived(void* spi_pointer, int32_t *status) {
//   SPI* spi = (SPI*) spi_pointer;
//   uint16_t result = spi->spi->readReceivedElements(status);
//   return result;
// }

// /**
//  * Have all pending transfers completed?
//  *
//  * @return True if no transfers are pending.
//  */
// bool isSPIDone(void* spi_pointer, int32_t *status) {
//   SPI* spi = (SPI*) spi_pointer;
//   bool result = spi->spi->readStatus_Idle(status);
//   return result;
// }


// /**
//  * Determine if the receive FIFO was full when attempting to add new data at
//  * end of a transfer.
//  *
//  * @return True if the receive FIFO overflowed.
//  */
// bool hadSPIReceiveOverflow(void* spi_pointer, int32_t *status) {
//   SPI* spi = (SPI*) spi_pointer;
//   bool result = spi->spi->readStatus_ReceivedDataOverflow(status);
//   return result;
// }

// /**
//  * Write a word to the slave device.  Blocks until there is space in the
//  * output FIFO.
//  *
//  * If not running in output only mode, also saves the data received
//  * on the MISO input during the transfer into the receive FIFO.
//  */
// void writeSPI(void* spi_pointer, uint32_t data, int32_t *status) {
//   SPI* spi = (SPI*) spi_pointer;
//   if (spi->channels.MOSI_Channel == 0 && spi->channels.MOSI_Module == 0) {
// 	*status = SPI_WRITE_NO_MOSI;
// 	return;
//   }

//   Synchronized sync(spi->semaphore);

//   while (getSPIOutputFIFOAvailable(spi_pointer, status) == 0)
// 	delayTicks(HAL_NO_WAIT);

//   spi->spi->writeDataToLoad(data, status);
//   spi->spi->strobeLoad(status);
// }

// /**
//  * Read a word from the receive FIFO.
//  *
//  * Waits for the current transfer to complete if the receive FIFO is empty.
//  *
//  * If the receive FIFO is empty, there is no active transfer, and initiate
//  * is false, errors.
//  *
//  * @param initiate	If true, this function pushes "0" into the
//  *				    transmit buffer and initiates a transfer.
//  *				    If false, this function assumes that data is
//  *				    already in the receive FIFO from a previous write.
//  */
// uint32_t readSPI(void* spi_pointer, bool initiate, int32_t *status) {
//   SPI* spi = (SPI*) spi_pointer;
//   if (spi->channels.MISO_Channel == 0 && spi->channels.MISO_Module == 0) {
// 	*status = SPI_READ_NO_MISO;
// 	return 0;
//   }

//   uint32_t data;
//   {
// 	Synchronized sync(spi->semaphore);

// 	if (initiate) {
// 	  spi->spi->writeDataToLoad(0, status);
// 	  spi->spi->strobeLoad(status);
// 	}

// 	// Do we have anything ready to read?
// 	if (getSPINumReceived(spi_pointer, status) == 0) {
// 	  if (!initiate && isSPIDone(spi_pointer, status)
// 		  && getSPIOutputFIFOAvailable(spi_pointer, status) == kTransmitFIFODepth) {
// 		// Nothing to read: error out
// 		*status = SPI_READ_NO_DATA;
// 		return 0;
// 	  }

// 	  // Wait for the transaction to complete
// 	  while (getSPINumReceived(spi_pointer, status) == 0)
// 		delayTicks(HAL_NO_WAIT);
// 	}

// 	spi->spi->strobeReadReceivedData(status);
// 	data = spi->spi->readReceivedData(status);
//   }

//   return data;
// }

// /**
//  * Stop any transfer in progress and empty the transmit FIFO.
//  */
// void resetSPI(void* spi_pointer, int32_t *status) {
//   SPI* spi = (SPI*) spi_pointer;
//   spi->spi->strobeReset(status);
// }

// /**
//  * Empty the receive FIFO.
//  */
// void clearSPIReceivedData(void* spi_pointer, int32_t *status) {
//   SPI* spi = (SPI*) spi_pointer;
//   spi->spi->strobeClearReceivedData(status);
// }



void* initializeSPI(uint8_t sclk_routing_module, uint32_t sclk_routing_pin,
					uint8_t mosi_routing_module, uint32_t mosi_routing_pin,
					uint8_t miso_routing_module, uint32_t miso_routing_pin, int32_t *status) {
  return NULL;
}
void cleanSPI(void* spi_pointer, int32_t *status) {}
void setSPIBitsPerWord(void* spi_pointer, uint32_t bits, int32_t *status) {}
uint32_t getSPIBitsPerWord(void* spi_pointer, int32_t *status) {
  return 0;
}
void setSPIClockRate(void* spi_pointer, double hz, int32_t *status) {}
void setSPIMSBFirst(void* spi_pointer, int32_t *status) {}
void setSPILSBFirst(void* spi_pointer, int32_t *status) {}
void setSPISampleDataOnFalling(void* spi_pointer, int32_t *status) {}
void setSPISampleDataOnRising(void* spi_pointer, int32_t *status) {}
void setSPISlaveSelect(void* spi_pointer, uint8_t ss_routing_module, uint32_t ss_routing_pin,
					   int32_t *status) {}
void setSPILatchMode(void* spi_pointer, tFrameMode mode, int32_t *status) {}
tFrameMode getSPILatchMode(void* spi_pointer, int32_t *status) {
  return (tFrameMode) 0;
}
void setSPIFramePolarity(void* spi_pointer, bool activeLow, int32_t *status) {}
bool getSPIFramePolarity(void* spi_pointer, int32_t *status) {
  return false;
}
void setSPIClockActiveLow(void* spi_pointer, int32_t *status) {}
void setSPIClockActiveHigh(void* spi_pointer, int32_t *status) {}
void applySPIConfig(void* spi_pointer, int32_t *status) {}
uint16_t getSPIOutputFIFOAvailable(void* spi_pointer, int32_t *status) {
  return 0;
}
uint16_t getSPINumReceived(void* spi_pointer, int32_t *status) {
  return 0;
}
bool isSPIDone(void* spi_pointer, int32_t *status) {
  return false;
}
bool hadSPIReceiveOverflow(void* spi_pointer, int32_t *status) {
  return false;
}
void writeSPI(void* spi_pointer, uint32_t data, int32_t *status) {}
uint32_t readSPI(void* spi_pointer, bool initiate, int32_t *status) {}
void resetSPI(void* spi_pointer, int32_t *status) {}
void clearSPIReceivedData(void* spi_pointer, int32_t *status) {}

/**
 * Generic transaction.
 * 
 * This is a lower-level interface to the I2C hardware giving you more control over each transaction.
 * 
 * @param dataToSend Buffer of data to send as part of the transaction.
 * @param sendSize Number of bytes to send as part of the transaction. [0..6]
 * @param dataReceived Buffer to read data into.
 * @param receiveSize Number of byted to read from the device. [0..7]
 * @return Transfer Aborted... false for success, true for aborted.
 */
bool doI2CTransaction(uint8_t address, bool compatibilityMode, uint8_t *dataToSend,
					  uint8_t sendSize, uint8_t *dataReceived, uint8_t receiveSize,
					  int32_t *status) {
  return doI2CTransactionWithModule(1, address, compatibilityMode, dataToSend, sendSize,
									dataReceived, receiveSize, status);
}

/**
 * Generic transaction.
 * 
 * This is a lower-level interface to the I2C hardware giving you more control over each transaction.
 * 
 * @param dataToSend Buffer of data to send as part of the transaction.
 * @param sendSize Number of bytes to send as part of the transaction. [0..6]
 * @param dataReceived Buffer to read data into.
 * @param receiveSize Number of byted to read from the device. [0..7]
 * @return Transfer Aborted... false for success, true for aborted.
 */
bool doI2CTransactionWithModule(uint8_t module, uint8_t address, bool compatibilityMode,
								uint8_t *dataToSend, uint8_t sendSize, uint8_t *dataReceived,
								uint8_t receiveSize, int32_t *status) {
  // initializeDigital(status);
  // if (sendSize > 6) {
  // 	*status = PARAMETER_OUT_OF_RANGE;
  // 	// TODO: wpi_setWPIErrorWithContext(ParameterOutOfRange, "sendSize");
  // 	return true;
  // }
  // if (receiveSize > 7) {
  // 	*status = PARAMETER_OUT_OF_RANGE;
  // 	// TODO: wpi_setWPIErrorWithContext(ParameterOutOfRange, "receiveSize");
  // 	return true;
  // }

  // uint32_t data=0;
  // uint32_t dataHigh=0;
  // uint32_t i;
  // for(i=0; i<sendSize && i<sizeof(data); i++) {
  // 	data |= (uint32_t)dataToSend[i] << (8*i);
  // }
  // for(; i<sendSize; i++) {
  // 	dataHigh |= (uint32_t)dataToSend[i] << (8*(i-sizeof(data)));
  // }

  // bool aborted = true;
  // {
  // 	Synchronized sync(digitalI2CSemaphore);
  // 	digitalModules[module]->writeI2CConfig_Address(address, status);
  // 	digitalModules[module]->writeI2CConfig_BytesToWrite(sendSize, status);
  // 	digitalModules[module]->writeI2CConfig_BytesToRead(receiveSize, status);
  // 	if (sendSize > 0) digitalModules[module]->writeI2CDataToSend(data, status);
  // 	if (sendSize > sizeof(data)) digitalModules[module]->writeI2CConfig_DataToSendHigh(dataHigh, status);
  // 	digitalModules[module]->writeI2CConfig_BitwiseHandshake(compatibilityMode, status);
  // 	uint8_t transaction = digitalModules[module]->readI2CStatus_Transaction(status);
  // 	digitalModules[module]->strobeI2CStart(status);
  // 	while(transaction == digitalModules[module]->readI2CStatus_Transaction(status)) delayTicks(1);
  // 	while(!digitalModules[module]->readI2CStatus_Done(status)) delayTicks(1);
  // 	aborted = digitalModules[module]->readI2CStatus_Aborted(status);
  // 	if (receiveSize > 0) data = digitalModules[module]->readI2CDataReceived(status);
  // 	if (receiveSize > sizeof(data)) dataHigh = digitalModules[module]->readI2CStatus_DataReceivedHigh(status);
  // }

  // for(i=0; i<receiveSize && i<sizeof(data); i++) {
  // 	dataReceived[i] = (data >> (8*i)) & 0xFF;
  // }
  // for(; i<receiveSize; i++) {
  // 	dataReceived[i] = (dataHigh >> (8*(i-sizeof(data)))) & 0xFF;
  // }
  // return aborted;
  return false; // XXX: What happened to I2C?
}


//// Float JNA Hack
// double
void setPWMRateIntHack(int32_t rate, int32_t *status) {
  setPWMRate(intToFloat(rate), status);
}

void setPWMRateWithModuleIntHack(uint8_t module, int32_t rate, int32_t *status) {
  setPWMRateWithModule(module, intToFloat(rate), status);
}

void setPWMDutyCycleIntHack(void* pwmGenerator, int32_t dutyCycle, int32_t *status) {
  setPWMDutyCycle(pwmGenerator, intToFloat(dutyCycle), status);
}

void setPWMDutyCycleWithModuleIntHack(uint8_t module, void* pwmGenerator, int32_t dutyCycle, int32_t *status) {
  setPWMDutyCycleWithModule(module, pwmGenerator, dutyCycle, status);
}
