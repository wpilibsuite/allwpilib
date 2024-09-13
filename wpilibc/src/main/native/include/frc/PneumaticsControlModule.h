// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>

#include <hal/Types.h>
#include <wpi/DenseMap.h>
#include <wpi/mutex.h>

#include "PneumaticsBase.h"

namespace frc {
/** Module class for controlling a Cross The Road Electronics Pneumatics Control
 * Module. */
class PneumaticsControlModule : public PneumaticsBase {
 public:
  /** Constructs a PneumaticsControlModule with the default ID (0). */
  PneumaticsControlModule();

  /**
   * Constructs a PneumaticsControlModule.
   *
   * @param module module number to construct
   */
  explicit PneumaticsControlModule(int module);

  ~PneumaticsControlModule() override = default;

  bool GetCompressor() const override;

  /**
   * Disables the compressor. The compressor will not turn on until
   * EnableCompressorDigital() is called.
   */
  void DisableCompressor() override;

  void EnableCompressorDigital() override;

  /**
   * Enables the compressor in digital mode. Analog mode is unsupported by the
   * CTRE PCM.
   *
   * @param minPressure Unsupported.
   * @param maxPressure Unsupported.
   * @see EnableCompressorDigital()
   */
  void EnableCompressorAnalog(
      units::pounds_per_square_inch_t minPressure,
      units::pounds_per_square_inch_t maxPressure) override;

  /**
   * Enables the compressor in digital mode. Hybrid mode is unsupported by the
   * CTRE PCM.
   *
   * @param minPressure Unsupported.
   * @param maxPressure Unsupported.
   * @see EnableCompressorDigital()
   */
  void EnableCompressorHybrid(
      units::pounds_per_square_inch_t minPressure,
      units::pounds_per_square_inch_t maxPressure) override;

  CompressorConfigType GetCompressorConfigType() const override;

  bool GetPressureSwitch() const override;

  units::ampere_t GetCompressorCurrent() const override;

  /**
   * Return whether the compressor current is currently too high.
   *
   * @return True if the compressor current is too high, otherwise false.
   * @see GetCompressorCurrentTooHighStickyFault()
   */
  bool GetCompressorCurrentTooHighFault() const;

  /**
   * Returns whether the compressor current has been too high since sticky
   * faults were last cleared. This fault is persistent and can be cleared by
   * ClearAllStickyFaults()
   *
   * @return True if the compressor current has been too high since sticky
   * faults were last cleared.
   * @see GetCompressorCurrentTooHighFault()
   */
  bool GetCompressorCurrentTooHighStickyFault() const;

  /**
   * Returns whether the compressor is currently shorted.
   *
   * @return True if the compressor is currently shorted, otherwise false.
   * @see GetCompressorShortedStickyFault()
   */
  bool GetCompressorShortedFault() const;

  /**
   * Returns whether the compressor has been shorted since sticky faults were
   * last cleared. This fault is persistent and can be cleared by
   * ClearAllStickyFaults()
   *
   * @return True if the compressor has been shorted since sticky faults were
   * last cleared, otherwise false.
   * @see GetCompressorShortedFault()
   */
  bool GetCompressorShortedStickyFault() const;

  /**
   * Returns whether the compressor is currently disconnected.
   *
   * @return True if compressor is currently disconnected, otherwise false.
   * @see GetCompressorNotConnectedStickyFault()
   */
  bool GetCompressorNotConnectedFault() const;

  /**
   * Returns whether the compressor has been disconnected since sticky faults
   * were last cleared. This fault is persistent and can be cleared by
   * ClearAllStickyFaults()
   *
   * @return True if the compressor has been disconnected since sticky faults
   * were last cleared, otherwise false.
   * @see GetCompressorNotConnectedFault()
   */
  bool GetCompressorNotConnectedStickyFault() const;

  /**
   * Returns whether the solenoid is currently reporting a voltage fault.
   *
   * @return True if solenoid is reporting a fault, otherwise false.
   * @see GetSolenoidVoltageStickyFault()
   */
  bool GetSolenoidVoltageFault() const;

  /**
   * Returns whether the solenoid has reported a voltage fault since sticky
   * faults were last cleared. This fault is persistent and can be cleared by
   * ClearAllStickyFaults()
   *
   * @return True if solenoid is reporting a fault, otherwise false.
   * @see GetSolenoidVoltageFault()
   */
  bool GetSolenoidVoltageStickyFault() const;

  /** Clears all sticky faults on this device. */
  void ClearAllStickyFaults();

  void SetSolenoids(int mask, int values) override;

  int GetSolenoids() const override;

  int GetModuleNumber() const override;

  int GetSolenoidDisabledList() const override;

  void FireOneShot(int index) override;

  void SetOneShotDuration(int index, units::second_t duration) override;

  bool CheckSolenoidChannel(int channel) const override;

  int CheckAndReserveSolenoids(int mask) override;

  void UnreserveSolenoids(int mask) override;

  bool ReserveCompressor() override;

  void UnreserveCompressor() override;

  /**
   * Unsupported by the CTRE PCM.
   *
   * @param channel Unsupported.
   * @return 0
   */
  units::volt_t GetAnalogVoltage(int channel) const override;

  /**
   * Unsupported by the CTRE PCM.
   *
   * @param channel Unsupported.
   * @return 0
   */
  units::pounds_per_square_inch_t GetPressure(int channel) const override;

  Solenoid MakeSolenoid(int channel) override;
  DoubleSolenoid MakeDoubleSolenoid(int forwardChannel,
                                    int reverseChannel) override;
  Compressor MakeCompressor() override;

 private:
  class DataStore;
  friend class DataStore;
  friend class PneumaticsBase;
  PneumaticsControlModule(HAL_CTREPCMHandle handle, int module);

  static std::shared_ptr<PneumaticsBase> GetForModule(int module);

  std::shared_ptr<DataStore> m_dataStore;
  HAL_CTREPCMHandle m_handle;
  int m_module;

  static wpi::mutex m_handleLock;
  static std::unique_ptr<wpi::DenseMap<int, std::weak_ptr<DataStore>>>
      m_handleMap;
  static std::weak_ptr<DataStore>& GetDataStore(int module);
};
}  // namespace frc
