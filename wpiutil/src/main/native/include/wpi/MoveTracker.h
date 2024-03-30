// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <atomic>
#include <concepts>
#include <memory>

#include "wpi/Signal.h"

namespace wpi {

class MoveTrackerBase;

namespace detail {
class MoveTrackerData {
  friend class wpi::MoveTrackerBase;

 public:
#if 0
  ~MoveTrackerData() {
    if (m_ptr != nullptr) {
      onMove(nullptr, m_ptr);
    }
  }
#endif
  MoveTrackerData(const MoveTrackerData&) = delete;
  MoveTrackerData& operator=(const MoveTrackerData&) = delete;

  MoveTrackerBase* Get() const { return m_ptr; }

#if 0
  /**
   * Signal called on object move or deletion.
   * First parameter is the destination (moved to) object pointer, or nullptr
   * if the object is being deleted.
   * Second parameter is the source (moved from) object pointer.
   *
   * @note On move, the callback is called from the MoveTrackerBase base class
   * move constructor, so the pointer cannot be safely upcasted and used in the
   * callback itself because the move has not yet completed for the derived
   * class.
   */
  wpi::sig::Signal_mt<MoveTrackerBase*, MoveTrackerBase*> onMove;
#endif

 private:
  explicit MoveTrackerData(MoveTrackerBase* ptr) : m_ptr{ptr} {}

  void Move(MoveTrackerBase* to, MoveTrackerBase* from) {
    m_ptr = to;
#if 0
    onMove(to, from);
#endif
  }

  std::atomic<MoveTrackerBase*> m_ptr;
};
}  // namespace detail

/**
 * Base class that calls a list of callbacks on object move and deletion.
 * The list of callbacks is not copied or changed on object copy.
 */
class MoveTrackerBase {
  template <std::derived_from<MoveTrackerBase> T>
  friend class MoveWeakPtr;

 public:
  virtual ~MoveTrackerBase() {
    if (m_tracker) {
      m_tracker->Move(nullptr, this);
    }
  }

  MoveTrackerBase(const MoveTrackerBase& rhs) noexcept {}
  MoveTrackerBase& operator=(const MoveTrackerBase& rhs) noexcept {  // NOLINT
    return *this;
  }

  MoveTrackerBase(MoveTrackerBase&& rhs) : m_tracker{std::move(rhs.m_tracker)} {
    if (m_tracker) {
      m_tracker->Move(this, &rhs);
    }
  }

  MoveTrackerBase& operator=(MoveTrackerBase&& rhs) {
    if (&rhs != this) {
      if (m_tracker) {
        m_tracker->Move(nullptr, this);
      }
      m_tracker = std::move(rhs.m_tracker);
      if (m_tracker) {
        m_tracker->Move(this, &rhs);
      }
    }
    return *this;
  }

 protected:
  MoveTrackerBase()
      : m_tracker{std::make_shared<detail::MoveTrackerData>(this)} {}

 private:
  std::shared_ptr<detail::MoveTrackerData> m_tracker;
};

template <std::derived_from<MoveTrackerBase> T>
class MoveWeakPtr {
 public:
  explicit MoveWeakPtr(T* ptr)
      : m_data{static_cast<MoveTrackerBase*>(ptr)->m_tracker} {}

  T* Get() const {
    if (auto data = m_data.lock()) {
      return static_cast<T*>(data->Get());
    } else {
      return nullptr;
    }
  }

 private:
  std::weak_ptr<detail::MoveTrackerData> m_data;
};

}  // namespace wpi
