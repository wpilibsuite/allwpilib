// Copyright (c) Sleipnir contributors

#pragma once

#include <concepts>
#include <cstddef>
#include <memory>
#include <utility>

namespace slp {

/**
 * A custom intrusive shared pointer implementation without thread
 * synchronization overhead.
 *
 * Types used with this class should have three things:
 *
 * 1. A zero-initialized public counter variable that serves as the shared
 *    pointer's reference count.
 * 2. A free function `void inc_ref_count(T*)` that increments the reference
 *    count.
 * 3. A free function `void dec_ref_count(T*)` that decrements the reference
 *    count and deallocates the pointed to object if the reference count reaches
 *    zero.
 *
 * @tparam T The type of the object to be reference counted.
 */
template <typename T>
class IntrusiveSharedPtr {
 public:
  template <typename>
  friend class IntrusiveSharedPtr;

  /**
   * Constructs an empty intrusive shared pointer.
   */
  constexpr IntrusiveSharedPtr() noexcept = default;

  /**
   * Constructs an empty intrusive shared pointer.
   */
  constexpr IntrusiveSharedPtr(std::nullptr_t) noexcept {}  // NOLINT

  /**
   * Constructs an intrusive shared pointer from the given pointer and takes
   * ownership.
   *
   * @param ptr The pointer of which to take ownership.
   */
  explicit constexpr IntrusiveSharedPtr(T* ptr) noexcept : m_ptr{ptr} {
    if (m_ptr != nullptr) {
      inc_ref_count(m_ptr);
    }
  }

  constexpr ~IntrusiveSharedPtr() {
    if (m_ptr != nullptr) {
      dec_ref_count(m_ptr);
    }
  }

  /**
   * Copy constructs from the given intrusive shared pointer.
   *
   * @param rhs The other intrusive shared pointer.
   */
  constexpr IntrusiveSharedPtr(const IntrusiveSharedPtr<T>& rhs) noexcept
      : m_ptr{rhs.m_ptr} {
    if (m_ptr != nullptr) {
      inc_ref_count(m_ptr);
    }
  }

  /**
   * Copy constructs from the given intrusive shared pointer.
   *
   * @param rhs The other intrusive shared pointer.
   */
  template <typename U>
    requires(!std::same_as<T, U> && std::convertible_to<U*, T*>)
  constexpr IntrusiveSharedPtr(  // NOLINT
      const IntrusiveSharedPtr<U>& rhs) noexcept
      : m_ptr{rhs.m_ptr} {
    if (m_ptr != nullptr) {
      inc_ref_count(m_ptr);
    }
  }

  /**
   * Makes a copy of the given intrusive shared pointer.
   *
   * @param rhs The other intrusive shared pointer.
   * @return This intrusive shared pointer.
   */
  constexpr IntrusiveSharedPtr<T>& operator=(  // NOLINT
      const IntrusiveSharedPtr<T>& rhs) noexcept {
    if (m_ptr == rhs.m_ptr) {
      return *this;
    }

    if (m_ptr != nullptr) {
      dec_ref_count(m_ptr);
    }

    m_ptr = rhs.m_ptr;

    if (m_ptr != nullptr) {
      inc_ref_count(m_ptr);
    }

    return *this;
  }

  /**
   * Makes a copy of the given intrusive shared pointer.
   *
   * @param rhs The other intrusive shared pointer.
   * @return This intrusive shared pointer.
   */
  template <typename U>
    requires(!std::same_as<T, U> && std::convertible_to<U*, T*>)
  constexpr IntrusiveSharedPtr<T>& operator=(  // NOLINT
      const IntrusiveSharedPtr<U>& rhs) noexcept {
    if (m_ptr == rhs.m_ptr) {
      return *this;
    }

    if (m_ptr != nullptr) {
      dec_ref_count(m_ptr);
    }

    m_ptr = rhs.m_ptr;

    if (m_ptr != nullptr) {
      inc_ref_count(m_ptr);
    }

    return *this;
  }

  /**
   * Move constructs from the given intrusive shared pointer.
   *
   * @param rhs The other intrusive shared pointer.
   */
  constexpr IntrusiveSharedPtr(IntrusiveSharedPtr<T>&& rhs) noexcept
      : m_ptr{std::exchange(rhs.m_ptr, nullptr)} {}

  /**
   * Move constructs from the given intrusive shared pointer.
   *
   * @param rhs The other intrusive shared pointer.
   */
  template <typename U>
    requires(!std::same_as<T, U> && std::convertible_to<U*, T*>)
  constexpr IntrusiveSharedPtr(  // NOLINT
      IntrusiveSharedPtr<U>&& rhs) noexcept
      : m_ptr{std::exchange(rhs.m_ptr, nullptr)} {}

  /**
   * Move assigns from the given intrusive shared pointer.
   *
   * @param rhs The other intrusive shared pointer.
   * @return This intrusive shared pointer.
   */
  constexpr IntrusiveSharedPtr<T>& operator=(
      IntrusiveSharedPtr<T>&& rhs) noexcept {
    if (m_ptr == rhs.m_ptr) {
      return *this;
    }

    std::swap(m_ptr, rhs.m_ptr);

    return *this;
  }

  /**
   * Move assigns from the given intrusive shared pointer.
   *
   * @param rhs The other intrusive shared pointer.
   * @return This intrusive shared pointer.
   */
  template <typename U>
    requires(!std::same_as<T, U> && std::convertible_to<U*, T*>)
  constexpr IntrusiveSharedPtr<T>& operator=(
      IntrusiveSharedPtr<U>&& rhs) noexcept {
    if (m_ptr == rhs.m_ptr) {
      return *this;
    }

    std::swap(m_ptr, rhs.m_ptr);

    return *this;
  }

  /**
   * Returns the internal pointer.
   *
   * @return The internal pointer.
   */
  constexpr T* get() const noexcept { return m_ptr; }

  /**
   * Returns the object pointed to by the internal pointer.
   *
   * @return The object pointed to by the internal pointer.
   */
  constexpr T& operator*() const noexcept { return *m_ptr; }

  /**
   * Returns the internal pointer.
   *
   * @return The internal pointer.
   */
  constexpr T* operator->() const noexcept { return m_ptr; }

  /**
   * Returns true if the internal pointer isn't nullptr.
   *
   * @return True if the internal pointer isn't nullptr.
   */
  explicit constexpr operator bool() const noexcept { return m_ptr != nullptr; }

  /**
   * Returns true if the given intrusive shared pointers point to the same
   * object.
   *
   * @param lhs The left-hand side.
   * @param rhs The right-hand side.
   */
  friend constexpr bool operator==(const IntrusiveSharedPtr<T>& lhs,
                                   const IntrusiveSharedPtr<T>& rhs) noexcept {
    return lhs.m_ptr == rhs.m_ptr;
  }

  /**
   * Returns true if the given intrusive shared pointers point to different
   * objects.
   *
   * @param lhs The left-hand side.
   * @param rhs The right-hand side.
   */
  friend constexpr bool operator!=(const IntrusiveSharedPtr<T>& lhs,
                                   const IntrusiveSharedPtr<T>& rhs) noexcept {
    return lhs.m_ptr != rhs.m_ptr;
  }

  /**
   * Returns true if the left-hand intrusive shared pointer points to nullptr.
   *
   * @param lhs The left-hand side.
   */
  friend constexpr bool operator==(const IntrusiveSharedPtr<T>& lhs,
                                   std::nullptr_t) noexcept {
    return lhs.m_ptr == nullptr;
  }

  /**
   * Returns true if the right-hand intrusive shared pointer points to nullptr.
   *
   * @param rhs The right-hand side.
   */
  friend constexpr bool operator==(std::nullptr_t,
                                   const IntrusiveSharedPtr<T>& rhs) noexcept {
    return nullptr == rhs.m_ptr;
  }

  /**
   * Returns true if the left-hand intrusive shared pointer doesn't point to
   * nullptr.
   *
   * @param lhs The left-hand side.
   */
  friend constexpr bool operator!=(const IntrusiveSharedPtr<T>& lhs,
                                   std::nullptr_t) noexcept {
    return lhs.m_ptr != nullptr;
  }

  /**
   * Returns true if the right-hand intrusive shared pointer doesn't point to
   * nullptr.
   *
   * @param rhs The right-hand side.
   */
  friend constexpr bool operator!=(std::nullptr_t,
                                   const IntrusiveSharedPtr<T>& rhs) noexcept {
    return nullptr != rhs.m_ptr;
  }

 private:
  T* m_ptr = nullptr;
};

/**
 * Constructs an object of type T and wraps it in an intrusive shared pointer
 * using args as the parameter list for the constructor of T.
 *
 * @tparam T Type of object for intrusive shared pointer.
 * @tparam Args Types of constructor arguments.
 * @param args Constructor arguments for T.
 */
template <typename T, typename... Args>
IntrusiveSharedPtr<T> make_intrusive_shared(Args&&... args) {
  return IntrusiveSharedPtr<T>{new T(std::forward<Args>(args)...)};
}

/**
 * Constructs an object of type T and wraps it in an intrusive shared pointer
 * using alloc as the storage allocator of T and args as the parameter list for
 * the constructor of T.
 *
 * @tparam T Type of object for intrusive shared pointer.
 * @tparam Alloc Type of allocator for T.
 * @tparam Args Types of constructor arguments.
 * @param alloc The allocator for T.
 * @param args Constructor arguments for T.
 */
template <typename T, typename Alloc, typename... Args>
IntrusiveSharedPtr<T> allocate_intrusive_shared(Alloc alloc, Args&&... args) {
  auto ptr = std::allocator_traits<Alloc>::allocate(alloc, sizeof(T));
  std::allocator_traits<Alloc>::construct(alloc, ptr,
                                          std::forward<Args>(args)...);
  return IntrusiveSharedPtr<T>{ptr};
}

}  // namespace slp
