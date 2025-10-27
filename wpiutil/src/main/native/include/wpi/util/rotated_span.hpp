// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <array>
#include <cstddef>
#include <iterator>
#include <span>
#include <type_traits>

namespace wpi {

/**
 * This is a simple rotated span view.  Indexed/iterated access provides a
 * continuous view of the underlying span that wraps at the span size.  An
 * internal offset determines the starting location.
 *
 * Constructors take a "rotation" value--if positive, the offset is the same as
 * the rotation; if negative, the offset is set relative to the end of the
 * array.
 *
 * For example, given an array of 5 values, providing a rotation value of 2
 * will result in an index of 0 accessing underlying span index 2, index 2
 * accessing underlying span index 4, and index 4 accessing underlying span
 * index 1.
 *
 * Similarly, providing a rotation value of -2 will result in index 0 accessing
 * underlying span index 3 (5-2), index 2 accessing underlying span index 0,
 * and index 4 accessing underlying span index 2.
 *
 * @tparam T element type
 * @tparam Extent static sized extent, or std::dynamic_extent
 */
template <typename T, size_t Extent = std::dynamic_extent>
class rotated_span {
 public:
  using element_type = T;
  using value_type = std::remove_cv_t<T>;
  using size_type = size_t;
  using difference_type = std::ptrdiff_t;
  using pointer = T*;
  using const_pointer = const T*;
  using reference = element_type&;
  using const_reference = const element_type&;

  // member constants
  static constexpr size_t extent = Extent;

  // constructors, copy and assignment

  constexpr rotated_span() noexcept
    requires(Extent == std::dynamic_extent || Extent == 0)
  = default;

  constexpr /*implicit*/ rotated_span(std::span<T, Extent> data,  // NOLINT
                                      int rotation = 0)
      : m_data{data}, m_offset{MakeOffset(data.size(), rotation)} {}

  template <std::contiguous_iterator It>
  constexpr explicit(extent != std::dynamic_extent)
      rotated_span(It first, size_type count, int rotation = 0) noexcept
      : rotated_span{std::span<T, Extent>{first, count}, rotation} {}

  template <std::contiguous_iterator It, std::sized_sentinel_for<It> End>
    requires(!std::is_convertible_v<End, size_type>)
  constexpr explicit(extent != std::dynamic_extent)
      rotated_span(It first, End last,
                   int rotation = 0) noexcept(noexcept(std::span<T, Extent>{
          first, last}))
      : rotated_span{std::span<T, Extent>{first, last}, rotation} {}

  template <size_t ArrayExtent>
    requires(Extent == std::dynamic_extent || ArrayExtent == Extent)
  constexpr rotated_span(  // NOLINT
      std::type_identity_t<element_type> (&arr)[ArrayExtent],
      int rotation = 0) noexcept
      : rotated_span{std::span<T, Extent>{arr}, rotation} {}

  template <typename Tp, size_t ArrayExtent>
  constexpr rotated_span(std::array<Tp, ArrayExtent>& arr,  // NOLINT
                         int rotation = 0) noexcept
      : rotated_span{std::span<T, Extent>{arr}, rotation} {}

  template <typename Tp, size_t ArrayExtent>
  constexpr rotated_span(const std::array<Tp, ArrayExtent>& arr,  // NOLINT
                         int rotation = 0) noexcept
      : rotated_span{std::span<T, Extent>{arr}, rotation} {}

  template <typename OType, size_t OExtent>
    requires(Extent == std::dynamic_extent || OExtent == std::dynamic_extent ||
             Extent == OExtent)
  constexpr explicit(extent != std::dynamic_extent &&
                     OExtent == std::dynamic_extent)
      rotated_span(const rotated_span<OType, OExtent>& other,
                   int rotation) noexcept
      : m_data{other.m_data},
        m_offset{MakeOffset(other.m_data.size(), other.m_offset + rotation)} {}

  template <typename OType, size_t OExtent>
    requires(Extent == std::dynamic_extent || OExtent == std::dynamic_extent ||
             Extent == OExtent)
  constexpr explicit(extent != std::dynamic_extent &&
                     OExtent == std::dynamic_extent) rotated_span(  // NOLINT
      const rotated_span<OType, OExtent>& other) noexcept
      : m_data{other.m_data}, m_offset{other.m_offset} {}
  constexpr rotated_span& operator=(const rotated_span&) noexcept = default;
  constexpr rotated_span(rotated_span&&) noexcept = default;
  constexpr rotated_span& operator=(rotated_span&&) noexcept = default;

  ~rotated_span() noexcept = default;

  // observers

  constexpr std::span<T, Extent> data() const noexcept { return m_data; }

  constexpr size_type offset() const noexcept { return m_offset; }

  constexpr size_type size() const noexcept { return m_data.size(); }

  constexpr size_type size_bytes() const noexcept {
    return m_data.size_bytes();
  }

  constexpr bool empty() const noexcept { return m_data.empty(); }

  // element access

  [[nodiscard]]
  constexpr reference front() const noexcept {
    return m_data[m_offset];
  }

  [[nodiscard]]
  constexpr reference back() const noexcept {
    return m_data[(m_offset + m_data.size() - 1) % m_data.size()];
  }

  [[nodiscard]]
  constexpr reference operator[](size_type idx) const noexcept {
    return m_data[(m_offset + idx) % m_data.size()];
  }

  // iterator support

  class iterator {
   public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = rotated_span::value_type;
    using difference_type = rotated_span::difference_type;
    using pointer = rotated_span::pointer;
    using reference = rotated_span::reference;

    constexpr iterator(const rotated_span* obj, size_type idx) noexcept
        : m_obj{obj}, m_idx{idx} {}

    constexpr iterator& operator++() noexcept {
      ++m_idx;
      return *this;
    }
    constexpr iterator operator++(int) noexcept {
      iterator retval = *this;
      ++(*this);
      return retval;
    }
    constexpr bool operator==(const iterator&) const = default;
    constexpr reference operator*() { return (*m_obj)[m_idx]; }

   private:
    const rotated_span* m_obj;
    size_type m_idx;
  };

  using reverse_iterator = std::reverse_iterator<iterator>;

  [[nodiscard]]
  constexpr iterator begin() const noexcept {
    return iterator(this, 0);
  }

  [[nodiscard]]
  constexpr iterator end() const noexcept {
    return iterator(this, size());
  }

  [[nodiscard]]
  constexpr reverse_iterator rbegin() const noexcept {
    return reverse_iterator(end());
  }

  [[nodiscard]]
  constexpr reverse_iterator rend() const noexcept {
    return reverse_iterator(begin());
  }

  // subviews
  [[nodiscard]]
  constexpr rotated_span rotate(int amt) const noexcept {
    return rotated_span{*this, amt};
  }

 private:
  std::span<T, Extent> m_data;
  size_type m_offset;

  static constexpr size_type MakeOffset(size_t size, int rotation) {
    if (size == 0) {
      return 0;
    }
    if (rotation >= 0) {
      return rotation % size;
    } else {
      // The usual arithmetic conversions mean that rotation is converted to
      // size_t, which is unsigned. Converting negative values to unsigned
      // integer types produces large numbers with useless remainders, so
      // instead make rotation positive before doing the modulo arithmetic.
      return size - (-rotation % size);
    }
  }
};

// deduction guides

template <typename Type, size_t ArrayExtent>
rotated_span(Type (&)[ArrayExtent]) -> rotated_span<Type, ArrayExtent>;

template <typename Type, size_t ArrayExtent>
rotated_span(std::array<Type, ArrayExtent>&) -> rotated_span<Type, ArrayExtent>;

template <std::contiguous_iterator It, typename End>
rotated_span(It, End)
    -> rotated_span<std::remove_reference_t<std::iter_reference_t<It>>>;

template <typename Type, size_t Extent>
[[nodiscard]]
inline rotated_span<const std::byte, Extent == std::dynamic_extent
                                         ? std::dynamic_extent
                                         : Extent * sizeof(Type)>
as_bytes(rotated_span<Type, Extent> sp) noexcept {
  return {std::as_bytes(sp.data()), sp.offset() * sizeof(Type)};
}

template <typename Type, size_t Extent>
  requires(!std::is_const_v<Type>)
[[nodiscard]]
inline rotated_span<std::byte, Extent == std::dynamic_extent
                                   ? std::dynamic_extent
                                   : Extent * sizeof(Type)>
as_writable_bytes(rotated_span<Type, Extent> sp) noexcept {
  return {std::as_writable_bytes(sp.data()), sp.offset() * sizeof(Type)};
}

}  // namespace wpi
