// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>
#include <initializer_list>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>

namespace wpi {

/// StringMap - this is a map lightly specialized for handling string keys
template <typename T,
          typename Allocator = std::allocator<std::pair<const std::string, T>>>
class StringMap : public std::map<std::string, T, std::less<>, Allocator> {
 public:
  using map_type = typename std::map<std::string, T, std::less<>>;
  using key_type = typename map_type::key_type;
  using mapped_type = typename map_type::mapped_type;
  using value_type = typename map_type::value_type;
  using size_type = typename map_type::size_type;
  using difference_type = typename map_type::difference_type;
  using key_compare = typename map_type::key_compare;
  using allocator_type = typename map_type::allocator_type;
  using reference = typename map_type::reference;
  using const_reference = typename map_type::const_reference;
  using pointer = typename map_type::pointer;
  using const_pointer = typename map_type::const_pointer;
  using iterator = typename map_type::iterator;
  using const_iterator = typename map_type::const_iterator;
  using reverse_iterator = typename map_type::reverse_iterator;
  using const_reverse_iterator = typename map_type::const_reverse_iterator;
  using node_type = typename map_type::node_type;
  using insert_return_type = typename map_type::insert_return_type;

  StringMap() = default;
  explicit StringMap(const Allocator& alloc) : map_type{alloc} {}

  template <typename InputIt>
  StringMap(InputIt first, InputIt last, const Allocator& alloc = Allocator())
      : map_type{first, last, alloc} {}

  StringMap(const StringMap&) = default;
  StringMap(const StringMap& other, const Allocator& alloc)
      : map_type{other, alloc} {}

  StringMap(StringMap&&) = default;
  StringMap(StringMap&& other, const Allocator& alloc)
      : map_type{other, alloc} {}

  StringMap(std::initializer_list<value_type> init,
            const Allocator& alloc = Allocator())
      : map_type{init, alloc} {}

  StringMap& operator=(const StringMap&) = default;
  StringMap& operator=(StringMap&&) noexcept(
      std::allocator_traits<Allocator>::is_always_equal::value &&
      std::is_nothrow_move_assignable<std::less<>>::value) = default;
  StringMap& operator=(std::initializer_list<value_type> ilist) {
    map_type::operator=(ilist);
    return *this;
  }

  using map_type::at;

  T& at(const char* key) { return (*this)[std::string_view{key}]; }

  const T& at(const char* key) const { return (*this)[std::string_view{key}]; }

  T& at(std::string_view key) {
#ifdef __cpp_lib_associative_heterogeneous_insertion
    return map_type::at(key);
#else
    auto it = find(key);
    if (it == this->end()) {
      throw std::out_of_range{std::string{key}};
    }
    return it->second;
#endif
  }

  const T& at(std::string_view key) const {
#ifdef __cpp_lib_associative_heterogeneous_insertion
    return map_type::at(key);
#else
    auto it = find(key);
    if (it == this->end()) {
      throw std::out_of_range{std::string{key}};
    }
    return it->second;
#endif
  }

  using map_type::operator[];

  T& operator[](const char* key) { return (*this)[std::string_view{key}]; }

  T& operator[](std::string_view key) { return try_emplace(key).first->second; }

  using map_type::insert_or_assign;

  template <typename M>
  std::pair<iterator, bool> insert_or_assign(const char* key, M&& obj) {
    return insert_or_assign(std::string_view{key}, std::forward<M>(obj));
  }

  template <typename M>
  std::pair<iterator, bool> insert_or_assign(std::string_view key, M&& obj) {
#ifdef __cpp_lib_associative_heterogeneous_insertion
    return map_type::insert_or_assign(key, std::forward<M>(obj));
#else
    auto it = find(key);
    if (it != this->end()) {
      it->second = std::forward<M>(obj);
      return {it, false};
    } else {
      return this->emplace(std::string{key}, std::forward<M>(obj));
    }
#endif
  }

  using map_type::try_emplace;

  template <typename... Args>
  std::pair<iterator, bool> try_emplace(const char* key, Args&&... args) {
    return try_emplace(std::string_view{key}, std::forward<Args>(args)...);
  }

  template <typename... Args>
  std::pair<iterator, bool> try_emplace(std::string_view key, Args&&... args) {
#ifdef __cpp_lib_associative_heterogeneous_insertion
    return map_type::try_emplace(key, std::forward<Args>(args)...);
#else
    auto it = find(key);
    if (it != this->end()) {
      return {it, false};
    }
    return map_type::try_emplace(std::string{key}, std::forward<Args>(args)...);
#endif
  }

  using map_type::erase;

  size_type erase(const char* key) { return erase(std::string_view{key}); }

  size_type erase(std::string_view key) {
#ifdef __cpp_lib_associative_heterogeneous_erasure
    return map_type::erase(key);
#else
    auto it = find(key);
    if (it == this->end()) {
      return 0;
    }
    this->erase(it);
    return 1;
#endif
  }

  void swap(StringMap& other) noexcept(
      std::allocator_traits<Allocator>::is_always_equal::value &&
      std::is_nothrow_swappable<std::less<>>::value) {
    map_type::swap(other);
  }

  using map_type::extract;

  node_type extract(const char* key) { return extract(std::string_view{key}); }

  node_type extract(std::string_view key) {
#ifdef __cpp_lib_associative_heterogeneous_insertion
    return map_type::extract(key);
#else
    auto it = find(key);
    if (it == this->end()) {
      return {};
    }
    return extract(it);
#endif
  }

  iterator find(std::string_view key) { return map_type::find(key); }

  const_iterator find(std::string_view key) const {
    return map_type::find(key);
  }
};

}  // namespace wpi

namespace std {
template <typename T>
inline void swap(wpi::StringMap<T>& lhs, wpi::StringMap<T>& rhs) {
  lhs.swap(rhs);
}
}  // namespace std
