#pragma once
#ifndef __HAL_PDP_H__
#define __HAL_PDP_H__

#ifdef __vxworks
#include <vxWorks.h>
#else
#include <stdint.h>
#endif

extern "C"
{
	double getPDPTemperature(int32_t *status);
	double getPDPVoltage(int32_t *status);
	double getPDPChannelCurrent(uint8_t channel, int32_t *status);
}

#endif /* __HAL_PDP_H__ */

