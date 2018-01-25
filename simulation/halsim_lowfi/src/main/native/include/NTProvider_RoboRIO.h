/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>

#include <HALSimLowFi.h>

class HALSimNTProviderRoboRIO : public HALSimNTProvider {
 public:
  void Initialize() override;
  void OnCallback(uint32_t channel,
                  std::shared_ptr<nt::NetworkTable> table) override;
  void OnInitializedChannel(uint32_t channel,
                            std::shared_ptr<nt::NetworkTable> table) override;
};
