#if __has_include(<span>)
#include <span>
#endif
#include "wpi/span.h"

#include "gtest/gtest.h"

void func1(wpi::span<const int> x) {}
#ifdef __cpp_lib_span
void func2(std::span<const int> x) {}
#endif

void func4(wpi::span<int> x) {}
#ifdef __cpp_lib_span
void func5(std::span<int> x) {}
#endif

TEST(Span, ConvertStdSpan) {
  func1(wpi::span<const int>{});
  func1(wpi::span<int>{});
#ifdef __cpp_lib_span
  func1(std::span<const int>{});
  func1(std::span<int>{});
#endif

#ifdef __cpp_lib_span
  func2(wpi::span<const int>{});
  func2(wpi::span<int>{});
  func2(std::span<const int>{});
  func2(std::span<int>{});
#endif

  //func4(wpi::span<const int>{});
  func4(wpi::span<int>{});
#ifdef __cpp_lib_span
  //func4(std::span<const int>{});
  func4(std::span<int>{});
#endif

#ifdef __cpp_lib_span
  //func5(wpi::span<const int>{});
  func5(wpi::span<int>{});
  //func5(std::span<const int>{});
  func5(std::span<int>{});
#endif
}
