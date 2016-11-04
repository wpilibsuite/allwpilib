
#ifndef __AICalibration_h__
#define __AICalibration_h__

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

	uint32_t FRC_NetworkCommunication_nAICalibration_getLSBWeight(const uint32_t aiSystemIndex, const uint32_t channel, int32_t *status);
	int32_t FRC_NetworkCommunication_nAICalibration_getOffset(const uint32_t aiSystemIndex, const uint32_t channel, int32_t *status);

#ifdef __cplusplus
}
#endif

#endif // __AICalibration_h__
