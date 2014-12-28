#include "HAL/Compressor.hpp"
#include "ctre/PCM.h"
#include <iostream>

static const int NUM_MODULE_NUMBERS = 63;
extern PCM *modules[NUM_MODULE_NUMBERS];
extern void initializePCM(int module);

void *initializeCompressor(uint8_t module) {
	initializePCM(module);
	
	return modules[module];
}

bool checkCompressorModule(uint8_t module) {
	return module < NUM_MODULE_NUMBERS;
}

bool getCompressor(void *pcm_pointer, int32_t *status) {
	PCM *module = (PCM *)pcm_pointer;
	bool value;
	
	*status = module->GetCompressor(value);
	
	return value;
}


void setClosedLoopControl(void *pcm_pointer, bool value, int32_t *status) {
	PCM *module = (PCM *)pcm_pointer;
	
	*status = module->SetClosedLoopControl(value);
}


bool getClosedLoopControl(void *pcm_pointer, int32_t *status) {
	PCM *module = (PCM *)pcm_pointer;
	bool value;
	
	*status = module->GetClosedLoopControl(value);
	
	return value;
}


bool getPressureSwitch(void *pcm_pointer, int32_t *status) {
	PCM *module = (PCM *)pcm_pointer;
	bool value;
	
	*status = module->GetPressure(value);
	
	return value;
}


float getCompressorCurrent(void *pcm_pointer, int32_t *status) {
	PCM *module = (PCM *)pcm_pointer;
	float value;
	
	*status = module->GetCompressorCurrent(value);
	
	return value;
}
bool getCompressorCurrentTooHighFault(void *pcm_pointer, int32_t *status) {
	PCM *module = (PCM *)pcm_pointer;
	bool value;
	
	*status = module->GetCompressorCurrentTooHighFault(value);
	
	return value;
}
bool getCompressorCurrentTooHighStickyFault(void *pcm_pointer, int32_t *status) {
	PCM *module = (PCM *)pcm_pointer;
	bool value;
	
	*status = module->GetCompressorCurrentTooHighStickyFault(value);
	
	return value;
}
bool getCompressorShortedStickyFault(void *pcm_pointer, int32_t *status) {
	PCM *module = (PCM *)pcm_pointer;
	bool value;
	
	*status = module->GetCompressorShortedStickyFault(value);
	
	return value;
}
bool getCompressorShortedFault(void *pcm_pointer, int32_t *status) {
	PCM *module = (PCM *)pcm_pointer;
	bool value;
	
	*status = module->GetCompressorShortedFault(value);
	
	return value;
}
bool getCompressorNotConnectedStickyFault(void *pcm_pointer, int32_t *status) {
	PCM *module = (PCM *)pcm_pointer;
	bool value;
	
	*status = module->GetCompressorNotConnectedStickyFault(value);
	
	return value;
}
bool getCompressorNotConnectedFault(void *pcm_pointer, int32_t *status) {
	PCM *module = (PCM *)pcm_pointer;
	bool value;
	
	*status = module->GetCompressorNotConnectedFault(value);
	
	return value;
}
void clearAllPCMStickyFaults(void *pcm_pointer, int32_t *status) {
	PCM *module = (PCM *)pcm_pointer;
	
	*status = module->ClearStickyFaults();
}
