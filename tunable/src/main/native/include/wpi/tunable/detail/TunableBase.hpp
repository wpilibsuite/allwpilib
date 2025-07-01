// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <cstdint>

#include "wpi/tunable/TunableRegistry.hpp"
#include "wpi/tunable/detail/TunableTypeValue.hpp"

namespace wpi {

struct TunableConfig;
class TunableRegistry;

namespace detail {

/** The base class for tunables. */
class TunableBase {
  friend class wpi::TunableRegistry;
  friend struct wpi::TunableRegistry::TunableInfo;

 protected:
  struct ForceTunableRegister {};

  constexpr explicit TunableBase(TunableTypeValue type)
      : m_uid{static_cast<uint32_t>(type) | TYPE_FLAG} {}
  TunableBase(TunableTypeValue type, const ForceTunableRegister&)
      : m_uid{TunableRegistry::RegisterTunable(this, nullptr, type)} {}
  TunableBase(TunableTypeValue type, const TunableConfig& config)
      : m_uid{TunableRegistry::RegisterTunable(this, &config, type)} {}

  constexpr TunableBase(const TunableBase& other) {
    if ((other.m_uid & TYPE_FLAG) != 0) {
      m_uid = other.m_uid;
    } else {
      m_uid = static_cast<uint32_t>(
                  TunableRegistry::GetTunable(other.m_uid & UID_MASK).type) |
              TYPE_FLAG;
    }
  }

  constexpr TunableBase(TunableBase&& other) : m_uid{other.m_uid} {
    other.m_uid = 0;
    if ((m_uid & TYPE_FLAG) == 0) {
      TunableRegistry::MoveTunable(m_uid & UID_MASK, this);
    }
  }

  constexpr TunableBase& operator=(const TunableBase& other) {
    if (this != &other) {
      if ((other.m_uid & TYPE_FLAG) != 0) {
        m_uid = other.m_uid;
      } else {
        m_uid = static_cast<uint32_t>(
                    TunableRegistry::GetTunable(other.m_uid & UID_MASK).type) |
                TYPE_FLAG;
      }
    }
    return *this;
  }

  constexpr TunableBase& operator=(TunableBase&& other) {
    if (this != &other) {
      m_uid = other.m_uid;
      other.m_uid = 0;
      if ((m_uid & TYPE_FLAG) == 0) {
        TunableRegistry::MoveTunable(m_uid & UID_MASK, this);
      }
    }
    return *this;
  }

  constexpr ~TunableBase() {
    if ((m_uid & TYPE_FLAG) == 0) {
      TunableRegistry::UnregisterTunable(m_uid & UID_MASK);
    }
  }

  uint32_t GetTunableUid() const { return m_uid & UID_MASK; }

  void SetTunableChanged() { m_uid |= CHANGE_FLAG; }

 private:
  constexpr static uint32_t TYPE_FLAG = 0x80000000;
  constexpr static uint32_t CHANGE_FLAG = 0x40000000;
  constexpr static uint32_t UID_MASK = 0x3FFFFFFF;

  bool GetTunableChanged() const { return m_uid & CHANGE_FLAG; }
  void ResetTunableChanged() { m_uid &= ~CHANGE_FLAG; }

  uint32_t m_uid;
};

/** Base class for member tunables. */
class TunableMemberBase : public TunableBase {
 public:
  virtual ~TunableMemberBase() = default;

 protected:
  constexpr explicit TunableMemberBase(TunableTypeValue type)
      : TunableBase{type} {}
  TunableMemberBase(TunableTypeValue type, const ForceTunableRegister& fr)
      : TunableBase{type, fr} {}
  TunableMemberBase(TunableTypeValue type, const TunableConfig& config)
      : TunableBase{type, config} {}
};

}  // namespace detail
}  // namespace wpi
