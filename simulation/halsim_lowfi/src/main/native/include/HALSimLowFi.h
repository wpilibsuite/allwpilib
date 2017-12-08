/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <networktables/NetworkTableInstance.h>

class HALSimLowFi {
public:
    std::shared_ptr<nt::NetworkTable> table;
    void Initialize();
};

class HALSimNTProvider {
public:
    void Inject(std::shared_ptr<HALSimLowFi> parent, std::string table);
    // Initialize is called by inject.
    virtual void Initialize() = 0;

    std::shared_ptr<HALSimLowFi> parent;
    std::shared_ptr<nt::NetworkTable> table;
};