// Copyright (c) Sleipnir contributors

#pragma once

#include <functional>
#include <memory>
#include <type_traits>
#include <utility>

namespace sleipnir {

/**
 * An implementation of std::function_ref, a lightweight non-owning reference to
 * a callable.
 */
template <class F>
class function_ref;

template <class R, class... Args>
class function_ref<R(Args...)> {
 public:
  constexpr function_ref() noexcept = delete;

  /**
   * Creates a `function_ref` which refers to the same callable as `rhs`.
   */
  constexpr function_ref(const function_ref<R(Args...)>& rhs) noexcept =
      default;

  /**
   * Constructs a `function_ref` referring to `f`.
   */
  template <typename F>
    requires(!std::is_same_v<std::decay_t<F>, function_ref> &&
             std::is_invocable_r_v<R, F &&, Args...>)
  constexpr function_ref(F&& f) noexcept  // NOLINT(google-explicit-constructor)
      : obj_(const_cast<void*>(
            reinterpret_cast<const void*>(std::addressof(f)))) {
    callback_ = [](void* obj, Args... args) -> R {
      return std::invoke(
          *reinterpret_cast<typename std::add_pointer<F>::type>(obj),
          std::forward<Args>(args)...);
    };
  }

  /**
   * Makes `*this` refer to the same callable as `rhs`.
   */
  constexpr function_ref<R(Args...)>& operator=(
      const function_ref<R(Args...)>& rhs) noexcept = default;

  /**
   * Makes `*this` refer to `f`.
   */
  template <typename F>
    requires std::is_invocable_r_v<R, F&&, Args...>
  constexpr function_ref<R(Args...)>& operator=(F&& f) noexcept {
    obj_ = reinterpret_cast<void*>(std::addressof(f));
    callback_ = [](void* obj, Args... args) {
      return std::invoke(
          *reinterpret_cast<typename std::add_pointer<F>::type>(obj),
          std::forward<Args>(args)...);
    };

    return *this;
  }

  /**
   * Swaps the referred callables of `*this` and `rhs`.
   */
  constexpr void swap(function_ref<R(Args...)>& rhs) noexcept {
    std::swap(obj_, rhs.obj_);
    std::swap(callback_, rhs.callback_);
  }

  /**
   * Call the stored callable with the given arguments.
   */
  R operator()(Args... args) const {
    return callback_(obj_, std::forward<Args>(args)...);
  }

 private:
  void* obj_ = nullptr;
  R (*callback_)(void*, Args...) = nullptr;
};

/**
 * Swaps the referred callables of `lhs` and `rhs`.
 */
template <typename R, typename... Args>
constexpr void swap(function_ref<R(Args...)>& lhs,
                    function_ref<R(Args...)>& rhs) noexcept {
  lhs.swap(rhs);
}

template <typename R, typename... Args>
function_ref(R (*)(Args...)) -> function_ref<R(Args...)>;

}  // namespace sleipnir
