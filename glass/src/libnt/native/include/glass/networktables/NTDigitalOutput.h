/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <string>

#include <ntcore_cpp.h>
#include <wpi/StringRef.h>

#include "glass/DataSource.h"
#include "glass/hardware/DIO.h"
#include "glass/networktables/NetworkTablesHelper.h"

namespace glass {

class NTDigitalOutputModel : public DIOModel {
 public:
  static constexpr const char* kType = "Digital Output";

  // path is to the table containing ".type", excluding the trailing /
  explicit NTDigitalOutputModel(wpi::StringRef path);
  NTDigitalOutputModel(NT_Inst inst, wpi::StringRef path);

  const char* GetName() const override { return m_nameValue.c_str(); }

  const char* GetSimDevice() const override { return nullptr; }

  DPWMModel* GetDPWM() override { return nullptr; }
  DutyCycleModel* GetDutyCycle() override { return nullptr; }
  EncoderModel* GetEncoder() override { return nullptr; }

  bool IsInput() const override { return true; }

  DataSource* GetValueData() override { return &m_valueData; }

  void SetValue(bool val) override;

  void Update() override;
  bool Exists() override;
  bool IsReadOnly() override { return !m_controllableValue; }

 private:
  NetworkTablesHelper m_nt;
  NT_Entry m_value;
  NT_Entry m_name;
  NT_Entry m_controllable;

  DataSource m_valueData;
  std::string m_nameValue;
  bool m_controllableValue = false;
};

}  // namespace glass
