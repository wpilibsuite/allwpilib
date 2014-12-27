/**
 * Compressor.h
 * Methods for interacting with a compressor with the CAN PCM device
 */

#ifdef __vxworks
#include <vxWorks.h>
#else
#include <stdint.h>
#endif

#ifndef __HAL_COMPRESSOR_H__
#define __HAL_COMPRESSOR_H__

extern "C" {
	void *initializeCompressor(uint8_t module);
	bool checkCompressorModule(uint8_t module);
	
	bool getCompressor(void *pcm_pointer, int32_t *status);
	
	void setClosedLoopControl(void *pcm_pointer, bool value, int32_t *status);
	bool getClosedLoopControl(void *pcm_pointer, int32_t *status);
	
	bool getPressureSwitch(void *pcm_pointer, int32_t *status);
	float getCompressorCurrent(void *pcm_pointer, int32_t *status);

	bool getCompressorCurrentTooHighFault(void *pcm_pointer, int32_t *status);
	bool getCompressorCurrentTooHighStickyFault(void *pcm_pointer, int32_t *status);
	bool getCompressorShortedStickyFault(void *pcm_pointer, int32_t *status);
	bool getCompressorShortedFault(void *pcm_pointer, int32_t *status);
	bool getCompressorNotConnectedStickyFault(void *pcm_pointer, int32_t *status);
	bool getCompressorNotConnectedFault(void *pcm_pointer, int32_t *status);
	void clearAllPCMStickyFaults(void *pcm_pointer, int32_t *status);
}

#endif

