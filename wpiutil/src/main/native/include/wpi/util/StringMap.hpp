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

/**
 * StringMap is a sorted associative container that contains key-value pairs
 * with unique string keys. Keys are sorted in the same order as std::string's
 * are compared.  Search, removal, and insertion operations have logarithmic
 * complexity.  The underlying implementation is std::map<std::string, T>.
 */
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

  /** Constructs an empty container. */
  StringMap() = default;

  /**
   * Constructs an empty container.
   *
   * @param alloc allocator to use for all memory allocations of this container
   */
  explicit StringMap(const Allocator& alloc) : map_type{alloc} {}

  /**
   * Constructs the container with the contents of the range [first, last).  If
   * multiple elements in the range have keys that compare equivalent, it is
   * unspecified which element is inserted.  If [first, last) is not a valid
   * range, the behavior is undefined.
   *
   * @param first start of the range to copy the elements from
   * @param last end of the range to copy the elements from
   * @param alloc allocator to use for all memory allocations of this container
   */
  template <typename InputIt>
  StringMap(InputIt first, InputIt last, const Allocator& alloc = Allocator())
      : map_type{first, last, alloc} {}

  /** Copy constructor. */
  StringMap(const StringMap&) = default;

  /**
   * Copy constructor.
   *
   * @param other another container to be used as source to initialize the
   *              elements of the container with
   * @param alloc allocator to use for all memory allocations of this container
   */
  StringMap(const StringMap& other, const Allocator& alloc)
      : map_type{other, alloc} {}

  /** Move constructor. */
  StringMap(StringMap&&) = default;

  /**
   * Move constructor.
   *
   * @param other another container to be used as source to initialize the
   *              elements of the container with
   * @param alloc allocator to use for all memory allocations of this container
   */
  StringMap(StringMap&& other, const Allocator& alloc)
      : map_type{other, alloc} {}

  /**
   * Initializer-list constructor.  Construct the container with the contents of
   * the initializer list init.  If multiple elements in the range have keys
   * that compare equal, it is unspecified which element is inserted.
   *
   * @param init initializer list to initialize the elements of the container
   *             with
   * @param alloc allocator to use for all memory allocations of this container
   */
  StringMap(std::initializer_list<value_type> init,
            const Allocator& alloc = Allocator())
      : map_type{init, alloc} {}

  /** Copy assignment operator. */
  StringMap& operator=(const StringMap&) = default;

  /** Move assignment operator. */
  StringMap& operator=(StringMap&&) noexcept(
      std::allocator_traits<Allocator>::is_always_equal::value &&
      std::is_nothrow_move_assignable<std::less<>>::value) = default;

  /**
   * Replaces the contents with those identified by initializer list ilist.
   *
   * @param ilist initializer list to use as data source
   */
  StringMap& operator=(std::initializer_list<value_type> ilist) {
    map_type::operator=(ilist);
    return *this;
  }

  /**
   * Returns a reference to the mapped value of the element with the specified
   * key.  If no such element exists, an exception of type std::out_of_range is
   * thrown.
   *
   * @param key the key of the element to find
   * @return A reference to the mapped value of the requested element.
   */
  T& at(const std::string& key) { return map_type::at(key); }

  /**
   * Returns a reference to the mapped value of the element with the specified
   * key.  If no such element exists, an exception of type std::out_of_range is
   * thrown.
   *
   * @param key the key of the element to find
   * @return A reference to the mapped value of the requested element.
   */
  const T& at(const std::string& key) const { return map_type::at(key); }

  /**
   * Returns a reference to the mapped value of the element with the specified
   * key.  If no such element exists, an exception of type std::out_of_range is
   * thrown.
   *
   * @param key the key of the element to find
   * @return A reference to the mapped value of the requested element.
   */
  T& at(const char* key) { return at(std::string_view{key}); }

  /**
   * Returns a reference to the mapped value of the element with the specified
   * key.  If no such element exists, an exception of type std::out_of_range is
   * thrown.
   *
   * @param key the key of the element to find
   * @return A reference to the mapped value of the requested element.
   */
  const T& at(const char* key) const { return at(std::string_view{key}); }

  /**
   * Returns a reference to the mapped value of the element with the specified
   * key.  If no such element exists, an exception of type std::out_of_range is
   * thrown.
   *
   * @param key the key of the element to find
   * @return A reference to the mapped value of the requested element.
   */
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

  /**
   * Returns a reference to the mapped value of the element with the specified
   * key.  If no such element exists, an exception of type std::out_of_range is
   * thrown.
   *
   * @param key the key of the element to find
   * @return A reference to the mapped value of the requested element.
   */
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

  /**
   * Returns a reference to the value that is mapped to a key, performing an
   * insertion if such a key does not exist.
   *
   * @param key the key of the element to find
   * @return A reference to the mapped value of the new element if no element
   *         with key key existed.  Otherwise, a reference to the mapped value
   *         of the existing element whose key is equal to key.
   */
  T& operator[](const std::string& key) { return map_type::operator[](key); }

  /**
   * Returns a reference to the value that is mapped to a key, performing an
   * insertion if such a key does not exist.
   *
   * @param key the key of the element to find
   * @return A reference to the mapped value of the new element if no element
   *         with key key existed.  Otherwise, a reference to the mapped value
   *         of the existing element whose key is equal to key.
   */
  T& operator[](std::string&& key) {
    return map_type::operator[](std::move(key));
  }

  /**
   * Returns a reference to the value that is mapped to a key, performing an
   * insertion if such a key does not exist.
   *
   * @param key the key of the element to find
   * @return A reference to the mapped value of the new element if no element
   *         with key key existed.  Otherwise, a reference to the mapped value
   *         of the existing element whose key is equal to key.
   */
  T& operator[](const char* key) { return operator[](std::string_view{key}); }

  /**
   * Returns a reference to the value that is mapped to a key, performing an
   * insertion if such a key does not exist.
   *
   * @param key the key of the element to find
   * @return A reference to the mapped value of the new element if no element
   *         with key key existed.  Otherwise, a reference to the mapped value
   *         of the existing element whose key is equal to key.
   */
  T& operator[](std::string_view key) { return try_emplace(key).first->second; }

  /**
   * If a key equal to key already exists in the container, assigns obj to the
   * mapped type corresponding to that key.  If the key does not exist, inserts
   * the value as if by calling insert().
   *
   * @param key the key used both to look up and to insert if not found
   * @param obj the value to insert or assign
   * @return The bool component is true if the insertion took place and false if
   *         the assignment took place.  The iterator component is pointing at
   *         the element that was inserted or updated.
   */
  template <typename M>
  std::pair<iterator, bool> insert_or_assign(std::string&& key, M&& obj) {
    return map_type::insert_or_assign(std::move(key), std::forward<M>(obj));
  }

  /**
   * If a key equal to key already exists in the container, assigns obj to the
   * mapped type corresponding to that key.  If the key does not exist, inserts
   * the value as if by calling insert().
   *
   * @param key the key used both to look up and to insert if not found
   * @param obj the value to insert or assign
   * @return The bool component is true if the insertion took place and false if
   *         the assignment took place.  The iterator component is pointing at
   *         the element that was inserted or updated.
   */
  template <typename M>
  std::pair<iterator, bool> insert_or_assign(const char* key, M&& obj) {
    return insert_or_assign(std::string_view{key}, std::forward<M>(obj));
  }

  /**
   * If a key equal to key already exists in the container, assigns obj to the
   * mapped type corresponding to that key.  If the key does not exist, inserts
   * the value as if by calling insert().
   *
   * @param key the key used both to look up and to insert if not found
   * @param obj the value to insert or assign
   * @return The bool component is true if the insertion took place and false if
   *         the assignment took place.  The iterator component is pointing at
   *         the element that was inserted or updated.
   */
  template <typename M>
  std::pair<iterator, bool> insert_or_assign(std::string_view key, M&& obj) {
#ifdef __cpp_lib_associative_heterogeneous_insertion
    return map_type::insert_or_assign(key, std::forward<M>(obj));
#else
    auto it = lower_bound(key);
    if (it != this->end() && it->first == key) {
      it->second = std::forward<M>(obj);
      return {it, false};
    } else {
      return {this->insert(it, {std::string{key}, std::forward<M>(obj)}), true};
    }
#endif
  }

  /**
   * If a key equal to key already exists in the container, assigns obj to the
   * mapped type corresponding to that key.  If the key does not exist, inserts
   * the value as if by calling insert().
   *
   * @param hint iterator to the position before which the new element will be
   *             inserted
   * @param key the key used both to look up and to insert if not found
   * @param obj the value to insert or assign
   * @return Iterator pointing at the element that was inserted or updated.
   */
  template <typename M>
  iterator insert_or_assign(const_iterator hint, std::string&& key, M&& obj) {
    return map_type::insert_or_assign(hint, std::move(key),
                                      std::forward<M>(obj));
  }

  /**
   * If a key equal to key already exists in the container, assigns obj to the
   * mapped type corresponding to that key.  If the key does not exist, inserts
   * the value as if by calling insert().
   *
   * @param hint iterator to the position before which the new element will be
   *             inserted
   * @param key the key used both to look up and to insert if not found
   * @param obj the value to insert or assign
   * @return Iterator pointing at the element that was inserted or updated.
   */
  template <typename M>
  iterator insert_or_assign(const_iterator hint, const char* key, M&& obj) {
    return insert_or_assign(hint, std::string_view{key}, std::forward<M>(obj));
  }

  /**
   * If a key equal to key already exists in the container, assigns obj to the
   * mapped type corresponding to that key.  If the key does not exist, inserts
   * the value as if by calling insert().
   *
   * @param hint iterator to the position before which the new element will be
   *             inserted
   * @param key the key used both to look up and to insert if not found
   * @param obj the value to insert or assign
   * @return Iterator pointing at the element that was inserted or updated.
   */
  template <typename M>
  iterator insert_or_assign(const_iterator hint, std::string_view key,
                            M&& obj) {
#ifdef __cpp_lib_associative_heterogeneous_insertion
    return map_type::insert_or_assign(hint, key, std::forward<M>(obj));
#else
    return map_type::insert_or_assign(hint, std::string{key},
                                      std::forward<M>(obj));
#endif
  }

  /**
   * Inserts a new element into the container constructed in-place with the
   * given args, if there is no element with the key in the container.
   *
   * No iterators or references are invalidated.
   *
   * @param key the key used both to look up and to insert if not found
   * @param args arguments to forward to the constructor of the element
   * @return A pair consisting of an interator to the inserted element (or to
   *         the element that prevented the insertion) and a bool value set to
   *         true if and only if the insertion took place.
   */
  template <typename... Args>
  std::pair<iterator, bool> emplace(std::string&& key, Args&&... args) {
    return map_type::emplace(std::move(key), std::forward<Args>(args)...);
  }

  /**
   * Inserts a new element into the container constructed in-place with the
   * given args, if there is no element with the key in the container.
   *
   * No iterators or references are invalidated.
   *
   * @param key the key used both to look up and to insert if not found
   * @param args arguments to forward to the constructor of the element
   * @return A pair consisting of an interator to the inserted element (or to
   *         the element that prevented the insertion) and a bool value set to
   *         true if and only if the insertion took place.
   */
  template <typename... Args>
  std::pair<iterator, bool> emplace(const char* key, Args&&... args) {
    return emplace(std::string_view{key}, std::forward<Args>(args)...);
  }

  /**
   * Inserts a new element into the container constructed in-place with the
   * given args, if there is no element with the key in the container.
   *
   * No iterators or references are invalidated.
   *
   * @param key the key used both to look up and to insert if not found
   * @param args arguments to forward to the constructor of the element
   * @return A pair consisting of an interator to the inserted element (or to
   *         the element that prevented the insertion) and a bool value set to
   *         true if and only if the insertion took place.
   */
  template <typename... Args>
  std::pair<iterator, bool> emplace(std::string_view key, Args&&... args) {
    return try_emplace(key, std::forward<Args>(args)...);
  }

  /**
   * If a key equal to key already exists in the container, does nothing.
   * Otherwise, inserts a new element into the container with key key and value
   * constructed with args.
   *
   * @param key the key used both to look up and to insert if not found
   * @param args arguments to forward to the constructor of the element
   * @return A pair consisting of an interator to the inserted element (or to
   *         the element that prevented the insertion) and a bool value set to
   *         true if and only if the insertion took place.
   */
  template <typename... Args>
  std::pair<iterator, bool> try_emplace(std::string&& key, Args&&... args) {
    return map_type::try_emplace(std::move(key), std::forward<Args>(args)...);
  }

  /**
   * If a key equal to key already exists in the container, does nothing.
   * Otherwise, inserts a new element into the container with key key and value
   * constructed with args.
   *
   * @param key the key used both to look up and to insert if not found
   * @param args arguments to forward to the constructor of the element
   * @return A pair consisting of an interator to the inserted element (or to
   *         the element that prevented the insertion) and a bool value set to
   *         true if and only if the insertion took place.
   */
  template <typename... Args>
  std::pair<iterator, bool> try_emplace(const char* key, Args&&... args) {
    return try_emplace(std::string_view{key}, std::forward<Args>(args)...);
  }

  /**
   * If a key equal to key already exists in the container, does nothing.
   * Otherwise, inserts a new element into the container with key key and value
   * constructed with args.
   *
   * @param key the key used both to look up and to insert if not found
   * @param args arguments to forward to the constructor of the element
   * @return A pair consisting of an interator to the inserted element (or to
   *         the element that prevented the insertion) and a bool value set to
   *         true if and only if the insertion took place.
   */
  template <typename... Args>
  std::pair<iterator, bool> try_emplace(std::string_view key, Args&&... args) {
#ifdef __cpp_lib_associative_heterogeneous_insertion
    return map_type::try_emplace(key, std::forward<Args>(args)...);
#else
    auto it = lower_bound(key);
    if (it != this->end() && it->first == key) {
      return {it, false};
    } else {
      return {try_emplace(it, key, std::forward<Args>(args)...), true};
    }
#endif
  }

  /**
   * If a key equal to key already exists in the container, does nothing.
   * Otherwise, inserts a new element into the container with key key and value
   * constructed with args.
   *
   * @param hint iterator to the position before which the new element will be
   *             inserted
   * @param key the key used both to look up and to insert if not found
   * @param args arguments to forward to the constructor of the element
   * @return An interator to the inserted element, or to the element that
   *         prevented the insertion.
   */
  template <typename... Args>
  iterator try_emplace(const_iterator hint, std::string&& key, Args&&... args) {
    return map_type::try_emplace(hint, std::move(key),
                                 std::forward<Args>(args)...);
  }

  /**
   * If a key equal to key already exists in the container, does nothing.
   * Otherwise, inserts a new element into the container with key key and value
   * constructed with args.
   *
   * @param hint iterator to the position before which the new element will be
   *             inserted
   * @param key the key used both to look up and to insert if not found
   * @param args arguments to forward to the constructor of the element
   * @return An interator to the inserted element, or to the element that
   *         prevented the insertion.
   */
  template <typename... Args>
  iterator try_emplace(const_iterator hint, const char* key, Args&&... args) {
    return try_emplace(hint, std::string_view{key},
                       std::forward<Args>(args)...);
  }

  /**
   * If a key equal to key already exists in the container, does nothing.
   * Otherwise, inserts a new element into the container with key key and value
   * constructed with args.
   *
   * @param hint iterator to the position before which the new element will be
   *             inserted
   * @param key the key used both to look up and to insert if not found
   * @param args arguments to forward to the constructor of the element
   * @return An interator to the inserted element, or to the element that
   *         prevented the insertion.
   */
  template <typename... Args>
  iterator try_emplace(const_iterator hint, std::string_view key,
                       Args&&... args) {
#ifdef __cpp_lib_associative_heterogeneous_insertion
    return map_type::try_emplace(hint, key, std::forward<Args>(args)...);
#else
    return map_type::try_emplace(hint, std::string{key},
                                 std::forward<Args>(args)...);
#endif
  }

  /**
   * Removes the element at pos.
   *
   * @param pos iterator to the element to remove
   * @return Iterator following the removed element.
   */
  iterator erase(iterator pos) { return map_type::erase(pos); }

  /**
   * Removes the element at pos.
   *
   * @param pos iterator to the element to remove
   * @return Iterator following the removed element.
   */
  iterator erase(const_iterator pos) { return map_type::erase(pos); }

  /**
   * Removes the elements in the range [first, last), which must be a valid
   * range in this.
   *
   * @param first Start of the range of elements to remove
   * @param last End of the range of elements to remove
   * @return Iterator following the last removed element.
   */
  iterator erase(iterator first, iterator last) {
    return map_type::erase(first, last);
  }

  /**
   * Removes the elements in the range [first, last), which must be a valid
   * range in this.
   *
   * @param first Start of the range of elements to remove
   * @param last End of the range of elements to remove
   * @return Iterator following the last removed element.
   */
  iterator erase(const_iterator first, const_iterator last) {
    return map_type::erase(first, last);
  }

  /**
   * Removes the element (if one exists) with the key equal to key.
   *
   * @param key key value of the elements to remove
   * @return Number of elements removed (0 or 1).
   */
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

  /**
   * Exchanges the contents of the container with those of other.  Does not
   * invoke any move, copy, or swap operations on individual elements.
   *
   * All iterators and references remain valid.  The end() iterator is
   * invalidated.
   *
   * @param other container to exchange the contents with
   */
  void swap(StringMap& other) noexcept(
      std::allocator_traits<Allocator>::is_always_equal::value &&
      std::is_nothrow_swappable<std::less<>>::value) {
    map_type::swap(other);
  }

  /**
   * Unlinks the node that contains the element pointed to by position and
   * returns a node handle that owns it.
   *
   * @param position a valid iterator into this container
   * @return A node handle that owns the extracted element
   */
  node_type extract(const_iterator position) {
    return map_type::extract(position);
  }

  /**
   * If the container has an element with key equal to key, unlinks the node
   * that contains that element from the container and returns a node handle
   * that owns it.  Otherwise, returns an empty node handle.
   *
   * @param key a key to identify the node to be extracted
   * @return A node handle that owns the extracted element, or empty node handle
   *         in case the element is not found.
   */
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

  /**
   * Returns the number of elements with key that equals the specified argument.
   *
   * @param key key value of the elements to count
   * @return Number of elements with key that equals key (either 0 or 1).
   */
  size_type count(std::string_view key) const { return map_type::count(key); }

  /**
   * Finds an element with key equal to key.
   *
   * @param key key value of the element to search for
   * @return An iterator to the requested element.  If no such element is found,
   *         past-the-end (see end()) iterator is returned.
   */
  iterator find(std::string_view key) { return map_type::find(key); }

  /**
   * Finds an element with key equal to key.
   *
   * @param key key value of the element to search for
   * @return An iterator to the requested element.  If no such element is found,
   *         past-the-end (see end()) iterator is returned.
   */
  const_iterator find(std::string_view key) const {
    return map_type::find(key);
  }

  /**
   * Checks if there is an element with key equal to key in the container.
   *
   * @param key key value of the element to search for
   * @return true if there is such an element, otherwise false
   */
  bool contains(std::string_view key) const { return map_type::contains(key); }

  /**
   * Returns a range containing all elements with the given key in the
   * container.  The range is defined by two iterators, one pointing to the
   * first element that is not less than key and another pointing to the first
   * element greater than key.  Alternatively, the first iterator may be
   * obtained with lower_bound(), and the second with upper_bound().
   *
   * @param key key value to compare the elements to
   * @return std::pair containing a pair of iterators defining the wanted range:
   *         the first pointing to the first element that is not less than key
   *         and the second pointing to the first element greater than key.  If
   *         there are no elements not less than key, past-the-end (see end())
   *         iterator is returned as the first element.  Similarly if there are
   *         no elements greater than key, past-the-end iterator is returned as
   *         the second element.
   */
  std::pair<iterator, iterator> equal_range(std::string_view key) {
    return map_type::equal_range(key);
  }

  /**
   * Returns a range containing all elements with the given key in the
   * container.  The range is defined by two iterators, one pointing to the
   * first element that is not less than key and another pointing to the first
   * element greater than key.  Alternatively, the first iterator may be
   * obtained with lower_bound(), and the second with upper_bound().
   *
   * @param key key value to compare the elements to
   * @return std::pair containing a pair of iterators defining the wanted range:
   *         the first pointing to the first element that is not less than key
   *         and the second pointing to the first element greater than key.  If
   *         there are no elements not less than key, past-the-end (see end())
   *         iterator is returned as the first element.  Similarly if there are
   *         no elements greater than key, past-the-end iterator is returned as
   *         the second element.
   */
  std::pair<const_iterator, const_iterator> equal_range(
      std::string_view key) const {
    return map_type::equal_range(key);
  }

  /**
   * Returns an iterator pointing to the first element that is not less than
   * (i.e. greater or equal to) key.
   *
   * @param key key value to compare the elements to
   * @return Iterator pointing to the first element that is not less than key.
   *         If no such element is found, a past-the-end iterator (see end()) is
   *         returned.
   */
  iterator lower_bound(std::string_view key) {
    return map_type::lower_bound(key);
  }

  /**
   * Returns an iterator pointing to the first element that is not less than
   * (i.e. greater or equal to) key.
   *
   * @param key key value to compare the elements to
   * @return Iterator pointing to the first element that is not less than key.
   *         If no such element is found, a past-the-end iterator (see end()) is
   *         returned.
   */
  const_iterator lower_bound(std::string_view key) const {
    return map_type::lower_bound(key);
  }

  /**
   * Returns an interator pointing to the first element that is greater than
   * key.
   *
   * @param key key value to compare the elements to
   * @return Iterator pointing to the first element that is greater than key.
   *         If no such element is found, past-the-end (see end()) iterator is
   *         returned.
   */
  iterator upper_bound(std::string_view key) {
    return map_type::upper_bound(key);
  }

  /**
   * Returns an interator pointing to the first element that is greater than
   * key.
   *
   * @param key key value to compare the elements to
   * @return Iterator pointing to the first element that is greater than key.
   *         If no such element is found, past-the-end (see end()) iterator is
   *         returned.
   */
  const_iterator upper_bound(std::string_view key) const {
    return map_type::upper_bound(key);
  }
};

}  // namespace wpi

namespace std {
template <typename T>
inline void swap(wpi::StringMap<T>& lhs, wpi::StringMap<T>& rhs) {
  lhs.swap(rhs);
}
}  // namespace std
