/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "HAL/AnalogAccumulator.h"

#include "AnalogInternal.h"
#include <VMXIO.h>
#include <VMXErrors.h>
#include <VMXResource.h>
#include "VMXPointers.h"

using namespace hal;

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

        //  SimAnalogInData[port->channel].SetAccumulatorInitialized(true);
    }

    void HAL_ResetAccumulator(HAL_AnalogInputHandle analogPortHandle, int32_t* status) {
        auto port = analogInputHandles->Get(analogPortHandle);
        if (port == nullptr) {
            *status = HAL_HANDLE_ERROR;
            return;
        }

        // TODO: Add ResetAccumulator functionality to VMX-pi HAL [Issue: #93]
        //  SimAnalogInData[port->channel].SetAccumulatorCenter(0);
        //  SimAnalogInData[port->channel].SetAccumulatorCount(0);
        //  SimAnalogInData[port->channel].SetAccumulatorValue(0);
    }

    void HAL_SetAccumulatorCenter(HAL_AnalogInputHandle analogPortHandle, int32_t center, int32_t* status) {
        auto port = analogInputHandles->Get(analogPortHandle);
        if (port == nullptr) {
            *status = HAL_HANDLE_ERROR;
            return;
        }

        // TODO: Add SetAccumulatorCenter functionality to VMX-pi HAL [Issue: #93]
    }

    void HAL_SetAccumulatorDeadband(HAL_AnalogInputHandle analogPortHandle, int32_t deadband, int32_t* status) {
        auto port = analogInputHandles->Get(analogPortHandle);
        if (port == nullptr) {
            *status = HAL_HANDLE_ERROR;
            return;
        }

        // TODO: Add SetAccumulatorDeadband functionality to VMX-pi HAL [Issue: #93]
        //  SimAnalogInData[port->channel].SetAccumulatorDeadband(deadband);
    }

    int64_t HAL_GetAccumulatorValue(HAL_AnalogInputHandle analogPortHandle, int32_t* status) {
        auto port = analogInputHandles->Get(analogPortHandle);
        if (port == nullptr) {
            *status = HAL_HANDLE_ERROR;
            return 0;
        }

        VMXResourceHandle handle = CREATE_VMX_RESOURCE_HANDLE(VMXResourceType::Accumulator, port);
        uint32_t output;
        VMXErrorCode error;

        return vmxIO->Accumulator_GetInstantaneousValue(handle, output, &error);
    }

    int64_t HAL_GetAccumulatorCount(HAL_AnalogInputHandle analogPortHandle, int32_t* status) {
        auto port = analogInputHandles->Get(analogPortHandle);
        if (port == nullptr) {
            *status = HAL_HANDLE_ERROR;
            return 0;
        }

        // TODO: Add GetAccumulatorCount functionality to VMX-pi HAL [Issue: #93]
        //  return SimAnalogInData[port->channel].GetAccumulatorCount();
    }

    void HAL_GetAccumulatorOutput(HAL_AnalogInputHandle analogPortHandle, int64_t* value, int64_t* count, int32_t* status) {
        auto port = analogInputHandles->Get(analogPortHandle);
        if (port == nullptr) {
            *status = HAL_HANDLE_ERROR;
            return;
        }

        //  *count = SimAnalogInData[port->channel].GetAccumulatorCount();
        //  *value = SimAnalogInData[port->channel].GetAccumulatorValue();
    }
}
