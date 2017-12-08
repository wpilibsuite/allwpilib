/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <HALSimLowFi.h>

#include <vector>

struct PWMPublisherInfo {
    void *provider;
    int channel;
};

class HALSimNTProviderPWM : public HALSimNTProvider {
public:
    virtual void Initialize() override;
private:
    int numChannels;
    std::vector<PWMPublisherInfo> publishers;
};