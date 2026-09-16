// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/tunables/Tunable.hpp"

#include <array>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "wpi/tunables/ComplexTunable.hpp"
#include "wpi/tunables/MockTunableBackend.hpp"
#include "wpi/tunables/TunableBackend.hpp"
#include "wpi/tunables/TunableConfig.hpp"
#include "wpi/tunables/TunableRegistry.hpp"
#include "wpi/tunables/Tunables.hpp"

using namespace wpi;
using namespace wpi::tunables;

TEST_CASE("TunableRegistryTest ReportWarning", "[tunable]") {
  std::string lastWarning;
  wpi::tunables::TunableRegistry::SetReportWarning(
      [&lastWarning](std::string_view msg) { lastWarning = msg; });
  wpi::tunables::TunableRegistry::ReportWarning("test warning");
  CHECK(lastWarning == "test warning");
  wpi::tunables::TunableRegistry::SetReportWarning(nullptr);
}

TEST_CASE("TunableRegistryTest ReportWarningAllowsReentry", "[tunable]") {
  std::vector<std::string> warnings;
  bool nested = false;
  wpi::tunables::TunableRegistry::SetReportWarning([&](std::string_view msg) {
    warnings.emplace_back(msg);
    auto reportWarning = wpi::tunables::TunableRegistry::GetReportWarning();
    wpi::tunables::TunableRegistry::SetReportWarning(reportWarning);
    if (!nested) {
      nested = true;
      wpi::tunables::TunableRegistry::ReportWarning("nested warning");
    }
  });

  wpi::tunables::TunableRegistry::ReportWarning("outer warning");

  REQUIRE(warnings.size() == 2u);
  CHECK(warnings[0] == "outer warning");
  CHECK(warnings[1] == "nested warning");
  wpi::tunables::TunableRegistry::SetReportWarning(nullptr);
}

static_assert(std::derived_from<wpi::tunables::TunableBool,
                                detail::TunableValueBase<bool>>);
static_assert(std::derived_from<wpi::tunables::TunableInt32,
                                detail::TunableValueBase<int32_t>>);
static_assert(std::derived_from<wpi::tunables::TunableInt64,
                                detail::TunableValueBase<int64_t>>);
static_assert(std::derived_from<wpi::tunables::TunableFloat,
                                detail::TunableValueBase<float>>);
static_assert(std::derived_from<wpi::tunables::TunableDouble,
                                detail::TunableValueBase<double>>);
static_assert(
    std::derived_from<wpi::tunables::TunableRaw,
                      detail::TunableValueBase<std::vector<uint8_t>>>);
static_assert(std::derived_from<wpi::tunables::TunableBoolVector,
                                detail::TunableValueBase<std::vector<bool>>>);
static_assert(
    std::derived_from<wpi::tunables::TunableInt32Vector,
                      detail::TunableValueBase<std::vector<int32_t>>>);
static_assert(
    std::derived_from<wpi::tunables::TunableInt64Vector,
                      detail::TunableValueBase<std::vector<int64_t>>>);
static_assert(std::derived_from<wpi::tunables::TunableFloatVector,
                                detail::TunableValueBase<std::vector<float>>>);
static_assert(std::derived_from<wpi::tunables::TunableDoubleVector,
                                detail::TunableValueBase<std::vector<double>>>);
static_assert(
    std::same_as<decltype(wpi::tunables::TunableRegistry::GetUpdateMutex()),
                 wpi::util::recursive_mutex&>);
static_assert([] {
  wpi::tunables::TunableDouble value{1.0};
  value.Set(2.0);
  if (value.Get() != 2.0) {
    return false;
  }
  value = 3.0;
  double converted = value;
  return value.Get() == 3.0 && converted == 3.0;
}());

namespace {
struct TunableTest {
  TunableTest() {
    wpi::tunables::TunableRegistry::RegisterBackend("", backend);
  }

  ~TunableTest() {
    wpi::tunables::TunableRegistry::SetReportWarning(nullptr);
    wpi::tunables::TunableRegistry::Reset();
  }

  std::shared_ptr<wpi::tunables::MockTunableBackend> backend =
      std::make_shared<wpi::tunables::MockTunableBackend>();
};

class RecordingChangedBackend : public wpi::tunables::TunableBackend {
 public:
  bool Publish(std::string_view, uint32_t uid, detail::TunableBase&,
               const wpi::tunables::TunableConfig*,
               detail::TunableTypeValue) override {
    m_uids.emplace_back(uid);
    return true;
  }

  void MarkDirty(uint32_t) override {}

  void Remove(std::string_view) override {}

  std::vector<PublishedTunable> RemovePrefix(std::string_view) override {
    return {};
  }

  void UnregisterTunable(uint32_t uid) override { std::erase(m_uids, uid); }

  void Update() override {
    ++updateCount;
    for (auto uid : m_uids) {
      auto info = wpi::tunables::TunableRegistry::GetTunable(uid);
      bool changed = info.IsChanged();
      changedStates.emplace_back(changed);
      if (changed) {
        wpi::tunables::TunableRegistry::ResetChangedAfterUpdate(uid);
      }
    }
  }

  std::vector<bool> changedStates;
  int updateCount = 0;

 private:
  std::vector<uint32_t> m_uids;
};

class RetiringMockTunableBackend : public wpi::tunables::MockTunableBackend {
 public:
  void Retire() override { ++retireCount; }

  int retireCount = 0;
};

struct TestStruct {
  int16_t a;
  int16_t b;
};

struct ReentrantMockUpdateStruct {
  int16_t value;
};

struct ReentrantMockUpdateState {
  static inline wpi::tunables::MockTunableBackend* backend = nullptr;
  static inline bool removePending = false;
  static inline bool queuePending = false;
};

struct CustomType {
  int32_t val;
};

template <typename T>
struct CustomType2 {
  T val;
};

template <typename T>
class CustomTunableType2 {
 public:
  CustomTunableType2() = default;
  explicit CustomTunableType2(const CustomType2<T>& val) : m_tunable{val.val} {}

  CustomType2<T> Get() const { return CustomType2<T>{m_tunable.Get()}; }
  void Set(CustomType2<T> value) { m_tunable = value.val; }

  wpi::tunables::Tunable<T>& GetInnerTunable() { return m_tunable; }

 private:
  wpi::tunables::Tunable<T> m_tunable;
};

template <typename T>
constexpr CustomTunableType2<T> MakeTunable(const CustomType2<T>& val) {
  return {val};
}
}  // namespace

template <typename T>
class MyTest : public decltype(MakeTunable(std::declval<T>())){};

MyTest<CustomType2<int32_t>> test;

static_assert(std::same_as<decltype(MakeTunable(CustomType2<int32_t>{})),
                           CustomTunableType2<int32_t>>);

static_assert(std::derived_from<MyTest<CustomType2<int32_t>>,
                                CustomTunableType2<int32_t>>);

template <>
class wpi::tunables::CustomTunable<CustomType> {
 public:
  CustomTunable() = default;
  explicit CustomTunable(const CustomType& val) : m_tunable{val.val} {}

  CustomType Get() const { return CustomType{m_tunable.Get()}; }
  void Set(CustomType value) { m_tunable = value.val; }

  wpi::tunables::Tunable<int32_t>& GetInnerTunable() { return m_tunable; }

 private:
  wpi::tunables::Tunable<int32_t> m_tunable;
};

static_assert(wpi::tunables::detail::CustomTunableType<CustomType>);

class InspectableDoubleTunable : public wpi::tunables::TunableDouble {
 public:
  using wpi::tunables::TunableDouble::TunableDouble;
  using wpi::tunables::TunableDouble::operator=;

  uint32_t GetUid() const { return GetTunableUid(); }
};

struct AssignableComplex : public wpi::tunables::ComplexTunable {
  explicit AssignableComplex(double value) : gain{value} {}

  uint32_t GetUid() const { return GetTunableUid(); }
  uint32_t GetGainUid() const { return gain.GetUid(); }
  double GetGain() const { return gain.Get(); }

  void PublishTunable(wpi::tunables::TunableTable& table) override {
    table.Publish("gain", gain);
  }

  InspectableDoubleTunable gain;
};

template <>
struct wpi::util::Struct<TestStruct> {
  static constexpr std::string_view GetTypeName() { return "TestStruct"; }
  static constexpr size_t GetSize() { return 4; }
  static constexpr std::string_view GetSchema() { return "int16 a;int16 b"; }
  static TestStruct Unpack(std::span<const uint8_t> data) {
    return {wpi::util::UnpackStruct<int16_t, 0>(data),
            wpi::util::UnpackStruct<int16_t, 2>(data)};
  }
  static void Pack(std::span<uint8_t> data, TestStruct value) {
    wpi::util::PackStruct<0>(data, value.a);
    wpi::util::PackStruct<2>(data, value.b);
  }
};

template <>
struct wpi::util::Struct<ReentrantMockUpdateStruct> {
  static constexpr std::string_view GetTypeName() {
    return "ReentrantMockUpdateStruct";
  }
  static constexpr size_t GetSize() { return 2; }
  static constexpr std::string_view GetSchema() { return "int16 value"; }
  static ReentrantMockUpdateStruct Unpack(std::span<const uint8_t> data) {
    if (ReentrantMockUpdateState::removePending) {
      ReentrantMockUpdateState::removePending = false;
      wpi::tunables::Remove("removeMe");
    }
    if (ReentrantMockUpdateState::queuePending) {
      ReentrantMockUpdateState::queuePending = false;
      ReentrantMockUpdateState::backend->SetDouble("/queued", 4.0);
    }
    return {wpi::util::UnpackStruct<int16_t, 0>(data)};
  }
  static void Pack(std::span<uint8_t> data, ReentrantMockUpdateStruct value) {
    wpi::util::PackStruct<0>(data, value.value);
  }
};

struct MemberComplex : public wpi::tunables::ComplexTunable {
  int32_t gain = 1;
  TestStruct point{2, 3};
  int updateCount = 0;

  void PublishTunable(wpi::tunables::TunableTable& table) override {
    table.Publish("gain", this, &MemberComplex::gain);
    table.Publish("point", this, &MemberComplex::point);
  }

  void UpdateTunable() const override {
    ++const_cast<MemberComplex*>(this)->updateCount;
  }
};

struct RemovableMemberComplex : public wpi::tunables::ComplexTunable {
  int32_t gain = 1;

  void PublishTunable(wpi::tunables::TunableTable& table) override {
    PublishGain(table);
  }

  void PublishGain(wpi::tunables::TunableTable& table) {
    table.Publish("gain", this, &RemovableMemberComplex::gain);
  }

  void RemoveGain() { RemoveChildTunable("gain"); }

  void MarkGainChanged() { SetChildTunableChanged("gain"); }
};

struct ChangedParentMemberComplex : public wpi::tunables::ComplexTunable {
  int32_t gain = 1;

  void PublishTunable(wpi::tunables::TunableTable&) override {}

  void PublishGain(wpi::tunables::TunableTable& table) {
    table.Publish("gain", this, &ChangedParentMemberComplex::gain);
  }

  void MarkChanged() { SetTunableChanged(); }
};

struct DynamicComplex : public wpi::tunables::ComplexTunable {
  wpi::tunables::TunableDouble initial{1.0};
  wpi::tunables::TunableDouble dynamic{2.0};
  wpi::tunables::TunableDouble later{3.0};

  void PublishTunable(wpi::tunables::TunableTable& table) override {
    table.Publish("initial", initial);
  }

  void PublishDynamic() { PublishChildTunable("dynamic", dynamic); }

  void PublishLater() { PublishChildTunable("later", later); }

  void RemoveDynamic() { RemoveChildTunable("dynamic"); }
};

struct UpdatingDynamicComplex : public wpi::tunables::ComplexTunable {
  wpi::tunables::TunableDouble initial{1.0};
  wpi::tunables::TunableDouble dynamic{2.0};
  int updateCount = 0;

  void PublishTunable(wpi::tunables::TunableTable& table) override {
    table.Publish("initial", initial);
  }

  void UpdateTunable() const override {
    auto self = const_cast<UpdatingDynamicComplex*>(this);
    ++self->updateCount;
    if (self->updateCount == 1) {
      self->PublishChildTunable("dynamic", self->dynamic);
    } else if (self->updateCount == 2) {
      self->RemoveChildTunable("dynamic");
    }
  }
};

class FakeProtobufTunable : public detail::TunableProtobufBase {
 public:
  FakeProtobufTunable(std::string typeString, std::vector<uint8_t> data)
      : m_typeString{std::move(typeString)}, m_data{std::move(data)} {}

  std::string GetProtobufTypeString() const override { return m_typeString; }

  bool UnpackProtobuf(std::span<const uint8_t> data) override {
    m_data.assign(data.begin(), data.end());
    return true;
  }

  bool PackProtobuf(std::vector<uint8_t>& buf) const override {
    buf = m_data;
    return true;
  }

  void ForEachProtobufDescriptor(
      wpi::util::function_ref<bool(std::string_view)>,
      wpi::util::function_ref<void(std::string_view, std::string_view)>)
      const override {}

 private:
  std::string m_typeString;
  std::vector<uint8_t> m_data;
};

class FakeProtobufMember : public detail::TunableMemberProtobufBase {
 public:
  FakeProtobufMember(std::string typeString, std::vector<uint8_t> data)
      : m_typeString{std::move(typeString)}, m_data{std::move(data)} {}

  std::string GetProtobufTypeString() const override { return m_typeString; }

  bool UnpackProtobuf(wpi::tunables::ComplexTunable*,
                      std::span<const uint8_t> data) override {
    m_data.assign(data.begin(), data.end());
    return true;
  }

  bool PackProtobuf(wpi::tunables::ComplexTunable*,
                    std::vector<uint8_t>& buf) const override {
    buf = m_data;
    return true;
  }

  void ForEachProtobufDescriptor(
      wpi::util::function_ref<bool(std::string_view)>,
      wpi::util::function_ref<void(std::string_view, std::string_view)>)
      const override {}

 private:
  std::string m_typeString;
  std::vector<uint8_t> m_data;
};

struct FakeProtobufComplex : public wpi::tunables::ComplexTunable {
  void PublishTunable(wpi::tunables::TunableTable& table) override {
    table.Publish("value", this,
                  std::make_unique<FakeProtobufMember>(
                      "proto:FakeMember", std::vector<uint8_t>{4, 5, 6}));
  }
};

struct DirectStructComplex : public wpi::tunables::ComplexTunable {
  TestStruct point{2, 3};

  void PublishTunable(wpi::tunables::TunableTable& table) override {
    table.Publish("point", this, &DirectStructComplex::point);
  }
};

struct WrappedStructComplex : public wpi::tunables::ComplexTunable {
  wpi::tunables::Tunable<TestStruct> point{TestStruct{2, 3}};

  void PublishTunable(wpi::tunables::TunableTable& table) override {
    table.Publish("point", point);
  }
};

TEST_CASE_METHOD(TunableTest, "TunableTest IntTunable", "[tunable]") {
  wpi::tunables::Tunable<int32_t> tunable;

  tunable.Set(42);
  int32_t val = tunable.Get();
  CHECK(val == 42);

  tunable = 63;
  val = tunable;
  CHECK(val == 63);

  wpi::tunables::Publish("test", tunable);
  backend->SetInt32("/test", 84);
  wpi::tunables::TunableRegistry::Update();
  val = tunable.Get();
  CHECK(val == 84);
}

TEST_CASE_METHOD(TunableTest, "TunableTest PrimitiveAndVectorTunables",
                 "[tunable]") {
  wpi::tunables::TunableBool boolean{true};
  wpi::tunables::TunableInt64 integer64{1};
  wpi::tunables::TunableFloat floatValue{2.0f};
  wpi::tunables::TunableDouble doubleValue{3.0};
  wpi::tunables::TunableString stringValue{"start"};
  wpi::tunables::TunableInt32Vector vectorValue{std::vector<int32_t>{1, 2}};

  wpi::tunables::Publish("boolean", boolean);
  wpi::tunables::Publish("integer64", integer64);
  wpi::tunables::Publish("float", floatValue);
  wpi::tunables::Publish("double", doubleValue);
  wpi::tunables::Publish("string", stringValue);
  wpi::tunables::Publish("vector", vectorValue);

  backend->SetBool("/boolean", false);
  backend->SetInt64("/integer64", 10);
  backend->SetFloat("/float", 20.0f);
  backend->SetDouble("/double", 30.0);
  backend->SetString("/string", "remote");
  std::vector<int32_t> remoteVector{3, 4};
  backend->SetInt32Vector("/vector", remoteVector);
  remoteVector[0] = 99;
  wpi::tunables::TunableRegistry::Update();

  CHECK_FALSE(boolean.Get());
  CHECK(integer64.Get() == 10);
  CHECK(floatValue.Get() == 20.0f);
  CHECK(doubleValue.Get() == 30.0);
  CHECK(stringValue.Get() == "remote");
  CHECK(vectorValue.Get() == (std::vector<int32_t>{3, 4}));
}

TEST_CASE_METHOD(TunableTest, "TunableTest MockBackendGetValues", "[tunable]") {
  wpi::tunables::TunableBool boolean{true};
  wpi::tunables::TunableInt32 integer32{1};
  wpi::tunables::TunableInt64 integer64{2};
  wpi::tunables::TunableFloat floatValue{3.0f};
  wpi::tunables::TunableDouble doubleValue{4.0};
  wpi::tunables::TunableString stringValue{"value"};
  wpi::tunables::TunableRaw rawValue{std::vector<uint8_t>{1, 2}};
  wpi::tunables::TunableBoolVector boolVector{std::vector<bool>{true, false}};
  wpi::tunables::TunableInt32Vector int32Vector{std::vector<int32_t>{3, 4}};
  wpi::tunables::TunableInt64Vector int64Vector{std::vector<int64_t>{5, 6}};
  wpi::tunables::TunableFloatVector floatVector{std::vector<float>{7.0f, 8.0f}};
  wpi::tunables::TunableDoubleVector doubleVector{
      std::vector<double>{9.0, 10.0}};
  wpi::tunables::TunableStringVector stringVector{
      std::vector<std::string>{"one", "two"}};

  wpi::tunables::Publish("boolean", boolean);
  wpi::tunables::Publish("integer32", integer32);
  wpi::tunables::Publish("integer64", integer64);
  wpi::tunables::Publish("float", floatValue);
  wpi::tunables::Publish("double", doubleValue);
  wpi::tunables::Publish("string", stringValue);
  wpi::tunables::Publish("raw", rawValue);
  wpi::tunables::Publish("boolVector", boolVector);
  wpi::tunables::Publish("int32Vector", int32Vector);
  wpi::tunables::Publish("int64Vector", int64Vector);
  wpi::tunables::Publish("floatVector", floatVector);
  wpi::tunables::Publish("doubleVector", doubleVector);
  wpi::tunables::Publish("stringVector", stringVector);

  CHECK(backend->GetBool("/boolean"));
  CHECK(backend->GetInt32("/integer32") == 1);
  CHECK(backend->GetInt64("/integer64") == 2);
  CHECK(backend->GetFloat("/float") == 3.0f);
  CHECK(backend->GetDouble("/double") == 4.0);
  CHECK(backend->GetString("/string") == "value");
  CHECK(backend->GetRaw("/raw") == (std::vector<uint8_t>{1, 2}));
  CHECK(backend->GetBoolVector("/boolVector") ==
        (std::vector<bool>{true, false}));
  CHECK(backend->GetInt32Vector("/int32Vector") ==
        (std::vector<int32_t>{3, 4}));
  CHECK(backend->GetInt64Vector("/int64Vector") ==
        (std::vector<int64_t>{5, 6}));
  CHECK(backend->GetFloatVector("/floatVector") ==
        (std::vector<float>{7.0f, 8.0f}));
  CHECK(backend->GetDoubleVector("/doubleVector") ==
        (std::vector<double>{9.0, 10.0}));
  CHECK(backend->GetStringVector("/stringVector") ==
        (std::vector<std::string>{"one", "two"}));
  CHECK(backend->GetValue<std::vector<std::string>>("/stringVector") ==
        (std::vector<std::string>{"one", "two"}));

  MemberComplex complex;
  wpi::tunables::Publish("member", complex);
  CHECK(backend->GetInt32("/member/gain") == 1);

  CHECK_THROWS_AS(backend->GetString("/integer32"), std::invalid_argument);
  CHECK_THROWS_AS(backend->GetDouble("/missing"), std::invalid_argument);
}

TEST_CASE_METHOD(TunableTest, "TunableTest MockBackendStructAndProtobufGetters",
                 "[tunable]") {
  wpi::tunables::Tunable<TestStruct> structValue{TestStruct{1, 2}};
  wpi::tunables::Tunable<std::vector<TestStruct>> structVector{
      std::vector<TestStruct>{{3, 4}, {5, 6}}};
  wpi::tunables::Tunable<std::array<TestStruct, 2>> structArray{
      std::array<TestStruct, 2>{TestStruct{7, 8}, TestStruct{9, 10}}};

  wpi::tunables::Publish("struct", structValue);
  wpi::tunables::Publish("structVector", structVector);
  wpi::tunables::Publish("structArray", structArray);

  auto readStruct = backend->GetStruct<TestStruct>("/struct");
  CHECK(readStruct.a == 1);
  CHECK(readStruct.b == 2);
  CHECK(backend->GetStructTypeName("/struct") == "TestStruct");
  CHECK(backend->GetStructData("/struct").size() == 4u);

  auto readVector = backend->GetStructVector<TestStruct>("/structVector");
  REQUIRE(readVector.size() == 2u);
  CHECK(readVector[0].a == 3);
  CHECK(readVector[0].b == 4);
  CHECK(readVector[1].a == 5);
  CHECK(readVector[1].b == 6);

  auto readArray =
      backend->GetStruct<std::array<TestStruct, 2>>("/structArray");
  CHECK(readArray[0].a == 7);
  CHECK(readArray[0].b == 8);
  CHECK(readArray[1].a == 9);
  CHECK(readArray[1].b == 10);

  MemberComplex complex;
  wpi::tunables::Publish("memberStruct", complex);
  auto readMemberStruct = backend->GetStruct<TestStruct>("/memberStruct/point");
  CHECK(readMemberStruct.a == 2);
  CHECK(readMemberStruct.b == 3);

  FakeProtobufTunable protobuf{"proto:Fake", {1, 2, 3}};
  wpi::tunables::Publish("protobuf", protobuf);
  CHECK(backend->GetProtobufTypeString("/protobuf") == "proto:Fake");
  CHECK(backend->GetProtobufData("/protobuf") ==
        (std::vector<uint8_t>{1, 2, 3}));

  FakeProtobufComplex protobufComplex;
  wpi::tunables::Publish("memberProtobuf", protobufComplex);
  CHECK(backend->GetProtobufTypeString("/memberProtobuf/value") ==
        "proto:FakeMember");
  CHECK(backend->GetProtobufData("/memberProtobuf/value") ==
        (std::vector<uint8_t>{4, 5, 6}));

  CHECK_THROWS_AS(backend->GetStruct<TestStruct>("/structVector"),
                  std::invalid_argument);
  CHECK_THROWS_AS(backend->GetStructData("/protobuf"), std::invalid_argument);
  CHECK_THROWS_AS(backend->GetProtobufData("/struct"), std::invalid_argument);
}

TEST_CASE_METHOD(TunableTest, "TunableTest MutateMarksTunablesChanged",
                 "[tunable]") {
  class InspectableInt : public wpi::tunables::TunableInt32 {
   public:
    using wpi::tunables::TunableInt32::TunableInt32;

    uint32_t GetUid() const { return GetTunableUid(); }
  };
  class InspectableVector : public wpi::tunables::TunableInt32Vector {
   public:
    using wpi::tunables::TunableInt32Vector::TunableInt32Vector;

    uint32_t GetUid() const { return GetTunableUid(); }
  };

  wpi::tunables::TunableConfig config;
  InspectableInt integer{1, config};
  InspectableVector vector{std::vector<int32_t>{1, 2}, config};

  auto integerInfo =
      wpi::tunables::TunableRegistry::GetTunable(integer.GetUid());
  auto vectorInfo = wpi::tunables::TunableRegistry::GetTunable(vector.GetUid());
  REQUIRE(integerInfo);
  REQUIRE(vectorInfo);
  CHECK_FALSE(integerInfo.IsChanged());
  CHECK_FALSE(vectorInfo.IsChanged());

  integer.Mutate() = 2;
  vector.Mutate().push_back(3);

  integerInfo = wpi::tunables::TunableRegistry::GetTunable(integer.GetUid());
  vectorInfo = wpi::tunables::TunableRegistry::GetTunable(vector.GetUid());
  CHECK(integer.Get() == 2);
  CHECK(vector.Get() == (std::vector<int32_t>{1, 2, 3}));
  CHECK(integerInfo.IsChanged());
  CHECK(vectorInfo.IsChanged());
}

TEST_CASE_METHOD(TunableTest, "TunableTest SameScalarAssignmentDoesNotDirty",
                 "[tunable]") {
  InspectableDoubleTunable value{1.0};
  wpi::tunables::Publish("value", value);

  auto info = wpi::tunables::TunableRegistry::GetTunable(value.GetUid());
  REQUIRE(info);
  CHECK_FALSE(info.IsChanged());

  value = 1.0;
  info = wpi::tunables::TunableRegistry::GetTunable(value.GetUid());
  REQUIRE(info);
  CHECK_FALSE(info.IsChanged());

  value = 2.0;
  info = wpi::tunables::TunableRegistry::GetTunable(value.GetUid());
  REQUIRE(info);
  CHECK(info.IsChanged());
}

TEST_CASE_METHOD(
    TunableTest,
    "TunableTest AssignmentPreservesPublishedDestinationRegistration",
    "[tunable]") {
  uint32_t copySourceUid;
  uint32_t copyDestinationUid;
  {
    InspectableDoubleTunable source{2.0};
    InspectableDoubleTunable destination{1.0};
    wpi::tunables::Publish("copySource", source);
    wpi::tunables::Publish("copyDestination", destination);
    copySourceUid = source.GetUid();
    copyDestinationUid = destination.GetUid();

    destination = source;

    CHECK(source.GetUid() == copySourceUid);
    CHECK(destination.GetUid() == copyDestinationUid);
    CHECK(destination.Get() == 2.0);
    REQUIRE(wpi::tunables::TunableRegistry::GetTunable(copyDestinationUid));
    CHECK(wpi::tunables::TunableRegistry::GetTunable(copyDestinationUid)
              .IsChanged());

    backend->SetDouble("/copySource", 3.0);
    backend->SetDouble("/copyDestination", 4.0);
    wpi::tunables::TunableRegistry::Update();

    CHECK(source.Get() == 3.0);
    CHECK(destination.Get() == 4.0);
  }
  CHECK_FALSE(wpi::tunables::TunableRegistry::GetTunable(copySourceUid));
  CHECK_FALSE(wpi::tunables::TunableRegistry::GetTunable(copyDestinationUid));

  uint32_t moveSourceUid;
  uint32_t moveDestinationUid;
  {
    InspectableDoubleTunable source{6.0};
    InspectableDoubleTunable destination{5.0};
    wpi::tunables::Publish("moveSource", source);
    wpi::tunables::Publish("moveDestination", destination);
    moveSourceUid = source.GetUid();
    moveDestinationUid = destination.GetUid();

    destination = std::move(source);

    CHECK(source.GetUid() == moveSourceUid);
    CHECK(destination.GetUid() == moveDestinationUid);
    CHECK(destination.Get() == 6.0);
    REQUIRE(wpi::tunables::TunableRegistry::GetTunable(moveDestinationUid));
    CHECK(wpi::tunables::TunableRegistry::GetTunable(moveDestinationUid)
              .IsChanged());

    backend->SetDouble("/moveSource", 7.0);
    backend->SetDouble("/moveDestination", 8.0);
    wpi::tunables::TunableRegistry::Update();

    CHECK(source.Get() == 7.0);
    CHECK(destination.Get() == 8.0);
  }
  CHECK_FALSE(wpi::tunables::TunableRegistry::GetTunable(moveSourceUid));
  CHECK_FALSE(wpi::tunables::TunableRegistry::GetTunable(moveDestinationUid));
}

TEST_CASE_METHOD(
    TunableTest,
    "TunableTest MoveAssignmentMarksPublishedSourceChangedForMovableValues",
    "[tunable]") {
  class InspectableStringTunable : public wpi::tunables::TunableString {
   public:
    using wpi::tunables::TunableString::TunableString;

    uint32_t GetUid() const { return GetTunableUid(); }
  };

  InspectableStringTunable source{std::string{"source"}};
  InspectableStringTunable destination{std::string{"destination"}};
  wpi::tunables::Publish("moveStringSource", source);
  wpi::tunables::Publish("moveStringDestination", destination);
  uint32_t sourceUid = source.GetUid();
  uint32_t destinationUid = destination.GetUid();

  auto sourceInfo = wpi::tunables::TunableRegistry::GetTunable(sourceUid);
  auto destinationInfo =
      wpi::tunables::TunableRegistry::GetTunable(destinationUid);
  REQUIRE(sourceInfo);
  REQUIRE(destinationInfo);
  CHECK_FALSE(sourceInfo.IsChanged());
  CHECK_FALSE(destinationInfo.IsChanged());

  destination = std::move(source);

  CHECK(source.GetUid() == sourceUid);
  CHECK(destination.GetUid() == destinationUid);
  CHECK(destination.Get() == "source");
  REQUIRE(wpi::tunables::TunableRegistry::GetTunable(sourceUid));
  REQUIRE(wpi::tunables::TunableRegistry::GetTunable(destinationUid));
  CHECK(wpi::tunables::TunableRegistry::GetTunable(sourceUid).IsChanged());
  CHECK(wpi::tunables::TunableRegistry::GetTunable(destinationUid).IsChanged());
}

TEST_CASE_METHOD(
    TunableTest,
    "TunableTest ComplexAssignmentPreservesPublishedDestinationRegistration",
    "[tunable]") {
  uint32_t copyComplexUid;
  uint32_t copyGainUid;
  {
    AssignableComplex source{2.0};
    AssignableComplex destination{1.0};
    wpi::tunables::Publish("copyComplex", destination);
    copyComplexUid = destination.GetUid();
    copyGainUid = destination.GetGainUid();

    destination = source;

    CHECK(destination.GetUid() == copyComplexUid);
    CHECK(destination.GetGainUid() == copyGainUid);
    CHECK(destination.GetGain() == 2.0);
    REQUIRE(wpi::tunables::TunableRegistry::GetTunable(copyComplexUid));
    REQUIRE(wpi::tunables::TunableRegistry::GetTunable(copyGainUid));
    CHECK(
        wpi::tunables::TunableRegistry::GetTunable(copyComplexUid).IsChanged());
    CHECK(wpi::tunables::TunableRegistry::GetTunable(copyGainUid).IsChanged());

    backend->SetDouble("/copyComplex/gain", 3.0);
    wpi::tunables::TunableRegistry::Update();

    CHECK(destination.GetGain() == 3.0);
  }
  CHECK_FALSE(wpi::tunables::TunableRegistry::GetTunable(copyComplexUid));
  CHECK_FALSE(wpi::tunables::TunableRegistry::GetTunable(copyGainUid));

  uint32_t moveComplexUid;
  uint32_t moveGainUid;
  {
    AssignableComplex source{5.0};
    AssignableComplex destination{4.0};
    wpi::tunables::Publish("moveComplex", destination);
    moveComplexUid = destination.GetUid();
    moveGainUid = destination.GetGainUid();

    destination = std::move(source);

    CHECK(destination.GetUid() == moveComplexUid);
    CHECK(destination.GetGainUid() == moveGainUid);
    CHECK(destination.GetGain() == 5.0);
    REQUIRE(wpi::tunables::TunableRegistry::GetTunable(moveComplexUid));
    REQUIRE(wpi::tunables::TunableRegistry::GetTunable(moveGainUid));
    CHECK(
        wpi::tunables::TunableRegistry::GetTunable(moveComplexUid).IsChanged());
    CHECK(wpi::tunables::TunableRegistry::GetTunable(moveGainUid).IsChanged());

    backend->SetDouble("/moveComplex/gain", 6.0);
    wpi::tunables::TunableRegistry::Update();

    CHECK(destination.GetGain() == 6.0);
  }
  CHECK_FALSE(wpi::tunables::TunableRegistry::GetTunable(moveComplexUid));
  CHECK_FALSE(wpi::tunables::TunableRegistry::GetTunable(moveGainUid));
}

TEST_CASE_METHOD(
    TunableTest,
    "TunableTest ComplexMoveAssignmentTransfersPublishedSourceRegistration",
    "[tunable]") {
  uint32_t sourceComplexUid;
  uint32_t sourceGainUid;
  uint32_t destinationComplexUid;
  uint32_t destinationGainUid;
  {
    AssignableComplex source{5.0};
    AssignableComplex destination{4.0};
    wpi::tunables::Publish("moveAssignSource", source);
    wpi::tunables::Publish("moveAssignDestination", destination);
    sourceComplexUid = source.GetUid();
    sourceGainUid = source.GetGainUid();
    destinationComplexUid = destination.GetUid();
    destinationGainUid = destination.GetGainUid();

    destination = std::move(source);

    CHECK(destination.GetUid() == sourceComplexUid);
    CHECK(destination.GetGainUid() == sourceGainUid);
    CHECK(destination.GetGain() == 5.0);
    CHECK_FALSE(
        wpi::tunables::TunableRegistry::GetTunable(destinationComplexUid));
    CHECK_FALSE(wpi::tunables::TunableRegistry::GetTunable(destinationGainUid));
    CHECK_FALSE(backend->GetUid("/moveAssignDestination"));
    CHECK_FALSE(backend->GetUid("/moveAssignDestination/gain"));
    REQUIRE(wpi::tunables::TunableRegistry::GetTunable(sourceComplexUid));
    REQUIRE(wpi::tunables::TunableRegistry::GetTunable(sourceGainUid));

    backend->SetDouble("/moveAssignSource/gain", 6.0);
    wpi::tunables::TunableRegistry::Update();

    CHECK(destination.GetGain() == 6.0);
  }
  CHECK_FALSE(wpi::tunables::TunableRegistry::GetTunable(sourceComplexUid));
  CHECK_FALSE(wpi::tunables::TunableRegistry::GetTunable(sourceGainUid));
  CHECK_FALSE(
      wpi::tunables::TunableRegistry::GetTunable(destinationComplexUid));
  CHECK_FALSE(wpi::tunables::TunableRegistry::GetTunable(destinationGainUid));
}

TEST_CASE_METHOD(TunableTest, "TunableTest ConfigImmutableAndOnTune",
                 "[tunable]") {
  int calls = 0;
  wpi::tunables::TunableConfig mutableConfig{
      .onTune = [&](detail::TunableBase&, wpi::tunables::ComplexTunable*) {
        ++calls;
      }};
  wpi::tunables::TunableConfig immutableConfig{.isMutable = false,
                                               .onTune = mutableConfig.onTune};
  wpi::tunables::TunableInt32 mutableTunable{0, mutableConfig};
  wpi::tunables::TunableInt32 immutableTunable{5, immutableConfig};

  wpi::tunables::Publish("mutable", mutableTunable);
  wpi::tunables::Publish("immutable", immutableTunable);

  backend->SetInt32("/mutable", 1);
  backend->SetInt32("/immutable", 42);
  CHECK(calls == 0);
  wpi::tunables::TunableRegistry::Update();

  CHECK(mutableTunable.Get() == 1);
  CHECK(immutableTunable.Get() == 5);
  CHECK(calls == 1);
}

TEST_CASE_METHOD(
    TunableTest,
    "TunableTest MockSetterCanRemovePendingActionAndContinueUpdate",
    "[tunable]") {
  wpi::tunables::Tunable<ReentrantMockUpdateStruct> first{
      ReentrantMockUpdateStruct{1}};
  wpi::tunables::TunableDouble removeMe{1.0};
  wpi::tunables::TunableDouble second{1.0};
  wpi::tunables::Publish("first", first);
  wpi::tunables::Publish("removeMe", removeMe);
  wpi::tunables::Publish("second", second);

  ReentrantMockUpdateState::removePending = true;
  backend->SetStruct<ReentrantMockUpdateStruct>("/first", {2});
  backend->SetDouble("/removeMe", 99.0);
  backend->SetDouble("/second", 3.0);
  wpi::tunables::TunableRegistry::Update();

  CHECK(first.Get().value == 2);
  CHECK(removeMe.Get() == 1.0);
  CHECK(second.Get() == 3.0);
}

TEST_CASE_METHOD(TunableTest,
                 "TunableTest MockSetterCanQueueActionForNextUpdate",
                 "[tunable]") {
  wpi::tunables::Tunable<ReentrantMockUpdateStruct> first{
      ReentrantMockUpdateStruct{1}};
  wpi::tunables::TunableDouble queued{1.0};
  wpi::tunables::Publish("first", first);
  wpi::tunables::Publish("queued", queued);

  ReentrantMockUpdateState::backend = backend.get();
  ReentrantMockUpdateState::queuePending = true;
  backend->SetStruct<ReentrantMockUpdateStruct>("/first", {2});
  wpi::tunables::TunableRegistry::Update();

  CHECK(first.Get().value == 2);
  CHECK(queued.Get() == 1.0);

  wpi::tunables::TunableRegistry::Update();

  CHECK(queued.Get() == 4.0);
  ReentrantMockUpdateState::backend = nullptr;
}

TEST_CASE_METHOD(
    TunableTest,
    "TunableTest UpdateDeduplicatesSameBackendRegisteredForMultiplePrefixes",
    "[tunable]") {
  wpi::tunables::TunableRegistry::Reset();
  auto recordingBackend = std::make_shared<RecordingChangedBackend>();
  wpi::tunables::TunableRegistry::RegisterBackend("", recordingBackend);
  wpi::tunables::TunableRegistry::RegisterBackend("/child", recordingBackend);

  wpi::tunables::TunableRegistry::Update();

  CHECK(recordingBackend->updateCount == 1);
}

TEST_CASE_METHOD(TunableTest, "TunableTest ResetRemovesAllBackendEntries",
                 "[tunable]") {
  wpi::tunables::TunableRegistry::Reset();
  auto cleanupBackend = std::make_shared<RetiringMockTunableBackend>();
  wpi::tunables::TunableRegistry::RegisterBackend("", cleanupBackend);
  wpi::tunables::TunableDouble tunable{1.0};
  wpi::tunables::Publish("retained", tunable);
  REQUIRE(cleanupBackend->GetUid("/retained"));

  wpi::tunables::TunableRegistry::Reset();

  CHECK_FALSE(cleanupBackend->GetUid("/retained"));
  CHECK(cleanupBackend->retireCount == 1);
}

TEST_CASE_METHOD(TunableTest, "TunableTest TunableConfigOptions", "[tunable]") {
  int calls = 0;
  wpi::tunables::TunableConfig config{
      .properties = wpi::util::json::object("min", 0),
      .robust = true,
      .typeString = "UnitTestWidget",
      .isMutable = false,
      .onTune = [&](detail::TunableBase&,
                    wpi::tunables::ComplexTunable*) { ++calls; },
      .polling = wpi::tunables::TunableConfig::Polling::ALWAYS_GET};
  class InspectableInt : public wpi::tunables::TunableInt32 {
   public:
    using wpi::tunables::TunableInt32::TunableInt32;

    uint32_t GetUid() const { return GetTunableUid(); }
  };
  InspectableInt tunable{1, config};
  auto info = wpi::tunables::TunableRegistry::GetTunable(tunable.GetUid());

  REQUIRE(static_cast<bool>(info));
  REQUIRE(info.config != nullptr);
  CHECK(info.config->robust);
  CHECK(info.config->properties.at("min") == 0);
  REQUIRE(info.config->typeString.has_value());
  CHECK(info.config->typeString.value() == "UnitTestWidget");
  CHECK_FALSE(info.config->isMutable);
  CHECK(info.config->polling ==
        wpi::tunables::TunableConfig::Polling::ALWAYS_GET);

  auto getOnChangeConfig = wpi::tunables::TunableConfig::GetOnChange();
  CHECK(getOnChangeConfig.polling ==
        wpi::tunables::TunableConfig::Polling::GET_ON_CHANGE);

  auto alwaysGetConfig = wpi::tunables::TunableConfig::AlwaysGet();
  CHECK(alwaysGetConfig.polling ==
        wpi::tunables::TunableConfig::Polling::ALWAYS_GET);

  wpi::tunables::Publish("configured", tunable);
  backend->SetInt32("/configured", 2);
  wpi::tunables::TunableRegistry::Update();

  CHECK(tunable.Get() == 1);
  CHECK(calls == 0);

  info.config->onTune(tunable, nullptr);
  CHECK(calls == 1);
}

TEST_CASE_METHOD(TunableTest,
                 "TunableTest OnTuneReceivesMemberTunableAndParent",
                 "[tunable]") {
  int calls = 0;
  bool receivedMember = false;
  bool receivedParent = false;
  MemberComplex complex;
  wpi::tunables::TunableConfig config{
      .onTune = [&](detail::TunableBase& tunable,
                    wpi::tunables::ComplexTunable* parent) {
        ++calls;
        receivedMember =
            &tunable != static_cast<detail::TunableBase*>(&complex);
        receivedParent = parent == &complex;
      }};

  wpi::tunables::Publish("complexOnTune/gain", &complex, &MemberComplex::gain,
                         config);
  backend->SetInt32("/complexOnTune/gain", 7);
  wpi::tunables::TunableRegistry::Update();

  CHECK(complex.gain == 7);
  CHECK(calls == 1);
  CHECK(receivedMember);
  CHECK(receivedParent);
}

TEST_CASE_METHOD(TunableTest,
                 "TunableTest ComplexUpdateRunsBeforeBackendTuning",
                 "[tunable]") {
  struct UpdatingComplex : wpi::tunables::ComplexTunable {
    int32_t source = 1;
    int32_t value = 0;
    int32_t tunedValue = -1;

    void PublishTunable(wpi::tunables::TunableTable& table) override {
      table.Publish(
          "value", this, &UpdatingComplex::value,
          wpi::tunables::TunableConfig{
              .onTune =
                  [](detail::TunableBase&,
                     wpi::tunables::ComplexTunable* self) {
                    if (auto complex = static_cast<UpdatingComplex*>(self)) {
                      complex->tunedValue = complex->value;
                    }
                  },
              .parent = this});
    }

    void UpdateTunable() const override {
      const_cast<UpdatingComplex*>(this)->value = source;
    }
  };

  UpdatingComplex complex;
  wpi::tunables::Publish("updating", complex);

  complex.source = 2;
  backend->SetInt32("/updating/value", 5);
  wpi::tunables::TunableRegistry::Update();

  CHECK(complex.value == 5);
  CHECK(complex.tunedValue == 5);

  wpi::tunables::TunableRegistry::Update();
  CHECK(complex.value == 2);
}

TEST_CASE_METHOD(TunableTest,
                 "TunableTest DestroyingComplexUnregistersMemberTunables",
                 "[tunable]") {
  std::optional<uint32_t> parentUid;
  std::optional<uint32_t> gainUid;
  std::optional<uint32_t> pointUid;
  {
    MemberComplex complex;
    wpi::tunables::Publish("destroyedComplex", complex);
    parentUid = backend->GetUid("/destroyedComplex");
    gainUid = backend->GetUid("/destroyedComplex/gain");
    pointUid = backend->GetUid("/destroyedComplex/point");

    REQUIRE(parentUid);
    REQUIRE(gainUid);
    REQUIRE(pointUid);

    backend->SetInt32("/destroyedComplex/gain", 7);
  }

  CHECK_FALSE(backend->GetUid("/destroyedComplex"));
  CHECK_FALSE(backend->GetUid("/destroyedComplex/gain"));
  CHECK_FALSE(backend->GetUid("/destroyedComplex/point"));
  CHECK_FALSE(wpi::tunables::TunableRegistry::GetTunable(*parentUid));
  CHECK_FALSE(wpi::tunables::TunableRegistry::GetTunable(*gainUid));
  CHECK_FALSE(wpi::tunables::TunableRegistry::GetTunable(*pointUid));
  CHECK_NOTHROW(wpi::tunables::TunableRegistry::Update());
}

TEST_CASE_METHOD(
    TunableTest,
    "TunableTest DestroyingChangedComplexUnregistersLaterMemberWrapper",
    "[tunable]") {
  std::optional<uint32_t> parentUid;
  std::optional<uint32_t> gainUid;
  {
    ChangedParentMemberComplex complex;
    wpi::tunables::Publish("changedParent", complex);
    parentUid = backend->GetUid("/changedParent");
    REQUIRE(parentUid);

    complex.MarkChanged();
    REQUIRE(wpi::tunables::TunableRegistry::GetTunable(*parentUid).IsChanged());

    auto table = wpi::tunables::GetTable("changedParent");
    complex.PublishGain(table);
    gainUid = backend->GetUid("/changedParent/gain");
    REQUIRE(gainUid);
  }

  CHECK_FALSE(backend->GetUid("/changedParent"));
  CHECK_FALSE(backend->GetUid("/changedParent/gain"));
  CHECK_FALSE(wpi::tunables::TunableRegistry::GetTunable(*parentUid));
  CHECK_FALSE(wpi::tunables::TunableRegistry::GetTunable(*gainUid));
}

TEST_CASE_METHOD(TunableTest, "TunableTest TunablesGetTableFacade",
                 "[tunable]") {
  wpi::tunables::Tunable<double> tunable;
  auto table = wpi::tunables::GetTable("arm");

  table.Publish("speed", tunable);
  backend->SetDouble("/arm/speed", 2.0);
  wpi::tunables::TunableRegistry::Update();

  CHECK(tunable.Get() == 2.0);
}

TEST_CASE_METHOD(TunableTest, "TunableTest TablePathsRouteAndRemove",
                 "[tunable]") {
  auto childBackend = std::make_shared<wpi::tunables::MockTunableBackend>();
  wpi::tunables::TunableRegistry::RegisterBackend("/child", childBackend);

  CHECK(wpi::tunables::GetTable().GetPath() == "/");
  CHECK(wpi::tunables::GetTable("drive").GetPath() == "/drive/");
  CHECK(wpi::tunables::GetTable("drive").GetTable("left").GetPath() ==
        "/drive/left/");

  wpi::tunables::TunableDouble root{1.0};
  wpi::tunables::TunableDouble child{2.0};
  wpi::tunables::Publish("root", root);
  wpi::tunables::Publish("child/value", child);

  backend->SetDouble("/root", 3.0);
  childBackend->SetDouble("/child/value", 4.0);
  wpi::tunables::TunableRegistry::Update();
  CHECK(root.Get() == 3.0);
  CHECK(child.Get() == 4.0);
  CHECK_THROWS_AS(backend->SetDouble("/child/value", 5.0),
                  std::invalid_argument);

  wpi::tunables::Remove("child/value");
  CHECK_THROWS_AS(childBackend->SetDouble("/child/value", 6.0),
                  std::invalid_argument);
}

TEST_CASE_METHOD(TunableTest, "TunableTest GetBackendNormalizesPath",
                 "[tunable]") {
  auto childBackend = std::make_shared<wpi::tunables::MockTunableBackend>();
  wpi::tunables::TunableRegistry::RegisterBackend("/child", childBackend);

  CHECK(wpi::tunables::TunableRegistry::GetBackend("child/value") ==
        childBackend);
  CHECK(wpi::tunables::TunableRegistry::GetBackend("//child//value") ==
        childBackend);
  CHECK(wpi::tunables::TunableRegistry::GetBackend("children/value") ==
        backend);

  wpi::tunables::TunableRegistry::Reset();
  auto childOnlyBackend = std::make_shared<wpi::tunables::MockTunableBackend>();
  wpi::tunables::TunableRegistry::RegisterBackend("child", childOnlyBackend);

  CHECK(wpi::tunables::TunableRegistry::GetBackend("child/value") ==
        childOnlyBackend);

  wpi::tunables::TunableRegistry::Reset();
  auto repeatedSlashChildBackend =
      std::make_shared<wpi::tunables::MockTunableBackend>();
  wpi::tunables::TunableRegistry::RegisterBackend("//child",
                                                  repeatedSlashChildBackend);

  CHECK(wpi::tunables::TunableRegistry::GetBackend("child/value") ==
        repeatedSlashChildBackend);
}

TEST_CASE_METHOD(TunableTest,
                 "TunableTest MissingBackendWarnsAndUsesNoopBackend",
                 "[tunable]") {
  wpi::tunables::TunableRegistry::Reset();
  std::vector<std::string> warnings;
  wpi::tunables::TunableRegistry::SetReportWarning(
      [&warnings](std::string_view msg) { warnings.emplace_back(msg); });

  auto missingBackend = wpi::tunables::TunableRegistry::GetBackend("missing");
  wpi::tunables::TunableDouble tunable{1.0};
  wpi::tunables::Publish("missing", tunable);
  wpi::tunables::TunableRegistry::Update();

  CHECK(missingBackend->RemovePrefix("/missing").empty());
  bool foundWarning = false;
  for (auto&& warning : warnings) {
    if (warning.find("no backend for path '/missing'") != std::string::npos) {
      foundWarning = true;
    }
  }
  CHECK(foundWarning);
}

TEST_CASE_METHOD(TunableTest,
                 "TunableTest RegisterBackendMigratesExistingMatchingTunables",
                 "[tunable]") {
  wpi::tunables::TunableDouble root{1.0};
  wpi::tunables::TunableDouble child{2.0};
  wpi::tunables::Publish("root", root);
  wpi::tunables::Publish("child/value", child);

  backend->SetDouble("/root", 3.0);
  backend->SetDouble("/child/value", 4.0);
  wpi::tunables::TunableRegistry::Update();
  CHECK(root.Get() == 3.0);
  CHECK(child.Get() == 4.0);

  auto childBackend = std::make_shared<wpi::tunables::MockTunableBackend>();
  wpi::tunables::TunableRegistry::RegisterBackend("/child", childBackend);

  CHECK_THROWS_AS(backend->SetDouble("/child/value", 5.0),
                  std::invalid_argument);
  childBackend->SetDouble("/child/value", 6.0);
  backend->SetDouble("/root", 7.0);
  wpi::tunables::TunableRegistry::Update();

  CHECK(root.Get() == 7.0);
  CHECK(child.Get() == 6.0);
}

TEST_CASE_METHOD(TunableTest,
                 "TunableTest RegisterBackendNormalizesPrefixForMigration",
                 "[tunable]") {
  wpi::tunables::TunableDouble child{2.0};
  wpi::tunables::Publish("child/value", child);

  CHECK(backend->GetDouble("/child/value") == 2.0);

  auto childBackend = std::make_shared<wpi::tunables::MockTunableBackend>();
  wpi::tunables::TunableRegistry::RegisterBackend("child", childBackend);

  CHECK_THROWS_AS(backend->SetDouble("/child/value", 3.0),
                  std::invalid_argument);
  childBackend->SetDouble("/child/value", 4.0);
  wpi::tunables::TunableRegistry::Update();

  CHECK(child.Get() == 4.0);
}

TEST_CASE_METHOD(TunableTest,
                 "TunableTest RegisterBackendMigrationUsesPathBoundaries",
                 "[tunable]") {
  wpi::tunables::TunableDouble child{2.0};
  wpi::tunables::TunableDouble children{3.0};
  wpi::tunables::Publish("child/value", child);
  wpi::tunables::Publish("children/value", children);

  auto childBackend = std::make_shared<wpi::tunables::MockTunableBackend>();
  wpi::tunables::TunableRegistry::RegisterBackend("/child", childBackend);

  CHECK_THROWS_AS(backend->SetDouble("/child/value", 4.0),
                  std::invalid_argument);
  CHECK(backend->GetDouble("/children/value") == 3.0);
  CHECK_THROWS_AS(childBackend->GetDouble("/children/value"),
                  std::invalid_argument);

  childBackend->SetDouble("/child/value", 5.0);
  backend->SetDouble("/children/value", 6.0);
  wpi::tunables::TunableRegistry::Update();

  CHECK(child.Get() == 5.0);
  CHECK(children.Get() == 6.0);
}

TEST_CASE_METHOD(
    TunableTest,
    "TunableTest RegisterBackendReplacementMigratesExistingTunables",
    "[tunable]") {
  wpi::tunables::TunableDouble tunable{1.0};
  wpi::tunables::Publish("value", tunable);

  auto replacementBackend =
      std::make_shared<wpi::tunables::MockTunableBackend>();
  wpi::tunables::TunableRegistry::RegisterBackend("", replacementBackend);

  CHECK_THROWS_AS(backend->SetDouble("/value", 2.0), std::invalid_argument);
  replacementBackend->SetDouble("/value", 3.0);
  wpi::tunables::TunableRegistry::Update();

  CHECK(tunable.Get() == 3.0);
}

TEST_CASE_METHOD(
    TunableTest,
    "TunableTest RegisterBackendReplacementRetiresDisplacedBackend",
    "[tunable]") {
  wpi::tunables::TunableRegistry::Reset();
  auto retiringBackend = std::make_shared<RetiringMockTunableBackend>();
  wpi::tunables::TunableRegistry::RegisterBackend("", retiringBackend);
  wpi::tunables::TunableDouble tunable{1.0};
  wpi::tunables::Publish("value", tunable);

  auto replacementBackend =
      std::make_shared<wpi::tunables::MockTunableBackend>();
  wpi::tunables::TunableRegistry::RegisterBackend("", replacementBackend);

  CHECK_FALSE(retiringBackend->GetUid("/value"));
  CHECK(retiringBackend->retireCount == 1);

  replacementBackend->SetDouble("/value", 2.0);
  wpi::tunables::TunableRegistry::Update();

  CHECK(tunable.Get() == 2.0);
}

TEST_CASE_METHOD(
    TunableTest,
    "TunableTest RegisterBackendReplacementKeepsStillRegisteredBackendOpen",
    "[tunable]") {
  wpi::tunables::TunableRegistry::Reset();
  auto sharedBackend = std::make_shared<RetiringMockTunableBackend>();
  wpi::tunables::TunableRegistry::RegisterBackend("", sharedBackend);
  wpi::tunables::TunableRegistry::RegisterBackend("/retained", sharedBackend);
  wpi::tunables::TunableDouble tunable{1.0};
  wpi::tunables::Publish("retained/value", tunable);

  auto replacementBackend =
      std::make_shared<wpi::tunables::MockTunableBackend>();
  wpi::tunables::TunableRegistry::RegisterBackend("", replacementBackend);

  CHECK(sharedBackend->retireCount == 0);
  REQUIRE(sharedBackend->GetUid("/retained/value"));
}

TEST_CASE_METHOD(TunableTest,
                 "TunableTest RegisterBackendMigratesComplexTunable",
                 "[tunable]") {
  MemberComplex complex;
  wpi::tunables::Publish("child/complex", complex);

  auto childBackend = std::make_shared<wpi::tunables::MockTunableBackend>();
  wpi::tunables::TunableRegistry::RegisterBackend("/child", childBackend);

  CHECK_THROWS_AS(backend->SetInt32("/child/complex/gain", 2),
                  std::invalid_argument);
  childBackend->SetInt32("/child/complex/gain", 4);
  childBackend->SetStruct<TestStruct>("/child/complex/point", {5, 6});
  wpi::tunables::TunableRegistry::Update();

  CHECK(complex.gain == 4);
  CHECK(complex.point.a == 5);
  CHECK(complex.point.b == 6);
  CHECK(complex.updateCount == 1);
}

TEST_CASE_METHOD(TunableTest,
                 "TunableTest MockBackendRemovePrefixReturnsMatchingTunables",
                 "[tunable]") {
  wpi::tunables::TunableDouble root{1.0};
  wpi::tunables::TunableDouble child{2.0};
  wpi::tunables::TunableDouble children{3.0};
  wpi::tunables::Publish("root", root);
  wpi::tunables::Publish("child/value", child);
  wpi::tunables::Publish("children/value", children);

  auto removed = backend->RemovePrefix("child");

  REQUIRE(removed.size() == 1u);
  CHECK(removed[0].path == "/child/value");
  backend->SetDouble("/root", 3.0);
  CHECK_THROWS_AS(backend->SetDouble("/child/value", 4.0),
                  std::invalid_argument);
  backend->SetDouble("/children/value", 5.0);
  wpi::tunables::TunableRegistry::Update();
  CHECK(root.Get() == 3.0);
  CHECK(child.Get() == 2.0);
  CHECK(children.Get() == 5.0);
}

TEST_CASE_METHOD(TunableTest, "TunableTest MockBackendNormalizesPaths",
                 "[tunable]") {
  wpi::tunables::TunableDouble value{1.0};
  wpi::tunables::Publish("child/value", value);

  REQUIRE(backend->GetUid("child//value").has_value());
  CHECK(backend->GetDouble("//child/value") == 1.0);

  backend->SetDouble("child//value", 2.0);
  wpi::tunables::TunableRegistry::Update();

  CHECK(value.Get() == 2.0);
  CHECK(backend->GetDouble("/child/value") == 2.0);

  backend->Remove("child//value");

  CHECK_THROWS_AS(backend->GetDouble("/child/value"), std::invalid_argument);
}

TEST_CASE_METHOD(TunableTest,
                 "TunableTest ComplexTunablePublishesMembersAndUpdates",
                 "[tunable]") {
  MemberComplex complex;
  wpi::tunables::Publish("complex", complex);

  backend->SetInt32("/complex/gain", 10);
  backend->SetStruct<TestStruct>("/complex/point", {11, 12});
  wpi::tunables::TunableRegistry::Update();

  CHECK(complex.gain == 10);
  CHECK(complex.point.a == 11);
  CHECK(complex.point.b == 12);
  CHECK(complex.updateCount == 1);
}

TEST_CASE_METHOD(TunableTest,
                 "TunableTest RejectedComplexTunableDoesNotPublishMembers",
                 "[tunable]") {
  wpi::tunables::TunableDouble existing{1.0};
  wpi::tunables::Publish("complex", existing);

  {
    MemberComplex rejected;
    wpi::tunables::Publish("complex", rejected);

    CHECK(backend->GetDouble("/complex") == 1.0);
    CHECK(backend->GetUid("/complex").has_value());
    CHECK_FALSE(backend->GetUid("/complex/gain").has_value());
    CHECK_FALSE(backend->GetUid("/complex/point").has_value());
  }

  CHECK(backend->GetDouble("/complex") == 1.0);
  CHECK(backend->GetUid("/complex").has_value());
  CHECK_FALSE(backend->GetUid("/complex/gain").has_value());
  CHECK_FALSE(backend->GetUid("/complex/point").has_value());
}

TEST_CASE_METHOD(
    TunableTest,
    "TunableTest ComplexTunableAliasesUpdateOncePerRegistryCycleAcrossBackends",
    "[tunable]") {
  struct CountingComplex : wpi::tunables::ComplexTunable {
    void PublishTunable(wpi::tunables::TunableTable&) override {}

    void UpdateTunable() const override {
      ++const_cast<CountingComplex*>(this)->updateCount;
    }

    int updateCount = 0;
  };

  auto childBackend = std::make_shared<wpi::tunables::MockTunableBackend>();
  wpi::tunables::TunableRegistry::RegisterBackend("/child", childBackend);

  CountingComplex complex;
  wpi::tunables::Publish("first", complex);
  wpi::tunables::Publish("child/second", complex);

  wpi::tunables::TunableRegistry::Update();
  CHECK(complex.updateCount == 1);

  wpi::tunables::TunableRegistry::Update();
  CHECK(complex.updateCount == 2);
}

TEST_CASE_METHOD(
    TunableTest,
    "TunableTest ComplexTunableDynamicChildrenUseAllActiveParentPaths",
    "[tunable]") {
  DynamicComplex complex;
  wpi::tunables::Publish("first", complex);
  wpi::tunables::Publish("second", complex);

  complex.PublishDynamic();

  CHECK(backend->GetDouble("/first/dynamic") == 2.0);
  CHECK(backend->GetDouble("/second/dynamic") == 2.0);

  wpi::tunables::Remove("first");
  complex.PublishLater();

  CHECK_THROWS_AS(backend->GetDouble("/first/later"), std::invalid_argument);
  CHECK(backend->GetDouble("/second/later") == 3.0);

  complex.RemoveDynamic();

  CHECK_THROWS_AS(backend->GetDouble("/first/dynamic"), std::invalid_argument);
  CHECK_THROWS_AS(backend->GetDouble("/second/dynamic"), std::invalid_argument);
  CHECK(backend->GetDouble("/second/initial") == 1.0);
}

TEST_CASE_METHOD(TunableTest,
                 "TunableTest ComplexUpdateCanPublishAndRemoveChildren",
                 "[tunable]") {
  UpdatingDynamicComplex complex;
  wpi::tunables::Publish("a", complex);

  wpi::tunables::TunableRegistry::Update();

  CHECK(backend->GetDouble("/a/dynamic") == 2.0);

  wpi::tunables::TunableRegistry::Update();

  CHECK_THROWS_AS(backend->GetDouble("/a/dynamic"), std::invalid_argument);
  CHECK(backend->GetDouble("/a/initial") == 1.0);
}

TEST_CASE_METHOD(
    TunableTest,
    "TunableTest RemoveChildTunableUnregistersMemberVariableWrapper",
    "[tunable]") {
  RemovableMemberComplex complex;
  wpi::tunables::Publish("removableMember", complex);
  auto table = wpi::tunables::GetTable("removableMember");

  for (int i = 0; i < 3; ++i) {
    auto uid = backend->GetUid("/removableMember/gain");
    REQUIRE(uid);

    complex.RemoveGain();

    CHECK_FALSE(backend->GetUid("/removableMember/gain"));
    CHECK_FALSE(wpi::tunables::TunableRegistry::GetTunable(*uid));

    complex.PublishGain(table);
  }

  auto uid = backend->GetUid("/removableMember/gain");
  REQUIRE(uid);
  auto info = wpi::tunables::TunableRegistry::GetTunable(*uid);
  REQUIRE(static_cast<bool>(info));

  complex.MarkGainChanged();

  CHECK(info.IsChanged());
}

TEST_CASE_METHOD(TunableTest, "TunableTest RemoveComplexTunableRemovesMembers",
                 "[tunable]") {
  MemberComplex complex;
  wpi::tunables::Publish("complex", complex);

  wpi::tunables::Remove("complex");

  CHECK_THROWS_AS(backend->SetInt32("/complex/gain", 10),
                  std::invalid_argument);
  CHECK_THROWS_AS(backend->SetStruct<TestStruct>("/complex/point", {11, 12}),
                  std::invalid_argument);
  wpi::tunables::TunableRegistry::Update();

  CHECK(complex.gain == 1);
  CHECK(complex.point.a == 2);
  CHECK(complex.point.b == 3);
  CHECK(complex.updateCount == 0);
}

TEST_CASE_METHOD(TunableTest,
                 "TunableTest ComplexTunableDirectStructSerializableMember",
                 "[tunable]") {
  DirectStructComplex complex;
  wpi::tunables::Publish("directStruct", complex);

  backend->SetStruct<TestStruct>("/directStruct/point", {4, 5});
  wpi::tunables::TunableRegistry::Update();

  CHECK(complex.point.a == 4);
  CHECK(complex.point.b == 5);
}

TEST_CASE_METHOD(TunableTest,
                 "TunableTest ComplexTunableWrappedStructSerializableMember",
                 "[tunable]") {
  WrappedStructComplex complex;
  wpi::tunables::Publish("wrappedStruct", complex);

  backend->SetStruct<TestStruct>("/wrappedStruct/point", {6, 7});
  wpi::tunables::TunableRegistry::Update();

  CHECK(complex.point.Get().a == 6);
  CHECK(complex.point.Get().b == 7);
}

TEST_CASE_METHOD(TunableTest, "TunableTest wpi::tunables::CustomTunable",
                 "[tunable]") {
  wpi::tunables::Tunable<CustomType> tunable;

  tunable.Set(CustomType{42});
  auto val = tunable.Get();
  CHECK(val.val == 42);

  tunable = CustomType{63};
  val = tunable;
  CHECK(val.val == 63);

  wpi::tunables::Publish("testCustom", tunable);
  backend->SetInt32("/testCustom", 84);
  wpi::tunables::TunableRegistry::Update();
  val = tunable.Get();
  CHECK(val.val == 84);
}

TEST_CASE_METHOD(TunableTest, "TunableTest StructTunable", "[tunable]") {
  wpi::tunables::Tunable<TestStruct> tunable;

  tunable.Set({1, 2});
  auto val = tunable.Get();
  CHECK(val.a == 1);
  CHECK(val.b == 2);

  tunable = {2, 3};
  val = tunable;
  CHECK(val.a == 2);
  CHECK(val.b == 3);

  wpi::tunables::Publish("testStruct", tunable);
  backend->SetStruct<TestStruct>("/testStruct", {3, 4});
  wpi::tunables::TunableRegistry::Update();
  val = tunable.Get();
  CHECK(val.a == 3);
  CHECK(val.b == 4);
}

TEST_CASE_METHOD(TunableTest, "TunableTest StructVectorTunable", "[tunable]") {
  wpi::tunables::Tunable<std::vector<TestStruct>> tunable;

  tunable.Set({{1, 2}, {3, 4}});
  auto val = tunable.Get();
  CHECK(val.size() == 2u);
  CHECK(val[0].a == 1);
  CHECK(val[0].b == 2);
  CHECK(val[1].a == 3);
  CHECK(val[1].b == 4);

  tunable = {{2, 3}, {4, 5}, {6, 7}};
  val = tunable;
  CHECK(val.size() == 3u);
  CHECK(val[0].a == 2);
  CHECK(val[0].b == 3);
  CHECK(val[1].a == 4);
  CHECK(val[1].b == 5);
  CHECK(val[2].a == 6);
  CHECK(val[2].b == 7);

  wpi::tunables::Publish("testStructVector", tunable);
  backend->SetStructVector<TestStruct>("/testStructVector", {{{5, 6}, {7, 8}}});
  wpi::tunables::TunableRegistry::Update();
  val = tunable.Get();
  CHECK(val.size() == 2u);
  CHECK(val[0].a == 5);
  CHECK(val[0].b == 6);
  CHECK(val[1].a == 7);
  CHECK(val[1].b == 8);
}

TEST_CASE_METHOD(TunableTest, "TunableTest StructArrayTunable", "[tunable]") {
  wpi::tunables::Tunable<std::array<TestStruct, 2>> tunable;

  tunable.Set({{{1, 2}, {3, 4}}});
  auto val = tunable.Get();
  CHECK(val.size() == 2u);
  CHECK(val[0].a == 1);
  CHECK(val[0].b == 2);
  CHECK(val[1].a == 3);
  CHECK(val[1].b == 4);

  tunable = {{{2, 3}, {4, 5}}};
  val = tunable;
  CHECK(val.size() == 2u);
  CHECK(val[0].a == 2);
  CHECK(val[0].b == 3);
  CHECK(val[1].a == 4);
  CHECK(val[1].b == 5);

  wpi::tunables::Publish("testStructArray", tunable);
  backend->SetStructVector<TestStruct>("/testStructArray", {{{5, 6}, {7, 8}}});
  wpi::tunables::TunableRegistry::Update();
  val = tunable.Get();
  CHECK(val.size() == 2u);
  CHECK(val[0].a == 5);
  CHECK(val[0].b == 6);
  CHECK(val[1].a == 7);
  CHECK(val[1].b == 8);
}
