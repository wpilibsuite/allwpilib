// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <type_traits>
#include <utility>
#include <vector>

namespace tcb {

namespace detail {

template <typename T, typename...>
struct vec_type_helper {
  using type = T;
};

template <typename... Args>
struct vec_type_helper<void, Args...> {
  using type = typename std::common_type_t<Args...>;
};

template <typename T, typename... Args>
using vec_type_helper_t = typename vec_type_helper<T, Args...>::type;

template <typename, typename...>
struct all_constructible_and_convertible : std::true_type {};

template <typename T, typename First, typename... Rest>
struct all_constructible_and_convertible<T, First, Rest...>
    : std::conditional_t<
          std::is_constructible_v<T, First> && std::is_convertible_v<First, T>,
          all_constructible_and_convertible<T, Rest...>, std::false_type> {};

template <typename T, typename... Args,
          typename std::enable_if_t<!std::is_trivially_copyable_v<T>, int> = 0>
std::vector<T> make_vector_impl(Args&&... args) {
  std::vector<T> vec;
  vec.reserve(sizeof...(Args));
  using arr_t = int[];
  (void)arr_t{0, (vec.emplace_back(std::forward<Args>(args)), 0)...};
  return vec;
}

template <typename T, typename... Args,
          typename std::enable_if_t<std::is_trivially_copyable_v<T>, int> = 0>
std::vector<T> make_vector_impl(Args&&... args) {
  return std::vector<T>{std::forward<Args>(args)...};
}

}  // namespace detail

template <
    typename T = void, typename... Args,
    typename V = detail::vec_type_helper_t<T, Args...>,
    typename std::enable_if_t<
        detail::all_constructible_and_convertible<V, Args...>::value, int> = 0>
std::vector<V> make_vector(Args&&... args) {
  return detail::make_vector_impl<V>(std::forward<Args>(args)...);
}

}  // namespace tcb
