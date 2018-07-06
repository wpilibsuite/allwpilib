/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <AHRS.h>
#include "HALInitializer.h"
#include "HAL/Accelerometer.h"
#include "VMXPointers.h"

namespace hal {
    namespace init {
        void InitializeAccelerometer() {}
    }
}

extern "C" {
    void HAL_SetAccelerometerActive(HAL_Bool active) {
        if (active) {
            // TODO: Add Start, after stopping, functionality to VMX-pi HAL [Issue: #93]
        } else {
            vmxIMU->Stop();
        }
    }

    void HAL_SetAccelerometerRange(HAL_AccelerometerRange range) {
        // TODO: Add SetAccelerometerRange functionality to VMX-pi HAL [Issue: #93]
    }

    double HAL_GetAccelerometerX(void) {
        return (double) vmxIMU->GetWorldLinearAccelX();
    }

    double HAL_GetAccelerometerY(void) {
        return (double) vmxIMU->GetWorldLinearAccelY();
    }

    double HAL_GetAccelerometerZ(void) {
        return (double) vmxIMU->GetWorldLinearAccelZ();
    }
}