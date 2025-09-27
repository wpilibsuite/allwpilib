
#include <pybind11/operators.h>
#include <wpystruct.h>

//
// Thing to serialize
//

struct ThingA {
  ThingA() = default;
  ThingA(int x) : x(x) {}

  const int x = 0;

  bool operator==(const ThingA &other) const { return x == other.x; }
};

template <> struct wpi::Struct<ThingA> {
  static constexpr std::string_view GetTypeName() { return "ThingA"; }
  static constexpr size_t GetSize() { return 1; }
  static constexpr std::string_view GetSchema() { return "uint8 value"; }
  static ThingA Unpack(std::span<const uint8_t> data) {
    return ThingA{data[0]};
  }
  static void Pack(std::span<uint8_t> data, const ThingA &value) {
    data[0] = value.x;
  }
};

struct Outer {
  Outer() = default;
  Outer(const ThingA &t, int c) : inner(t), c(c) {}

  ThingA inner;
  int c = 0;

  bool operator==(const Outer &other) const {
    return inner == other.inner && c == other.c;
  }
};

template <>
struct wpi::Struct<Outer> {
  static constexpr std::string_view GetTypeName() { return "Outer"; }
  static constexpr size_t GetSize() { return wpi::GetStructSize<ThingA>() + 4; }
  static constexpr std::string_view GetSchema() {
    return "ThingA inner; int32 c";
  }

  static Outer Unpack(std::span<const uint8_t> data) {
    constexpr size_t innerSize = wpi::GetStructSize<ThingA>();
    return {wpi::UnpackStruct<ThingA, 0>(data),
            wpi::UnpackStruct<int32_t, innerSize>(data)};
  }
  static void Pack(std::span<uint8_t> data, const Outer& value) {
    constexpr size_t innerSize = wpi::GetStructSize<ThingA>();
    wpi::PackStruct<0>(data, value.inner);
    wpi::PackStruct<innerSize>(data, value.c);
  }
  static void ForEachNested(
      std::invocable<std::string_view, std::string_view> auto fn) {
    wpi::ForEachStructSchema<ThingA>(fn);
  }
};

void struct_test(py::module &m) {

  py::class_<ThingA> thingCls(m, "ThingA");
  thingCls.def(py::init<>());
  thingCls.def(py::init<int>());
  thingCls.def_readonly("x", &ThingA::x);
  thingCls.def(py::self == py::self);

  SetupWPyStruct<ThingA>(thingCls);

  py::class_<Outer> outerCls(m, "Outer");
  outerCls.def(py::init<>());
  outerCls.def(py::init<ThingA, int>());
  outerCls.def_readonly("inner", &Outer::inner);
  outerCls.def_readwrite("c", &Outer::c);
  outerCls.def(py::self == py::self);

  SetupWPyStruct<Outer>(outerCls);
}