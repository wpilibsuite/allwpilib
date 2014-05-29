#include "HAL/Compressor.hpp"
#include "ctre/PCM.h"
#include <iostream>

static const int NUM_PCMS = 2;
extern PCM *modules[NUM_PCMS];
extern void initializePCM();

void *initializeCompressor(uint8_t module) {
	initializePCM();
	
	return modules[module - 1];
}

bool checkCompressorModule(uint8_t module) {
	return module > 0 and module <= NUM_PCMS;
}


void setCompressor(void *pcm_pointer, bool value, int32_t *status) {
	PCM *module = (PCM *)pcm_pointer;
	
	*status = module->SetCompressor(value);
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

