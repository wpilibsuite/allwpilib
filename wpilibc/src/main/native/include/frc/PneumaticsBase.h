// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>

#include <units/current.h>
#include <units/pressure.h>
#include <units/time.h>
#include <units/voltage.h>

#include "frc/CompressorConfigType.h"
#include "frc/PneumaticsModuleType.h"

namespace frc {
class Solenoid;
class DoubleSolenoid;
class Compressor;
class PneumaticsBase {
 public:
  virtual ~PneumaticsBase() = default;

  virtual bool GetCompressor() const = 0;

  virtual bool GetPressureSwitch() const = 0;

  virtual units::ampere_t GetCompressorCurrent() const = 0;

  virtual void DisableCompressor() = 0;

  virtual void EnableCompressorDigital() = 0;

  virtual void EnableCompressorAnalog(
      units::pounds_per_square_inch_t minPressure,
      units::pounds_per_square_inch_t maxPressure) = 0;

  virtual void EnableCompressorHybrid(
      units::pounds_per_square_inch_t minPressure,
      units::pounds_per_square_inch_t maxPressure) = 0;

  virtual CompressorConfigType GetCompressorConfigType() const = 0;

  virtual void SetSolenoids(int mask, int values) = 0;

  virtual int GetSolenoids() const = 0;

  virtual int GetModuleNumber() const = 0;

  virtual int GetSolenoidDisabledList() const = 0;

  virtual void FireOneShot(int index) = 0;

  virtual void SetOneShotDuration(int index, units::second_t duration) = 0;

  virtual bool CheckSolenoidChannel(int channel) const = 0;

  virtual int CheckAndReserveSolenoids(int mask) = 0;

  virtual void UnreserveSolenoids(int mask) = 0;

  virtual bool ReserveCompressor() = 0;

  virtual void UnreserveCompressor() = 0;

  virtual units::volt_t GetAnalogVoltage(int channel) const = 0;

  virtual units::pounds_per_square_inch_t GetPressure(int channel) const = 0;

  virtual Solenoid MakeSolenoid(int channel) = 0;
  virtual DoubleSolenoid MakeDoubleSolenoid(int forwardChannel,
                                            int reverseChannel) = 0;
  virtual Compressor MakeCompressor() = 0;

  static std::shared_ptr<PneumaticsBase> GetForType(
      int module, PneumaticsModuleType moduleType);
  static int GetDefaultForType(PneumaticsModuleType moduleType);
};
}  // namespace frc
