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

