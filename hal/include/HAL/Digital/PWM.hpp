#pragma once
#include <stdint.h>

extern "C"
{
  bool checkPWMChannel(void* digital_port_pointer);
  
  void* initializePWMPort(void* port_pointer, int32_t *status);
  void freePWMPort(void* pwm_port_pointer);
  void setPWMBoundsDouble(void* pwm_port_pointer, double max, double deadbandMax, 
                          double center, double deadbandMinPwm, double minPwm,
                          int32_t *status);
  void getPWMBounds(void* pwm_port_pointer, int32_t *max, int32_t *deadbandMax,
                    int32_t *center, int32_t *deadbandMin, int32_t *min);
  void setPWMBounds(void* pwm_port_pointer, int32_t max, int32_t deadbandMax,
                    int32_t center, int32_t deadbandMin, int32_t min);
  void setPWMEliminateDeadband(void* pwm_port_pointer, bool eliminateDeadband);
  bool getPWMEliminateDeadband(void* pwm_port_pointer);
  void setPWM(void* pwm_port_pointer, float value, int32_t *status);
  void setPWMRaw(void* pwm_port_pointer, unsigned short value, int32_t *status);
  float getPWM(void* pwm_port_pointer, int32_t *status);
  unsigned short getPWMRaw(void* pwm_port_pointer, int32_t *status);
	bool allocatePWMChannel(void* pwm_port_pointer, int32_t *status);
	void freePWMChannel(void* pwm_port_pointer, int32_t *status);
	
	void latchPWMZero(void* pwm_port_pointer, int32_t *status);
	void setPWMPeriodScale(void* pwm_port_pointer, uint32_t squelchMask, int32_t *status);
}