
#pragma once

#include <functional>
#include <memory>
#include <string_view>

#include <fmt/format.h>
#include <wpi/struct/Struct.h>

#include <pybind11/functional.h>
#include <pybind11/typing.h>
#include <semiwrap.h>

static inline std::string pytypename(const py::type &t) {
  return ((PyTypeObject *)t.ptr())->tp_name;
}

//
// Dynamic struct + type caster
//

// This merely holds the python object being operated on, the actual
// serialization work is done in WPyStructConverter
struct WPyStruct {

  WPyStruct() = default;

  WPyStruct(const WPyStruct &other) {
    py::gil_scoped_acquire gil;
    py = other.py;
  }

  WPyStruct &operator=(const WPyStruct &other) {
    {
      py::gil_scoped_acquire gil;
      py = other.py;
    }
    return *this;
  }

  WPyStruct(WPyStruct &&) = default;

  WPyStruct(const py::object &py) : py(py) {}

  ~WPyStruct() {
    py::gil_scoped_acquire gil;
    py.release().dec_ref();
  }

  py::object py;
};

namespace pybind11 {
namespace detail {

template <> struct type_caster<WPyStruct> {
  // TODO: wpiutil.struct.T/TV?
  PYBIND11_TYPE_CASTER(WPyStruct, const_name("object"));

  bool load(handle src, bool convert) {
    // TODO: validation?
    value.py = py::reinterpret_borrow<py::object>(src);
    return true;
  }

  static handle cast(const WPyStruct &src, py::return_value_policy policy,
                     py::handle parent) {
    py::handle v = src.py;
    v.inc_ref();
    return v;
  }
};

} // namespace detail
} // namespace pybind11

//
// Struct info class implementation
//

// two types of converters: static C++ converter, and dynamic python converter
struct WPyStructConverter {
  virtual ~WPyStructConverter() = default;
  virtual std::string_view GetTypeName() const = 0;

  virtual size_t GetSize() const = 0;

  virtual std::string_view GetSchema() const = 0;

  virtual void Pack(std::span<uint8_t> data, const WPyStruct &value) const = 0;

  virtual WPyStruct Unpack(std::span<const uint8_t> data) const = 0;

  // virtual void UnpackInto(WPyStruct *pyv,
  //                         std::span<const uint8_t> data) const = 0;

  virtual void ForEachNested(
      const std::function<void(std::string_view, std::string_view)> &fn)
      const = 0;
};

// static C++ converter
template <typename T> struct WPyStructCppConverter : WPyStructConverter {
  std::string_view GetTypeName() const override {
    return wpi::Struct<T>::GetTypeName();
  }

  size_t GetSize() const override { return wpi::Struct<T>::GetSize(); }

  std::string_view GetSchema() const override {
    return wpi::Struct<T>::GetSchema();
  }

  void Pack(std::span<uint8_t> data, const WPyStruct &value) const override {
    py::gil_scoped_acquire gil;
    const T &v = value.py.cast<const T &>();
    wpi::Struct<T>::Pack(data, v);
  }

  WPyStruct Unpack(std::span<const uint8_t> data) const override {
    py::gil_scoped_acquire gil;
    return WPyStruct{py::cast(wpi::UnpackStruct<T>(data))};
  }

  // void UnpackInto(WPyStruct *pyv,
  //                 std::span<const uint8_t> data) const override {
  //   py::gil_scoped_acquire gil;
  //   T *v = pyv->py.cast<T *>();
  //   wpi::UnpackStructInto(v, data);
  // }

  void ForEachNested(
      const std::function<void(std::string_view, std::string_view)> &fn)
      const override {
    if constexpr (wpi::HasNestedStruct<T>) {
      wpi::Struct<T>::ForEachNested(fn);
    }
  }
};

template <typename T> void SetupWPyStruct(auto pycls) {

  auto *sptr =
      new std::shared_ptr<WPyStructConverter>(new WPyStructCppConverter<T>());

  py::capsule c(sptr, "WPyStruct", [](void *ptr) {
    delete (std::shared_ptr<WPyStructConverter> *)ptr;
  });

  pycls.def_property_readonly_static("WPIStruct",
                                     [c](py::object pycls) { return c; });
}

// dynamic python converter
struct WPyStructPyConverter : WPyStructConverter {

  WPyStructPyConverter(py::object o) {
    m_typename = o.attr("typename").cast<std::string>();
    m_schema = o.attr("schema").cast<std::string>();
    m_size = o.attr("size").cast<size_t>();

    m_pack = py::reinterpret_borrow<py::function>(o.attr("pack"));
    m_packInto = py::reinterpret_borrow<py::function>(o.attr("packInto"));
    m_unpack = py::reinterpret_borrow<py::function>(o.attr("unpack"));
    // m_unpackInto = py::reinterpret_borrow<py::function>(o.attr("unpackInto"));
    m_forEachNested =
        py::reinterpret_borrow<py::function>(o.attr("forEachNested"));
  }

  // copy all the relevant attributes locally
  std::string m_typename;
  std::string m_schema;
  size_t m_size;

  py::function m_pack;
  py::function m_packInto;
  py::function m_unpack;
  // py::function m_unpackInto;
  py::function m_forEachNested; // might be none

  std::string_view GetTypeName() const override { return m_typename; }

  size_t GetSize() const override { return m_size; }

  std::string_view GetSchema() const override { return m_schema; }

  void Pack(std::span<uint8_t> data, const WPyStruct &value) const override {
    py::gil_scoped_acquire gil;
    py::bytes result = m_pack(value.py);
    std::string_view rview = result;
    if (rview.size() != data.size()) {
      std::string msg = fmt::format(
          "{} pack did not return {} bytes (returned {})",
          pytypename(py::type::of(value.py)), data.size(), rview.size());
      throw py::value_error(msg);
    }

    rview.copy((char *)data.data(), rview.size());
  }

  WPyStruct Unpack(std::span<const uint8_t> data) const override {
    py::gil_scoped_acquire gil;
    auto view =
        py::memoryview::from_memory((const void *)data.data(), data.size());
    return WPyStruct(m_unpack(view));
  }

  // void UnpackInto(WPyStruct *pyv,
  //                 std::span<const uint8_t> data) const override {
  //   py::gil_scoped_acquire gil;
  //   auto view =
  //       py::memoryview::from_memory((const void *)data.data(), data.size());
  //   m_unpackInto(pyv->py, view);
  // }

  void ForEachNested(
      const std::function<void(std::string_view, std::string_view)> &fn)
      const override {
    py::gil_scoped_acquire gil;
    if (!m_forEachNested.is_none()) {
      m_forEachNested(fn);
    }
  }
};

// passed as I... to the wpi::Struct methods
struct WPyStructInfo {
  WPyStructInfo() = default;
  WPyStructInfo(const py::type &t) {
    if (!py::hasattr(t, "WPIStruct")) {

      throw py::type_error(
          fmt::format("{} is not struct serializable (does not have WPIStruct)",
                      pytypename(t)));
    }

    py::object s = t.attr("WPIStruct");

    // C++ version
    void *c = PyCapsule_GetPointer(s.ptr(), "WPyStruct");
    if (c != NULL) {
      cvt = *(std::shared_ptr<WPyStructConverter> *)c;
      return;
    }

    PyErr_Clear();

    // Python version
    try {
      cvt = std::make_shared<WPyStructPyConverter>(s);
    } catch (py::error_already_set &e) {
      std::string msg = fmt::format(
          "{} is not struct serializable (invalid WPIStruct)", pytypename(t));
      py::raise_from(e, PyExc_TypeError, msg.c_str());
      throw py::error_already_set();
    }
  }

  WPyStructInfo(const WPyStruct &v) : WPyStructInfo(py::type::of(v.py)) {}

  const WPyStructConverter* operator->() const {
    const auto *c = cvt.get();
    if (c == nullptr) {
      // TODO: would be nice to have a better error here, but we don't have
      // a good way to know our current context
      throw py::value_error("Object is closed");
    }
    return c;
  }

private:
  // holds something used to do serialization
  std::shared_ptr<WPyStructConverter> cvt;
};

// Leverages the converter stored in WPyStructInfo to do the actual work
template <> struct wpi::Struct<WPyStruct, WPyStructInfo> {
  static std::string_view GetTypeName(const WPyStructInfo &info) {
    return info->GetTypeName();
  }

  static size_t GetSize(const WPyStructInfo &info) {
    return info->GetSize();
  }

  static std::string_view GetSchema(const WPyStructInfo &info) {
    return info->GetSchema();
  }

  static WPyStruct Unpack(std::span<const uint8_t> data,
                          const WPyStructInfo &info) {
    return info->Unpack(data);
  }

  // static void UnpackInto(WPyStruct *v, std::span<const uint8_t> data,
  //                        const WPyStructInfo &info) {
  //   info->UnpackInto(v, data);
  // }

  static void Pack(std::span<uint8_t> data, const WPyStruct &value,
                   const WPyStructInfo &info) {
    info->Pack(data, value);
  }

  static void
  ForEachNested(std::invocable<std::string_view, std::string_view> auto fn,
                const WPyStructInfo &info) {
    info->ForEachNested(fn);
  }
};

static_assert(wpi::StructSerializable<WPyStruct, WPyStructInfo>);
static_assert(wpi::HasNestedStruct<WPyStruct, WPyStructInfo>);

// This breaks on readonly structs, so we disable for now
// static_assert(wpi::MutableStructSerializable<WPyStruct, WPyStructInfo>);
