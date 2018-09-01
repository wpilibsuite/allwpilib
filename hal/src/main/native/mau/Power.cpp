/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "HAL/Power.h"

namespace hal {
    namespace init {
        void InitializePower() {}
    }
}

extern "C" {
    double HAL_GetVinVoltage(int32_t* status) {
    //    return SimRoboRioData[0].GetVInVoltage();
        // TODO: ALL DYLAN! ALL!!!!
        return 0;
    }

    double HAL_GetVinCurrent(int32_t* status) {
    //    return SimRoboRioData[0].GetVInCurrent();
        // TODO: ALL DYLAN! ALL!!!!
        return 0;
    }

    double HAL_GetUserVoltage6V(int32_t* status) {
    //    return SimRoboRioData[0].GetUserVoltage6V();
        // TODO: ALL DYLAN! ALL!!!!
        return 0;
    }

    double HAL_GetUserCurrent6V(int32_t* status) {
    //    return SimRoboRioData[0].GetUserCurrent6V();
        // TODO: ALL DYLAN! ALL!!!!
        return 0;
    }

    HAL_Bool HAL_GetUserActive6V(int32_t* status) {
    //    return SimRoboRioData[0].GetUserActive6V();
        // TODO: ALL DYLAN! ALL!!!!
        return 0;
    }

    int32_t HAL_GetUserCurrentFaults6V(int32_t* status) {
    //    return SimRoboRioData[0].GetUserFaults6V();
        // TODO: ALL DYLAN! ALL!!!!
        return 0;
    }

    double HAL_GetUserVoltage5V(int32_t* status) {
    //    return SimRoboRioData[0].GetUserVoltage5V();
        // TODO: ALL DYLAN! ALL!!!!
        return 0;
    }

    double HAL_GetUserCurrent5V(int32_t* status) {
    //    return SimRoboRioData[0].GetUserCurrent5V();
        // TODO: ALL DYLAN! ALL!!!!
        return 0;
    }

    HAL_Bool HAL_GetUserActive5V(int32_t* status) {
    //    return SimRoboRioData[0].GetUserActive5V();
        // TODO: ALL DYLAN! ALL!!!!
        return 0;
    }

    int32_t HAL_GetUserCurrentFaults5V(int32_t* status) {
    //    return SimRoboRioData[0].GetUserFaults5V();
        // TODO: ALL DYLAN! ALL!!!!
        return 0;
    }

    double HAL_GetUserVoltage3V3(int32_t* status) {
    //    return SimRoboRioData[0].GetUserVoltage3V3();
        // TODO: ALL DYLAN! ALL!!!!
        return 0;
    }

    double HAL_GetUserCurrent3V3(int32_t* status) {
    //    return SimRoboRioData[0].GetUserCurrent3V3();
        // TODO: ALL DYLAN! ALL!!!!
        return 0;
    }

    HAL_Bool HAL_GetUserActive3V3(int32_t* status) {
    //    return SimRoboRioData[0].GetUserActive3V3();
        // TODO: ALL DYLAN! ALL!!!!
        return 0;
    }

    int32_t HAL_GetUserCurrentFaults3V3(int32_t* status) {
    //    return SimRoboRioData[0].GetUserFaults3V3();
        // TODO: ALL DYLAN! ALL!!!!
        return 0;
    }
}  // extern "C"
