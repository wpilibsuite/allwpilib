// From:
// http://stackoverflow.com/questions/21028299/is-this-behavior-of-vectorresizesize-type-n-under-c11-and-boost-container
// Credits: Casey and Howard Hinnant

#ifndef CSCORE_DEFAULT_INIT_ALLOCATOR_H_
#define CSCORE_DEFAULT_INIT_ALLOCATOR_H_

#include <memory>
#include <utility>

namespace cs {

// Allocator adaptor that interposes construct() calls to
// convert value initialization into default initialization.
template <typename T, typename A = std::allocator<T>>
class default_init_allocator : public A {
  using a_t = std::allocator_traits<A>;

 public:
  template <typename U>
  struct rebind {
    using other =
        default_init_allocator<U, typename a_t::template rebind_alloc<U>>;
  };

  using A::A;

  template <typename U>
  void construct(U* ptr) noexcept(
      std::is_nothrow_default_constructible<U>::value) {
    ::new (static_cast<void*>(ptr)) U;
  }
  template <typename U, typename... Args>
  void construct(U* ptr, Args&&... args) {
    a_t::construct(static_cast<A&>(*this), ptr, std::forward<Args>(args)...);
  }
};

}  // namespace cs

#endif  // CSCORE_DEFAULT_INIT_ALLOCATOR_H_
