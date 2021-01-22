// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef WPIUTIL_WPI_PRIORITY_QUEUE_H_
#define WPIUTIL_WPI_PRIORITY_QUEUE_H_

#include <algorithm>
#include <functional>
#include <utility>
#include <vector>

namespace wpi {

/**
 * This class is the same as std::priority_queue with two changes:
 *
 * 1. Adds a remove() function for removing all elements from the priority queue
 *    that match the given value.
 * 2. Replaces "void pop()" with "T pop()" so the element can be moved from the
 *    queue directly instead of copied from top().
 */
template <typename T, typename Sequence = std::vector<T>,
          typename Compare = std::less<typename Sequence::value_type>>
class priority_queue {
 public:
  static_assert(std::is_same_v<T, typename Sequence::value_type>,
                "value_type must be the same as the underlying container");

  using value_type = typename Sequence::value_type;
  using reference = typename Sequence::reference;
  using const_reference = typename Sequence::const_reference;
  using size_type = typename Sequence::size_type;
  using container_type = Sequence;
  using value_compare = Compare;

  template <typename Seq = Sequence,
            typename Requires = typename std::enable_if_t<
                std::is_default_constructible<Compare>{} &&
                std::is_default_constructible<Seq>{}>>
  priority_queue() {}

  priority_queue(const Compare& comp, const Sequence& c) : c(c), comp(comp) {
    std::make_heap(c.begin(), c.end(), comp);
  }

  explicit priority_queue(const Compare& comp, Sequence&& c = Sequence{})
      : c(std::move(c)), comp(comp) {
    std::make_heap(c.begin(), c.end(), comp);
  }

  template <typename InputIterator>
  priority_queue(InputIterator first, InputIterator last, const Compare& comp,
                 const Sequence& c)
      : c(c), comp(comp) {
    c.insert(c.end(), first, last);
    std::make_heap(c.begin(), c.end(), comp);
  }

  template <typename InputIterator>
  priority_queue(InputIterator first, InputIterator last,
                 const Compare& comp = Compare{}, Sequence&& c = Sequence{})
      : c(std::move(c)), comp(comp) {
    c.insert(c.end(), first, last);
    std::make_heap(c.begin(), c.end(), comp);
  }

  [[nodiscard]] bool empty() const { return c.empty(); }

  size_type size() const { return c.size(); }

  const_reference top() const { return c.front(); }

  void push(const value_type& value) {
    c.push_back(value);
    std::push_heap(c.begin(), c.end(), comp);
  }

  void push(value_type&& value) {
    c.push_back(std::move(value));
    std::push_heap(c.begin(), c.end(), comp);
  }

  template <typename... Args>
  void emplace(Args&&... args) {
    c.emplace_back(std::forward<Args>(args)...);
    std::push_heap(c.begin(), c.end(), comp);
  }

  T pop() {
    std::pop_heap(c.begin(), c.end(), comp);
    auto ret = std::move(c.back());
    c.pop_back();
    return ret;
  }

  bool remove(const T& value) {
    auto it = std::find(c.begin(), c.end(), value);
    if (it != this->c.end()) {
      c.erase(it);
      std::make_heap(c.begin(), c.end(), comp);
      return true;
    } else {
      return false;
    }
  }

 protected:
  Sequence c;
  Compare comp;
};

}  // namespace wpi

#endif  // WPIUTIL_WPI_PRIORITY_QUEUE_H_
