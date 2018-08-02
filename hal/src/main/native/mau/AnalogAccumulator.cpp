/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "HAL/AnalogAccumulator.h"

#include "AnalogInternal.h"
#include "HAL/handles/HandlesInternal.h"
#include "MauInternal.h"
#include <VMXIO.h>
#include <VMXErrors.h>
#include <VMXResource.h>

using namespace hal;
using namespace mau;

namespace hal {
    namespace init {
        void InitializeAnalogAccumulator() {}
    }
}

extern "C" {
    HAL_Bool HAL_IsAccumulatorChannel(HAL_AnalogInputHandle analogPortHandle, int32_t* status) {
        auto port = analogInputHandles->Get(analogPortHandle);
        if (port == nullptr) {
            *status = HAL_HANDLE_ERROR;
            return false;
        }
        for (int32_t i = 0; i < kNumAccumulators; i++) {
            if (port->channel == kAccumulatorChannels[i]) return true;
        }
        return false;
    }

    void HAL_InitAccumulator(HAL_AnalogInputHandle analogPortHandle, int32_t* status) {
        auto port = analogInputHandles->Get(analogPortHandle);
        if (port == nullptr) {
            //    *status = HAL_HANDLE_ERROR;
            return;
        }

        if (!HAL_IsAccumulatorChannel(analogPortHandle, status)) {
            *status = HAL_INVALID_ACCUMULATOR_CHANNEL;
            return;
        }

        VMXErrorCode vmxerr;
        int16_t index = getHandleIndex(analogPortHandle);

        VMXChannelIndex first;
        VMXResourceHandle accHandles[4];

        for ( uint8_t inCount = first; inCount < first + kNumAnalogInputs; inCount++) {
            // TODO: ALL DYLAN! ALL!!!!
        }
    }

    void HAL_ResetAccumulator(HAL_AnalogInputHandle analogPortHandle, int32_t* status) {
        auto port = analogInputHandles->Get(analogPortHandle);
        if (port == nullptr) {
            *status = HAL_HANDLE_ERROR;
            return;
        }

        // no-op
        // TODO: Add ResetAccumulator functionality to VMX-pi HAL [Issue: #98]
    }

    void HAL_SetAccumulatorCenter(HAL_AnalogInputHandle analogPortHandle, int32_t center, int32_t* status) {
        auto port = analogInputHandles->Get(analogPortHandle);
        if (port == nullptr) {
            *status = HAL_HANDLE_ERROR;
            return;
        }

        // no-op
        // TODO: Add SetAccumulatorCenter functionality to VMX-pi HAL [Issue: #98]
    }

    void HAL_SetAccumulatorDeadband(HAL_AnalogInputHandle analogPortHandle, int32_t deadband, int32_t* status) {
        auto port = analogInputHandles->Get(analogPortHandle);
        if (port == nullptr) {
            *status = HAL_HANDLE_ERROR;
            return;
        }

        // no-op
        // TODO: Add SetAccumulatorDeadband functionality to VMX-pi HAL [Issue: #98]
    }

    int64_t HAL_GetAccumulatorValue(HAL_AnalogInputHandle analogPortHandle, int32_t* status) {
        auto port = analogInputHandles->Get(analogPortHandle);
        if (port == nullptr) {
            *status = HAL_HANDLE_ERROR;
            return 0;
        }
        // TODO: ALL DYLAN! ALL!!!!
        return 0;
    }

    int64_t HAL_GetAccumulatorCount(HAL_AnalogInputHandle analogPortHandle, int32_t* status) {
        auto port = analogInputHandles->Get(analogPortHandle);
        if (port == nullptr) {
            *status = HAL_HANDLE_ERROR;
            return 0;
        }

        // TODO: Add GetAccumulatorCount functionality to VMX-pi HAL [Issue: #93]
        //  return SimAnalogInData[port->channel].GetAccumulatorCount();
        return 0;
    }

    void HAL_GetAccumulatorOutput(HAL_AnalogInputHandle analogPortHandle, int64_t* value, int64_t* count, int32_t* status) {
        auto port = analogInputHandles->Get(analogPortHandle);
        if (port == nullptr) {
            *status = HAL_HANDLE_ERROR;
            return;
        }

        // no-op
        // TODO: Add GetAccumulatorOutput functionality to VMX-pi HAL [Issue: #98]
        *value = 0;
        *count = 0;
    }
}
