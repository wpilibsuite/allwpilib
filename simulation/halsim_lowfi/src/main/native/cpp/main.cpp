/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <iostream>

#include <HALSimLowFi.h>
#include <NTProvider_PWM.h>
#include <NTProvider_DIO.h>

static HALSimLowFi halsim_lowfi;
static HALSimNTProviderPWM pwm_provider;
static HALSimNTProviderDIO dio_provider;

extern "C" {
#if defined(WIN32) || defined(_WIN32)
__declspec(dllexport)
#endif
    int HALSIM_InitExtension(void) {
        std::cout << "NetworkTables LowFi Simulator Initializing." << std::endl;
        halsim_lowfi.Initialize();
        halsim_lowfi.table->GetInstance().StartServer("networktables.ini");
        auto lowfi = std::make_shared<HALSimLowFi>(halsim_lowfi);

        pwm_provider.Inject(lowfi, "PWM");
        dio_provider.Inject(lowfi, "DIO");
        return 0;
    }
}  // extern "C"
