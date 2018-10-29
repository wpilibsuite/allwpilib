/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <comdef.h>
#include <shlwapi.h>  // QISearch

#include <cassert>

namespace cs {

template <typename Interface>
class RemoveAddRefRelease : public Interface {
  ULONG __stdcall AddRef();
  ULONG __stdcall Release();
  virtual ~RemoveAddRefRelease();
};

template <typename Interface>
class ComPtr {
 public:
  template <typename T>
  friend class ComPtr;

  ComPtr(std::nullptr_t = nullptr) noexcept {}  // NOLINT(runtime/explicit)
  ComPtr(const ComPtr& other) noexcept : m_ptr(other.m_ptr) {
    InternalAddRef();
  }

  template <typename T>
  ComPtr(const ComPtr<T>& other) noexcept : m_ptr(other.m_ptr) {
    InternalAddRef();
  }

  template <typename T>
  ComPtr(ComPtr<T>&& other) noexcept : m_ptr(other.m_ptr) {
    other.m_ptr = nullptr;
  }

  ~ComPtr() noexcept { InternalRelease(); }

  ComPtr& operator=(const ComPtr& other) noexcept {
    InternalCopy(other.m_ptr);
    return *this;
  }

  template <typename T>
  ComPtr& operator=(const ComPtr<T>& other) noexcept {
    InternalCopy(other.m_ptr);
    return *this;
  }

  template <typename T>
  ComPtr& operator=(ComPtr<T>&& other) noexcept {
    InternalMove(other);
    return *this;
  }

  void Swap(ComPtr& other) noexcept {
    Interface* temp = m_ptr;
    m_ptr = other.m_ptr;
    other.m_ptr = temp;
  }

  explicit operator bool() const noexcept { return nullptr != m_ptr; }

  void Reset() noexcept { InternalRelease(); }

  Interface* Get() const noexcept { return m_ptr; }

  Interface* Detach() noexcept {
    Interface* temp = m_ptr;
    m_ptr = nullptr;
    return temp;
  }

  void Copy(Interface* other) noexcept { InternalCopy(other); }

  void Attach(Interface* other) noexcept {
    InternalRelease();
    m_ptr = other;
  }

  Interface** GetAddressOf() noexcept {
    assert(m_ptr == nullptr);
    return &m_ptr;
  }

  void CopyTo(Interface** other) const noexcept {
    InternalAddRef();
    *other = m_ptr;
  }

  template <typename T>
  ComPtr<T> As() const noexcept {
    ComPtr<T> temp;
    m_ptr->QueryInterface(temp.GetAddressOf());
    return temp;
  }

  RemoveAddRefRelease<Interface>* operator->() const noexcept {
    return static_cast<RemoveAddRefRelease<Interface>*>(m_ptr);
  }

 private:
  Interface* m_ptr = nullptr;

  void InternalAddRef() const noexcept {
    if (m_ptr) {
      m_ptr->AddRef();
    }
  }

  void InternalRelease() noexcept {
    Interface* temp = m_ptr;
    if (temp) {
      m_ptr = nullptr;
      temp->Release();
    }
  }

  void InternalCopy(Interface* other) noexcept {
    if (m_ptr != other) {
      InternalRelease();
      m_ptr = other;
      InternalAddRef();
    }
  }

  template <typename T>
  void InternalMove(ComPtr<T>& other) noexcept {
    if (m_ptr != other.m_ptr) {
      InternalRelease();
      m_ptr = other.m_ptr;
      other.m_ptr = nullptr;
    }
  }
};

template <typename Interface>
void swap(
    ComPtr<Interface>& left,
    ComPtr<Interface>& right) noexcept {  // NOLINT(build/include_what_you_use)
  left.Swap(right);
}

}  // namespace cs
