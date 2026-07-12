
#include <memory>
#include <string>
#include <utility>

#include <pybind11/functional.h>
#include <pybind11/stl.h>
#include <semiwrap.h>

#include "wpi/util/sendable/SendableBuilder.hpp"
#include "wpi/util/sendable/SendableRegistry.hpp"

class MySendableBuilder : public wpi::util::SendableBuilder {
 public:
  explicit MySendableBuilder(py::dict keys) : keys(keys) {}

  ~MySendableBuilder() override {
    // leak this so the python interpreter doesn't crash on shutdown
    keys.release();
  }

  void SetSmartDashboardType(std::string_view type) override {}

  void SetActuator(bool value) override {}

  void AddBooleanProperty(std::string_view key, std::function<bool()> getter,
                          std::function<void(bool)> setter) override {}

  void PublishConstBoolean(std::string_view key, bool value) override {}

  void AddIntegerProperty(std::string_view key, std::function<int64_t()> getter,
                          std::function<void(int64_t)> setter) override {}

  void PublishConstInteger(std::string_view key, int64_t value) override {}

  void AddFloatProperty(std::string_view key, std::function<float()> getter,
                        std::function<void(float)> setter) override {}

  void PublishConstFloat(std::string_view key, float value) override {}

  void AddDoubleProperty(std::string_view key, std::function<double()> getter,
                         std::function<void(double)> setter) override {
    py::gil_scoped_acquire gil;
    py::object pykey = py::cast(key);
    keys[pykey] = std::make_tuple(getter, setter);
  }

  void PublishConstDouble(std::string_view key, double value) override {}

  void AddStringProperty(
      std::string_view key, std::function<std::string()> getter,
      std::function<void(std::string_view)> setter) override {}

  void PublishConstString(std::string_view key,
                          std::string_view value) override {}

  void AddBooleanArrayProperty(
      std::string_view key, std::function<std::vector<int>()> getter,
      std::function<void(std::span<const int>)> setter) override {}

  void PublishConstBooleanArray(std::string_view key,
                                std::span<const int> value) override {}

  void AddIntegerArrayProperty(
      std::string_view key, std::function<std::vector<int64_t>()> getter,
      std::function<void(std::span<const int64_t>)> setter) override {}

  void PublishConstIntegerArray(std::string_view key,
                                std::span<const int64_t> value) override {}

  void AddFloatArrayProperty(
      std::string_view key, std::function<std::vector<float>()> getter,
      std::function<void(std::span<const float>)> setter) override {}

  void PublishConstFloatArray(std::string_view key,
                              std::span<const float> value) override {}

  void AddDoubleArrayProperty(
      std::string_view key, std::function<std::vector<double>()> getter,
      std::function<void(std::span<const double>)> setter) override {}

  void PublishConstDoubleArray(std::string_view key,
                               std::span<const double> value) override {}

  void AddStringArrayProperty(
      std::string_view key, std::function<std::vector<std::string>()> getter,
      std::function<void(std::span<const std::string>)> setter) override {}

  void PublishConstStringArray(std::string_view key,
                               std::span<const std::string> value) override {}

  void AddRawProperty(
      std::string_view key, std::string_view typeString,
      std::function<std::vector<uint8_t>()> getter,
      std::function<void(std::span<const uint8_t>)> setter) override {}

  void PublishConstRaw(std::string_view key, std::string_view typeString,
                       std::span<const uint8_t> value) override {}

  void AddSmallStringProperty(
      std::string_view key,
      std::function<std::string_view(wpi::util::SmallVectorImpl<char>& buf)>
          getter,
      std::function<void(std::string_view)> setter) override {}

  void AddSmallBooleanArrayProperty(
      std::string_view key,
      std::function<std::span<const int>(wpi::util::SmallVectorImpl<int>& buf)>
          getter,
      std::function<void(std::span<const int>)> setter) override {}

  void AddSmallIntegerArrayProperty(
      std::string_view key,
      std::function<
          std::span<const int64_t>(wpi::util::SmallVectorImpl<int64_t>& buf)>
          getter,
      std::function<void(std::span<const int64_t>)> setter) override {}

  void AddSmallFloatArrayProperty(
      std::string_view key,
      std::function<
          std::span<const float>(wpi::util::SmallVectorImpl<float>& buf)>
          getter,
      std::function<void(std::span<const float>)> setter) override {}

  void AddSmallDoubleArrayProperty(
      std::string_view key,
      std::function<
          std::span<const double>(wpi::util::SmallVectorImpl<double>& buf)>
          getter,
      std::function<void(std::span<const double>)> setter) override {}

  void AddSmallStringArrayProperty(
      std::string_view key,
      std::function<std::span<const std::string>(
          wpi::util::SmallVectorImpl<std::string>& buf)>
          getter,
      std::function<void(std::span<const std::string>)> setter) override {}

  void AddSmallRawProperty(
      std::string_view key, std::string_view typeString,
      std::function<
          std::span<uint8_t>(wpi::util::SmallVectorImpl<uint8_t>& buf)>
          getter,
      std::function<void(std::span<const uint8_t>)> setter) override {}

  wpi::util::SendableBuilder::BackendKind GetBackendKind() const override {
    return wpi::util::SendableBuilder::BackendKind::kUnknown;
  }

  bool IsPublished() const override { return false; }
  void Update() override {}
  void ClearProperties() override {}

  py::dict keys;
};

void Publish(wpi::util::SendableRegistry::UID sendableUid, py::dict keys) {
  auto builder = std::make_unique<MySendableBuilder>(keys);
  wpi::util::SendableRegistry::Publish(sendableUid, std::move(builder));
}

void sendable_test(py::module& m) {
  m.def("publish", Publish);
}
