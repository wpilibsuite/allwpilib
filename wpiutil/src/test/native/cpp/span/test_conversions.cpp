#if __has_include(<span>)
#include <span>
#endif
#include "wpi/ArrayRef.h"

#include "gtest/gtest.h"

void func1(wpi::span<const int> x) {}
#ifdef __cpp_lib_span
void func2(std::span<const int> x) {}
#endif
void func3(wpi::ArrayRef<int> x) {} 

void func4(wpi::span<int> x) {}
#ifdef __cpp_lib_span
void func5(std::span<int> x) {}
#endif
void func6(wpi::MutableArrayRef<int> x) {} 

TEST(Span, ConvertArrayRefStdSpan) {
  func1(wpi::ArrayRef<int>{});
  func1(wpi::MutableArrayRef<int>{});
  func1(wpi::span<const int>{});
  func1(wpi::span<int>{});
#ifdef __cpp_lib_span
  func1(std::span<const int>{});
  func1(std::span<int>{});
#endif

#ifdef __cpp_lib_span
  func2(wpi::ArrayRef<int>{});
  func2(wpi::MutableArrayRef<int>{});
  func2(wpi::span<const int>{});
  func2(wpi::span<int>{});
  func2(std::span<const int>{});
  func2(std::span<int>{});
#endif

  func3(wpi::ArrayRef<int>{});
  func3(wpi::MutableArrayRef<int>{});
  func3(wpi::span<const int>{});
  func3(wpi::span<int>{});
#ifdef __cpp_lib_span
  func3(std::span<const int>{});
  func3(std::span<int>{});
#endif

  //func4(wpi::ArrayRef<int>{});
  func4(wpi::MutableArrayRef<int>{});
  //func4(wpi::span<const int>{});
  func4(wpi::span<int>{});
#ifdef __cpp_lib_span
  //func4(std::span<const int>{});
  func4(std::span<int>{});
#endif

#ifdef __cpp_lib_span
  //func5(wpi::ArrayRef<int>{});
  func5(wpi::MutableArrayRef<int>{});
  //func5(wpi::span<const int>{});
  func5(wpi::span<int>{});
  //func5(std::span<const int>{});
  func5(std::span<int>{});
#endif

  //func6(wpi::ArrayRef<int>{});
  func6(wpi::MutableArrayRef<int>{});
  //func6(wpi::span<const int>{});
  func6(wpi::span<int>{});
#ifdef __cpp_lib_span
  //func6(std::span<const int>{});
  func6(std::span<int>{});
#endif
}
