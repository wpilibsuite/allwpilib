
/*
This is an implementation of C++20's std::span
http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2019/n4820.pdf
*/

//          Copyright Tristan Brindle 2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef WPIUTIL_WPI_SPAN_HPP_INCLUDED
#define WPIUTIL_WPI_SPAN_HPP_INCLUDED

#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <type_traits>
#if __cplusplus >= 202002L && __has_include(<span>)
#include <span>
#endif

namespace wpi {

inline constexpr std::size_t dynamic_extent = SIZE_MAX;

template <typename ElementType, std::size_t Extent = dynamic_extent>
class span;

namespace detail {

template <typename E, std::size_t S>
struct span_storage {
    constexpr span_storage() noexcept = default;

    constexpr span_storage(E* p_ptr, std::size_t /*unused*/) noexcept
       : ptr(p_ptr)
    {}

    E* ptr = nullptr;
    static constexpr std::size_t size = S;
};

template <typename E>
struct span_storage<E, dynamic_extent> {
    constexpr span_storage() noexcept = default;

    constexpr span_storage(E* p_ptr, std::size_t p_size) noexcept
        : ptr(p_ptr), size(p_size)
    {}

    E* ptr = nullptr;
    std::size_t size = 0;
};

template <typename T>
using uncvref_t =
    typename std::remove_cv<typename std::remove_reference<T>::type>::type;

template <typename>
struct is_span : std::false_type {};

template <typename T, std::size_t S>
struct is_span<span<T, S>> : std::true_type {};

template <typename>
struct is_std_array : std::false_type {};

template <typename T, std::size_t N>
struct is_std_array<std::array<T, N>> : std::true_type {};

template <typename, typename = void>
struct has_size_and_data : std::false_type {};

template <typename T>
struct has_size_and_data<T, std::void_t<decltype(std::size(std::declval<T>())),
                                   decltype(std::data(std::declval<T>()))>>
    : std::true_type {};

template <typename C, typename U = uncvref_t<C>>
struct is_container {
    static constexpr bool value =
        !is_span<U>::value && !is_std_array<U>::value &&
        !std::is_array<U>::value && has_size_and_data<C>::value;
};

template <typename T>
using remove_pointer_t = typename std::remove_pointer<T>::type;

template <typename, typename, typename = void>
struct is_container_element_type_compatible : std::false_type {};

template <typename T, typename E>
struct is_container_element_type_compatible<
    T, E,
    typename std::enable_if<
        !std::is_same<typename std::remove_cv<decltype(
                          std::data(std::declval<T>()))>::type,
                      void>::value>::type>
    : std::is_convertible<
          remove_pointer_t<decltype(std::data(std::declval<T>()))> (*)[],
          E (*)[]> {};

template <typename, typename = size_t>
struct is_complete : std::false_type {};

template <typename T>
struct is_complete<T, decltype(sizeof(T))> : std::true_type {};

} // namespace detail

template <typename ElementType, std::size_t Extent>
class span {
    static_assert(std::is_object<ElementType>::value,
                  "A span's ElementType must be an object type (not a "
                  "reference type or void)");
    static_assert(detail::is_complete<ElementType>::value,
                  "A span's ElementType must be a complete type (not a forward "
                  "declaration)");
    static_assert(!std::is_abstract<ElementType>::value,
                  "A span's ElementType cannot be an abstract class type");

    using storage_type = detail::span_storage<ElementType, Extent>;

public:
    // constants and types
    using element_type = ElementType;
    using value_type = typename std::remove_cv<ElementType>::type;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using pointer = element_type*;
    using const_pointer = const element_type*;
    using reference = element_type&;
    using const_reference = const element_type&;
    using iterator = pointer;
    using reverse_iterator = std::reverse_iterator<iterator>;

    static constexpr size_type extent = Extent;

    // [span.cons], span constructors, copy, assignment, and destructor
    template <
        std::size_t E = Extent,
        typename std::enable_if<(E == dynamic_extent || E <= 0), int>::type = 0>
    constexpr span() noexcept
    {}

    constexpr span(pointer ptr, size_type count)
        : storage_(ptr, count)
    {
        assert(extent == dynamic_extent || count == extent);
    }

    constexpr span(pointer first_elem, pointer last_elem)
        : storage_(first_elem, last_elem - first_elem)
    {
        assert(extent == dynamic_extent ||
                        last_elem - first_elem ==
                            static_cast<std::ptrdiff_t>(extent));
    }

    template <std::size_t N, std::size_t E = Extent,
              typename std::enable_if<
                  (E == dynamic_extent || N == E) &&
                      detail::is_container_element_type_compatible<
                          element_type (&)[N], ElementType>::value,
                  int>::type = 0>
    constexpr span(element_type (&arr)[N]) noexcept : storage_(arr, N)
    {}

    template <std::size_t N, std::size_t E = Extent,
              typename std::enable_if<
                  (E == dynamic_extent || N == E) &&
                      detail::is_container_element_type_compatible<
                          std::array<value_type, N>&, ElementType>::value,
                  int>::type = 0>
    constexpr span(std::array<value_type, N>& arr) noexcept
        : storage_(arr.data(), N)
    {}

    template <std::size_t N, std::size_t E = Extent,
              typename std::enable_if<
                  (E == dynamic_extent || N == E) &&
                      detail::is_container_element_type_compatible<
                          const std::array<value_type, N>&, ElementType>::value,
                  int>::type = 0>
    constexpr span(const std::array<value_type, N>& arr) noexcept
        : storage_(arr.data(), N)
    {}

    template <
        typename Container, std::size_t E = Extent,
        typename std::enable_if<
            E == dynamic_extent && detail::is_container<Container>::value &&
                detail::is_container_element_type_compatible<
                    Container&, ElementType>::value,
            int>::type = 0>
    constexpr span(Container& cont)
        : storage_(std::data(cont), std::size(cont))
    {}

    template <
        typename Container, std::size_t E = Extent,
        typename std::enable_if<
            E == dynamic_extent && detail::is_container<Container>::value &&
                detail::is_container_element_type_compatible<
                    const Container&, ElementType>::value,
            int>::type = 0>
    constexpr span(const Container& cont)
        : storage_(std::data(cont), std::size(cont))
    {}

    constexpr span(const span& other) noexcept = default;

    template <typename OtherElementType, std::size_t OtherExtent,
              typename std::enable_if<
                  (Extent == OtherExtent || Extent == dynamic_extent) &&
                      std::is_convertible<OtherElementType (*)[],
                                          ElementType (*)[]>::value,
                  int>::type = 0>
    constexpr span(const span<OtherElementType, OtherExtent>& other) noexcept
        : storage_(other.data(), other.size())
    {}

#ifdef __cpp_lib_span
    constexpr span(std::span<ElementType> other) noexcept
        : storage_(other.data(), other.size())
    {}
#endif

    ~span() noexcept = default;

    constexpr span&
    operator=(const span& other) noexcept = default;

    // [span.sub], span subviews
    template <std::size_t Count>
    constexpr span<element_type, Count> first() const
    {
        assert(Count <= size());
        return {data(), Count};
    }

    template <std::size_t Count>
    constexpr span<element_type, Count> last() const
    {
        assert(Count <= size());
        return {data() + (size() - Count), Count};
    }

    template <std::size_t Offset, std::size_t Count = dynamic_extent>
    using subspan_return_t =
        span<ElementType, Count != dynamic_extent
                              ? Count
                              : (Extent != dynamic_extent ? Extent - Offset
                                                          : dynamic_extent)>;

    template <std::size_t Offset, std::size_t Count = dynamic_extent>
    constexpr subspan_return_t<Offset, Count> subspan() const
    {
        assert(Offset <= size() &&
                        (Count == dynamic_extent || Offset + Count <= size()));
        return {data() + Offset,
                Count != dynamic_extent ? Count : size() - Offset};
    }

    constexpr span<element_type, dynamic_extent>
    first(size_type count) const
    {
        assert(count <= size());
        return {data(), count};
    }

    constexpr span<element_type, dynamic_extent>
    last(size_type count) const
    {
        assert(count <= size());
        return {data() + (size() - count), count};
    }

    constexpr span<element_type, dynamic_extent>
    subspan(size_type offset, size_type count = dynamic_extent) const
    {
        assert(offset <= size() &&
                        (count == dynamic_extent || offset + count <= size()));
        return {data() + offset,
                count == dynamic_extent ? size() - offset : count};
    }

    // [span.obs], span observers
    constexpr size_type size() const noexcept { return storage_.size; }

    constexpr size_type size_bytes() const noexcept
    {
        return size() * sizeof(element_type);
    }

    [[nodiscard]] constexpr bool empty() const noexcept
    {
        return size() == 0;
    }

    // [span.elem], span element access
    constexpr reference operator[](size_type idx) const
    {
        assert(idx < size());
        return *(data() + idx);
    }

    constexpr reference front() const
    {
        assert(!empty());
        return *data();
    }

    constexpr reference back() const
    {
        assert(!empty());
        return *(data() + (size() - 1));
    }

    constexpr pointer data() const noexcept { return storage_.ptr; }

    // [span.iterators], span iterator support
    constexpr iterator begin() const noexcept { return data(); }

    constexpr iterator end() const noexcept { return data() + size(); }

    constexpr reverse_iterator rbegin() const noexcept
    {
        return reverse_iterator(end());
    }

    constexpr reverse_iterator rend() const noexcept
    {
        return reverse_iterator(begin());
    }

#ifdef __cpp_lib_span
    constexpr operator auto() const {
      return std::span < ElementType,
             (Extent == dynamic_extent)
                 ? std::dynamic_extent
                 : Extent > (storage_.ptr, storage_.size);
    }
#endif

private:
    storage_type storage_{};
};

/* Deduction Guides */
template <class T, size_t N>
span(T (&)[N])->span<T, N>;

template <class T, size_t N>
span(std::array<T, N>&)->span<T, N>;

template <class T, size_t N>
span(const std::array<T, N>&)->span<const T, N>;

template <class Container>
span(Container&)->span<typename Container::value_type>;

template <class Container>
span(const Container&)->span<const typename Container::value_type>;

template <typename ElementType, std::size_t Extent>
span<const std::byte, ((Extent == dynamic_extent) ? dynamic_extent
                                             : sizeof(ElementType) * Extent)>
as_bytes(span<ElementType, Extent> s) noexcept
{
    return {reinterpret_cast<const std::byte*>(s.data()), s.size_bytes()};
}

template <
    class ElementType, size_t Extent,
    typename std::enable_if<!std::is_const<ElementType>::value, int>::type = 0>
span<std::byte, ((Extent == dynamic_extent) ? dynamic_extent
                                       : sizeof(ElementType) * Extent)>
as_writable_bytes(span<ElementType, Extent> s) noexcept
{
    return {reinterpret_cast<std::byte*>(s.data()), s.size_bytes()};
}

template <std::size_t N, typename E, std::size_t S>
constexpr auto get(span<E, S> s) -> decltype(s[N])
{
    return s[N];
}

} // namespace wpi

namespace std {

template <typename ElementType, size_t Extent>
class tuple_size<wpi::span<ElementType, Extent>>
    : public integral_constant<size_t, Extent> {};

template <typename ElementType>
class tuple_size<wpi::span<
    ElementType, wpi::dynamic_extent>>; // not defined

template <size_t I, typename ElementType, size_t Extent>
class tuple_element<I, wpi::span<ElementType, Extent>> {
public:
    static_assert(Extent != wpi::dynamic_extent &&
                      I < Extent,
                  "");
    using type = ElementType;
};

} // end namespace std

#endif // WPIUTIL_WPI_SPAN_HPP_INCLUDED
