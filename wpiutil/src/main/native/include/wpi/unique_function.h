/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef WPIUTIL_WPI_UNIQUE_FUNCTION_H_
#define WPIUTIL_WPI_UNIQUE_FUNCTION_H_

#include <functional>
#include <type_traits>
#include <utility>

namespace wpi {

/**
 * Move-only variant of std::function.
 *
 * Implementation from https://stackoverflow.com/a/52358928
 */
template <typename T>
class unique_function : public std::function<T> {
  template <typename Fn, typename En = void>
  struct wrapper;

  // specialization for CopyConstructible Fn
  template <typename Fn>
  struct wrapper<Fn, std::enable_if_t<std::is_copy_constructible<Fn>::value>> {
    Fn fn;

    template <typename... Args>
    auto operator()(Args&&... args) {
      return fn(std::forward<Args>(args)...);
    }
  };

  // specialization for MoveConstructible-only Fn
  template <typename Fn>
  struct wrapper<Fn, std::enable_if_t<!std::is_copy_constructible<Fn>::value &&
                                      std::is_move_constructible<Fn>::value>> {
    Fn fn;

    wrapper(Fn&& fn) : fn(std::forward<Fn>(fn)) {}  // NOLINT: runtime/explicit

    wrapper(wrapper&&) = default;
    wrapper& operator=(wrapper&&) = default;

    // these two functions are instantiated by std::function
    // and are never called
    wrapper(const wrapper& rhs) : fn(const_cast<Fn&&>(rhs.fn)) {
      throw 0;
    }  // hack to initialize fn for non-DefaultContructible types
    wrapper& operator=(wrapper&) { throw 0; }

    template <typename... Args>
    auto operator()(Args&&... args) {
      return fn(std::forward<Args>(args)...);
    }
  };

  using base = std::function<T>;

 public:
  unique_function() noexcept = default;
  unique_function(std::nullptr_t) noexcept  // NOLINT: runtime/explicit
      : base(nullptr) {}

  template <typename Fn>
  unique_function(Fn&& f)  // NOLINT: runtime/explicit
      : base(wrapper<Fn>{std::forward<Fn>(f)}) {}

  unique_function(unique_function&&) = default;
  unique_function& operator=(unique_function&&) = default;

  unique_function& operator=(std::nullptr_t) {
    base::operator=(nullptr);
    return *this;
  }

  template <typename Fn>
  unique_function& operator=(Fn&& f) {
    base::operator=(wrapper<Fn>{std::forward<Fn>(f)});
    return *this;
  }

  using base::operator();
};

}  // namespace wpi

#endif  // WPIUTIL_WPI_UNIQUE_FUNCTION_H_
