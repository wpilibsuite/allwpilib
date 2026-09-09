// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <cstdint>
#include <type_traits>

#include "wpi/tunables/TunableRegistry.hpp"
#include "wpi/tunables/detail/TunableTypeValue.hpp"
#include "wpi/util/SymbolExports.hpp"

namespace wpi::tunables {

struct TunableConfig;
class TunableRegistry;

namespace detail {

/** The base class for tunables. */
class WPILIB_DLLEXPORT TunableBase {
  friend class wpi::tunables::TunableRegistry;
  friend struct wpi::tunables::TunableRegistry::TunableInfo;

 public:
  /**
   * Returns this tunable's tuning revision token.
   *
   * The token starts at zero and changes once for each tuning input that a
   * backend successfully applies to this tunable. Direct local Set() calls,
   * assignment, in-place mutation, and getter refreshes do not change it.
   * Reading the token does not consume or reset it, so independent observers
   * can each store a previous token and compare it to the current value with
   * !=.
   *
   * Treat this as a 64-bit equality token. Do not rely on ordering or sign.
   * This getter follows the same threading model as the rest of the tunable API
   * and does not make tunable access thread-safe.
   *
   * The revision is stored in the registry record for registered tunables. Unit
   * tests that call TunableRegistry::Reset() while a tunable is still alive
   * discard that record; such stale objects report zero until they are
   * re-created or otherwise re-registered.
   *
   * @return current tuning revision token
   */
  uint64_t GetTuneRevision() const;

 protected:
  struct ForceTunableRegister {};

  constexpr explicit TunableBase(TunableTypeValue type)
      : m_uid{static_cast<uint32_t>(type) | TYPE_FLAG} {}
  TunableBase(TunableTypeValue type, const ForceTunableRegister&)
      : m_uid{TunableRegistry::RegisterTunable(this, nullptr, type)} {}
  TunableBase(TunableTypeValue type, const TunableConfig& config)
      : m_uid{TunableRegistry::RegisterTunable(this, &config, type)} {}

  constexpr TunableBase(const TunableBase& other)
      : m_uid{GetUnregisteredTypeUid(other.m_uid)} {}

  constexpr TunableBase(TunableBase&& other) : m_uid{other.m_uid} {
    if (IsRegisteredUid(m_uid)) {
      TunableRegistry::MoveTunable(m_uid & UID_MASK, this);
      other.m_uid = GetUnregisteredTypeUid(m_uid);
    }
  }

  constexpr TunableBase& operator=(const TunableBase& other) {
    if (this != &other) {
      if (IsRegisteredUid(m_uid)) {
        SetTunableChanged();
      } else {
        m_uid = GetUnregisteredTypeUid(other.m_uid);
      }
    }
    return *this;
  }

  constexpr TunableBase& operator=(TunableBase&& other) {
    if (this != &other) {
      if (IsRegisteredUid(m_uid)) {
        SetTunableChanged();
        if (IsRegisteredUid(other.m_uid)) {
          other.SetTunableChanged();
        }
      } else {
        m_uid = other.m_uid;
        if (IsRegisteredUid(m_uid)) {
          TunableRegistry::MoveTunable(m_uid & UID_MASK, this);
          other.m_uid = GetUnregisteredTypeUid(m_uid);
        }
      }
    }
    return *this;
  }

  constexpr ~TunableBase() {
    if (IsRegisteredUid(m_uid)) {
      TunableRegistry::UnregisterTunable(m_uid & UID_MASK);
    }
  }

  uint32_t GetTunableUid() const { return m_uid & UID_MASK; }

  constexpr void SetTunableChanged() {
    bool wasChanged = m_uid & CHANGE_FLAG;
    m_uid |= CHANGE_FLAG;
    if (!std::is_constant_evaluated() && IsRegisteredUid(m_uid) &&
        !wasChanged) {
      TunableRegistry::NotifyChanged(m_uid & UID_MASK);
    }
  }

 private:
  constexpr static uint32_t TYPE_FLAG = 0x80000000;
  constexpr static uint32_t CHANGE_FLAG = 0x40000000;
  constexpr static uint32_t UID_MASK = 0x3FFFFFFF;

  constexpr static bool IsRegisteredUid(uint32_t uid) {
    return (uid & TYPE_FLAG) == 0;
  }

  constexpr static uint32_t GetUnregisteredTypeUid(uint32_t uid) {
    if (IsRegisteredUid(uid)) {
      return ((uid & UID_MASK) >> 24) | TYPE_FLAG;
    } else {
      return uid;
    }
  }

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
}  // namespace wpi::tunables
