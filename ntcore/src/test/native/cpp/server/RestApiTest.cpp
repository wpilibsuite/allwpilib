// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "server/RestApi.hpp"

#include <array>
#include <limits>
#include <string>
#include <string_view>
#include <utility>

#include <catch2/catch_test_macros.hpp>

#include "../MockLogger.hpp"
#include "server/RestCodec.hpp"
#include "server/ServerClientLocal.hpp"
#include "server/ServerStorage.hpp"
#include "wpi/util/json.hpp"
#include "wpi/util/raw_ostream.hpp"

using namespace wpi::nt;
using namespace wpi::nt::server;
using wpi::util::json;

namespace {
class RestApiTest {
 public:
  RestResponse Request(std::string_view method, std::string_view body = {},
                       std::string_view target = "/nt/v1/topics/%2Ftest") {
    return HandleRestRequest(storage, method, target, body);
  }

  json Read(std::string_view target = "/nt/v1/topics/%2Ftest") {
    auto response = Request("GET", {}, target);
    REQUIRE(response.status == 200);
    auto result = json::parse(response.body);
    REQUIRE(result);
    return *result;
  }

  wpi::MockLogger logger;
  ServerStorage storage{logger, [](auto, auto) {}};
};
}  // namespace

TEST_CASE_METHOD(RestApiTest, "REST topic lifecycle", "[ntcore][rest]") {
  CHECK(Request("GET").status == 404);
  CHECK(Request("PUT", R"({"type":"int","value":42})").status == 201);
  CHECK(Read().at("value") == 42);
  CHECK(Read().at("properties").at("retained") == true);
  CHECK(Read().at("timestamp").get_int() > 0);
  CHECK(Request("PUT", R"({"value":43})").status == 200);
  CHECK(Read().at("value") == 43);
  CHECK(Request("PATCH", R"({"properties":{"unit":"m","persistent":true}})")
            .status == 204);
  CHECK(storage.PersistentChanged());
  CHECK(Read().at("properties").at("unit") == "m");
  CHECK(Request("PATCH", R"({"properties":{"unit":null}})").status == 204);
  CHECK(storage.PersistentChanged());
  CHECK_FALSE(Read().at("properties").lookup("unit"));
  CHECK(Request("DELETE").status == 204);
  CHECK(storage.PersistentChanged());
  CHECK(Request("GET").status == 404);
  CHECK(Request("DELETE").status == 404);
  CHECK(storage.GetTopic("$pub$/test") == nullptr);
}

TEST_CASE_METHOD(RestApiTest, "REST value types", "[ntcore][rest]") {
  struct TestValue {
    std::string_view type;
    std::string_view value;
  };
  for (auto [type, value] :
       {TestValue{"boolean", "true"}, TestValue{"int", "9223372036854775807"},
        TestValue{"int", "-9223372036854775808"}, TestValue{"float", "1.25"},
        TestValue{"double", "1.25"}, TestValue{"string", R"("hello\nworld")"},
        TestValue{"json", R"("{\"a\":1}")"}, TestValue{"raw", R"("AP8=")"},
        TestValue{"msgpack", R"("gA==")"},
        TestValue{"struct:Example", R"("AP8=")"},
        TestValue{"boolean[]", "[true,false]"}, TestValue{"int[]", "[1,-2]"},
        TestValue{"float[]", "[1.25,-2.5]"},
        TestValue{"double[]", "[1.25,-2.5]"},
        TestValue{"string[]", R"(["a","b"])"}, TestValue{"int[]", "[]"}}) {
    INFO(type << ": " << value);
    auto input = json::object("type", type, "value", *json::parse(value));
    REQUIRE(Request("PUT", input.to_string()).status == 201);
    auto result = Read();
    CHECK(result.at("type") == type);
    CHECK(result.at("value") == *json::parse(value));
    REQUIRE(Request("PATCH", R"({"properties":{"persistent":true}})").status ==
            204);
    auto file = Request("GET", {}, "/nt/v1/persistent.json");
    REQUIRE(file.status == 200);
    REQUIRE(Request("DELETE").status == 204);
    REQUIRE(Request("PUT", file.body, "/nt/v1/persistent.json").status == 204);
    CHECK(Read().at("type") == type);
    CHECK(Read().at("value") == *json::parse(value));
    ServerStorage restored{logger, [](auto, auto) {}};
    REQUIRE(restored.LoadPersistent(file.body).empty());
    REQUIRE(restored.GetTopic("/test"));
    CHECK(restored.GetTopic("/test")->lastValue ==
          storage.GetTopic("/test")->lastValue);
    REQUIRE(Request("DELETE").status == 204);
  }
}

TEST_CASE_METHOD(RestApiTest, "REST invalid writes are atomic",
                 "[ntcore][rest]") {
  for (auto body : {"{",
                    "[]",
                    "{}",
                    R"({"value":2})",
                    R"({"type":"","value":2})",
                    R"({"type":"int","value":1.5})",
                    R"({"type":"int","value":9223372036854775808})",
                    R"({"type":"boolean","value":1})",
                    R"({"type":"string","value":false})",
                    R"({"type":"float","value":1e100})",
                    R"({"type":"double","value":null})",
                    R"({"type":"raw","value":"!"})",
                    R"({"type":"raw","value":"AP8=extra"})",
                    R"({"type":"raw","value":"AP9="})",
                    R"({"type":"int[]","value":[1,"bad"]})",
                    R"({"type":"boolean[]","value":[true,1]})",
                    R"({"type":"float[]","value":[1,1e100]})",
                    R"({"type":"double[]","value":[1,false]})",
                    R"({"type":"string[]","value":["a",2]})",
                    R"({"type":"int","value":1,"properties":[]})",
                    R"({"type":"int","value":1,"properties":{"cached":0}})",
                    R"({"type":"int","value":1,"unexpected":true})"}) {
    INFO(body);
    CHECK(Request("PUT", body).status == 400);
    CHECK(Request("GET").status == 404);
  }
  REQUIRE(Request("PUT", R"({"type":"int","value":1})").status == 201);
  CHECK(Request("PUT", R"({"type":"double","value":2})").status == 409);
  CHECK(Request("PUT", R"({"value":"bad","properties":{"persistent":true}})")
            .status == 400);
  CHECK(Read().at("value") == 1);
  CHECK_FALSE(storage.GetTopic("/test")->persistent);
  CHECK(Request("PATCH", R"({"properties":{},"value":2})").status == 400);
  CHECK(Request("PATCH", R"({"properties":{"retained":false}})").status == 204);
  CHECK(Request("GET").status == 404);
}

TEST_CASE_METHOD(RestApiTest, "REST routing and names", "[ntcore][rest]") {
  CHECK(Request("POST").status == 405);
  CHECK(Request("POST").allow == "GET, PUT, PATCH, DELETE, OPTIONS");
  CHECK(Request("OPTIONS").status == 204);
  CHECK(Request("PUT", {}, "/nt/v1/topics").status == 405);
  CHECK(Request("GET", {}, "/nt/v1/unknown").status == 404);
  CHECK(Request("GET", {}, "/nt/v1/topics/").status == 400);
  CHECK(Request("GET", {}, "/nt/v1/topics/%").status == 400);
  CHECK(Request("GET", {}, "/nt/v1/topics/%zz").status == 400);
  auto path = "/nt/v1/topics/%2Ftest%2F..%2F+%20%3F%23%25%2F%C2%B5";
  REQUIRE(Request("PUT", R"({"type":"int","value":1})", path).status == 201);
  CHECK(Read(path).at("name") == "/test/../+ ?#%/\xc2\xb5");
  CHECK(Read("/nt/v1/topics?prefix=%2Ftest").get_array().size() == 1);
  CHECK(Read("/nt/v1/topics?prefix=other").get_array().empty());
  CHECK(Read("/nt/v1/topics").get_array().size() == 1);
  CHECK(Read("/nt/v1/topics?prefix=%24pub%24").get_array().size() == 1);
  CHECK(
      Request("PUT", R"({"type":"int","value":1})", "/nt/v1/topics/%24clients")
          .status == 403);
  CHECK(Request("DELETE", {}, "/nt/v1/topics/%24pub%24%2Ftest").status == 403);
}

TEST_CASE_METHOD(RestApiTest, "REST respects publisher and cache state",
                 "[ntcore][rest]") {
  auto topic = storage.CreateTopic(nullptr, "/test", "double", json::object());
  topic->publisherCount = 1;
  CHECK(Request("DELETE").status == 409);
  CHECK(Request("PUT", R"({"value":1})").status == 200);
  CHECK(Read().at("value").get_number() == 1);
  CHECK_FALSE(topic->retained);
  CHECK(Request("PATCH", R"({"properties":{"cached":false}})").status == 204);
  CHECK(Request("PUT", R"({"value":2})").status == 200);
  CHECK(Read().at("value").is_null());
  CHECK(Read().at("timestamp") == 0);
  CHECK(Request("PATCH", R"({"properties":{"cached":true}})").status == 204);
  storage.SetValue(nullptr, topic,
                   Value::MakeDouble(std::numeric_limits<double>::infinity()));
  CHECK(Read().at("value").is_null());
}

TEST_CASE_METHOD(RestApiTest, "REST creation properties", "[ntcore][rest]") {
  CHECK(Request("PUT",
                R"({"type":"int","value":1,"properties":{"retained":false}})")
            .status == 400);
  CHECK(Request("GET").status == 404);
  REQUIRE(
      Request(
          "PUT",
          R"({"type":"int","value":1,"properties":{"retained":null,"persistent":true,"unit":null}})")
          .status == 201);
  CHECK(storage.PersistentChanged());
  CHECK_FALSE(Read().at("properties").lookup("retained"));
  CHECK_FALSE(Read().at("properties").lookup("unit"));
  CHECK(Request("PUT", R"({"value":2,"properties":{"cached":false}})").status ==
        200);
  CHECK(Read().at("value").is_null());
  CHECK(Request("PUT", R"({"value":3,"properties":{"cached":true}})").status ==
        200);
  CHECK(Read().at("value") == 3);
  CHECK(Request("PUT", R"({"value":4,"properties":{"persistent":false}})")
            .status == 204);
  CHECK(Request("GET").status == 404);
}

TEST_CASE_METHOD(RestApiTest, "REST direct topic resources", "[ntcore][rest]") {
  REQUIRE(
      Request("PUT", R"({"type":"int","value":42,"properties":{"unit":"m"}})")
          .status == 201);
  CHECK(Read("/nt/v1/topics/%2Ftest?type") == "int");
  CHECK(Read("/nt/v1/topics/%2Ftest?name") == "/test");
  CHECK(Read("/nt/v1/topics/%2Ftest?value") == 42);
  CHECK(Read("/nt/v1/topics/%2Ftest?timestamp").get_int() > 0);
  CHECK(Read("/nt/v1/topics/%2Ftest?properties").at("unit") == "m");
  CHECK(Request("PUT", "43", "/nt/v1/topics/%2Ftest?value").status == 204);
  CHECK(Read().at("value") == 43);
  CHECK(Request("PUT", R"("wrong")", "/nt/v1/topics/%2Ftest?value").status ==
        400);
  CHECK(Request("PUT", R"("double")", "/nt/v1/topics/%2Ftest?type").status ==
        405);
  CHECK(Request("DELETE", {}, "/nt/v1/topics/%2Ftest?value").status == 405);
  CHECK(Request("PUT", "1", "/nt/v1/topics/missing?value").status == 404);
  CHECK(Request("GET", {}, "/nt/v1/topics/%2Ftest/unknown").status == 404);
  CHECK(Request("PATCH", R"({"unit":null,"label":"distance"})",
                "/nt/v1/topics/%2Ftest?properties")
            .status == 204);
  CHECK_FALSE(Read().at("properties").lookup("unit"));
  CHECK(
      Request("PUT", R"({"retained":true})", "/nt/v1/topics/%2Ftest?properties")
          .status == 204);
  CHECK(Read().at("properties") == json::object("retained", true));
  CHECK(Request("PUT", "{}", "/nt/v1/topics/%2Ftest?properties").status == 204);
  CHECK(Request("GET").status == 404);
}

TEST_CASE_METHOD(RestApiTest, "REST query selectors do not shadow topic names",
                 "[ntcore][rest]") {
  REQUIRE(Request("PUT", R"({"type":"int","value":42})").status == 201);
  for (auto field : {"name", "type", "timestamp", "value", "properties"}) {
    INFO(field);
    auto barePath = std::string{"/nt/v1/topics/"} + field;
    REQUIRE(Request("PUT", R"({"type":"int","value":1})", barePath).status ==
            201);
    CHECK(Read(barePath).at("name") == field);
    CHECK(Read(barePath + "?name") == field);
    // Child topic names can match field selectors, including when slashes are
    // only partially encoded or left literal.
    auto childPath = std::string{"/nt/v1/topics/%2Ftest/"} + field;
    auto childName = std::string{"/test/"} + field;
    REQUIRE(Request("PUT", R"({"type":"int","value":2})", childPath).status ==
            201);
    CHECK(Read(childPath).at("name") == childName);
    CHECK(Read(childPath + "?name") == childName);
    CHECK(Read(std::string{"/nt/v1/topics/%2Ftest%2F"} + field).at("value") ==
          2);
    CHECK(Read(std::string{"/nt/v1/topics//test/"} + field + "?value") == 2);
    CHECK(Request("PUT", "3", childPath + "?value").status == 204);
    CHECK(Read(childPath + "?value") == 3);
    CHECK(Read(barePath + "?value") == 1);
    CHECK(Read().at("value") == 42);
    CHECK(Request("DELETE", {}, childPath).status == 204);
    CHECK(Read().at("value") == 42);
  }
  REQUIRE(Request("PUT", R"({"type":"int","value":4})",
                  "/nt/v1/topics/%2Ftest%3Fvalue")
              .status == 201);
  CHECK(Read("/nt/v1/topics/%2Ftest%3Fvalue?name") == "/test?value");
  CHECK(Read("/nt/v1/topics/%2Ftest%3Fvalue?value") == 4);
  CHECK(Read("/nt/v1/topics/%2Ftest?value") == 42);
}

TEST_CASE_METHOD(RestApiTest, "REST validates topic field selectors",
                 "[ntcore][rest]") {
  REQUIRE(Request("PUT", R"({"type":"int","value":42})").status == 201);
  CHECK(Read("/nt/v1/topics/%2Ftest?").at("value") == 42);
  CHECK(Read("/nt/v1/topics/%2Ftest?value=") == 42);
  CHECK(Read("/nt/v1/topics/%2Ftest?%76alue") == 42);
  auto original = Read();
  for (auto selector :
       {"?unknown", "?value&name", "?value&value", "?value=1", "?properties=1",
        "?value&unknown", "?timestamp&type", "??value", "?%zz", "?Value"}) {
    auto path = std::string{"/nt/v1/topics/%2Ftest"} + selector;
    INFO(path);
    for (auto method : {"GET", "PUT", "PATCH", "DELETE", "OPTIONS"}) {
      INFO(method);
      CHECK(Request(method, R"({"value":99})", path).status == 400);
      CHECK(Read() == original);
    }
  }
  for (auto field : {"name", "type", "timestamp"}) {
    auto path = std::string{"/nt/v1/topics/%2Ftest?"} + field;
    for (auto method : {"PUT", "PATCH", "DELETE"}) {
      CHECK(Request(method, "99", path).status == 405);
    }
    CHECK(Request("OPTIONS", {}, path).allow == "GET, OPTIONS");
  }
  CHECK(Request("OPTIONS", {}, "/nt/v1/topics/%2Ftest?value").allow ==
        "GET, PUT, OPTIONS");
  CHECK(Request("OPTIONS", {}, "/nt/v1/topics/%2Ftest?properties").allow ==
        "GET, PUT, PATCH, OPTIONS");
}

TEST_CASE_METHOD(RestApiTest, "REST content negotiation", "[ntcore][rest]") {
  REQUIRE(Request("PUT", R"({"type":"int","value":42})").status == 201);
  auto get = [&](std::string_view accept) {
    return HandleRestRequest(storage, "GET", "/nt/v1/topics/%2Ftest?value", {},
                             {}, accept);
  };
  for (auto accept : {"application/msgpack", "application/x-msgpack",
                      "application/json;q=0.2, application/msgpack;q=0.8",
                      "application/json;q=0, */*", "APPLICATION/MSGPACK"}) {
    INFO(accept);
    auto response = get(accept);
    REQUIRE(response.status == 200);
    CHECK(response.contentType == "application/msgpack");
    CHECK(response.body == "*");  // MessagePack positive fixint 42
  }
  for (auto accept : {"", "*/*", "application/*", "application/json",
                      "application/msgpack;q=0.1, application/json;q=0.9"}) {
    auto response = get(accept);
    CHECK(response.status == 200);
    CHECK(response.contentType == "application/json");
    CHECK(response.body == "42");
  }
  CHECK(get("text/html").status == 406);
  CHECK(get("application/json;q=0, application/msgpack;q=0, */*;q=1").status ==
        406);
  CHECK(get("application/msgpack;q=invalid").status == 406);
  CHECK(HandleRestRequest(storage, "PUT", "/nt/v1/topics/%2Ftest?value", "1",
                          "text/plain")
            .status == 415);
  CHECK(HandleRestRequest(storage, "PUT", "/nt/v1/topics/%2Ftest",
                          R"({"value":99})", "application/json", "text/plain")
            .status == 406);
  CHECK(Read().at("value") == 42);
}

TEST_CASE_METHOD(RestApiTest, "REST MessagePack scalar and aggregate writes",
                 "[ntcore][rest]") {
  auto body = RestEncodeMessagePack(json::object("type", "int", "value", 42));
  REQUIRE(body);
  auto response =
      HandleRestRequest(storage, "PUT", "/nt/v1/topics/%2Ftest", *body,
                        "application/msgpack", "application/msgpack");
  REQUIRE(response.status == 201);
  auto decoded = RestDecodeMessagePack(response.body);
  REQUIRE(decoded);
  CHECK(decoded->at("value") == 42);
  auto path = "/nt/v1/topics/%2Ftest?value";
  CHECK(HandleRestRequest(storage, "PUT", path, std::string_view{"\xd0\xfe", 2},
                          "application/msgpack")
            .status == 204);
  CHECK(Read().at("value") == -2);
  CHECK(HandleRestRequest(storage, "PUT", path, std::string_view{"\xc3", 1},
                          "application/msgpack")
            .status == 400);
  CHECK(Read().at("value") == -2);
}

TEST_CASE_METHOD(RestApiTest, "REST translates structured values",
                 "[ntcore][rest]") {
  auto topic = storage.CreateTopic(nullptr, "/test", "msgpack",
                                   json::object("retained", true));
  // {"a":[1,true,null]} encoded independently of the REST encoder.
  std::string bytes{
      "\x81\xa1"
      "a"
      "\x93\x01\xc3\xc0",
      7};
  storage.SetValue(
      nullptr, topic,
      Value::MakeRaw(std::span{reinterpret_cast<const uint8_t*>(bytes.data()),
                               bytes.size()}));
  auto path = "/nt/v1/topics/%2Ftest?value";
  CHECK(Read(path) == json::object("a", json::array(1, true, nullptr)));
  auto response =
      HandleRestRequest(storage, "GET", path, {}, {}, "application/msgpack");
  CHECK(response.body == bytes);
  CHECK(Request("PUT", R"({"answer":42})", path).status == 204);
  auto stored = topic->lastValue.GetRaw();
  auto decoded = RestDecodeMessagePack(
      {reinterpret_cast<const char*>(stored.data()), stored.size()});
  REQUIRE(decoded);
  CHECK(*decoded == json::object("answer", 42));
  CHECK(HandleRestRequest(storage, "PUT", path, bytes, "application/msgpack")
            .status == 204);
  CHECK(Read(path) == json::object("a", json::array(1, true, nullptr)));
  // Native MessagePack binary is preserved, but has no lossless JSON
  // equivalent.
  std::string binary{"\xc4\x02\x00\xff", 4};
  CHECK(HandleRestRequest(storage, "PUT", path, binary, "application/msgpack")
            .status == 204);
  CHECK(Request("GET", {}, path).status == 406);
  CHECK(HandleRestRequest(storage, "GET", path, {}, {}, "application/msgpack")
            .body == binary);
  CHECK(HandleRestRequest(storage, "PUT", path, bytes + bytes,
                          "application/msgpack")
            .status == 400);
  REQUIRE(Request("DELETE").status == 204);
  topic = storage.CreateTopic(nullptr, "/test", "json",
                              json::object("retained", true));
  CHECK(HandleRestRequest(storage, "PUT", path, bytes, "application/msgpack")
            .status == 204);
  CHECK(*json::parse(topic->lastValue.GetString()) ==
        json::object("a", json::array(1, true, nullptr)));
  CHECK(Read(path) == json::object("a", json::array(1, true, nullptr)));
  REQUIRE(Request("DELETE").status == 204);
  topic = storage.CreateTopic(nullptr, "/test", "raw",
                              json::object("retained", true));
  CHECK(HandleRestRequest(storage, "PUT", path, binary, "application/msgpack")
            .status == 204);
  CHECK(Read(path) == "AP8=");
  CHECK(HandleRestRequest(storage, "GET", path, {}, {}, "application/msgpack")
            .body == binary);
}

TEST_CASE_METHOD(RestApiTest, "REST MessagePack validation", "[ntcore][rest]") {
  auto topic = storage.CreateTopic(nullptr, "/test", "msgpack",
                                   json::object("retained", true));
  auto path = "/nt/v1/topics/%2Ftest?value";
  for (auto bytes :
       {std::string{}, std::string{"\xc1", 1},
        std::string{"\x81\xa1"
                    "a",
                    3},
        std::string{"\xdd\xff\xff\xff\xff", 5},
        std::string{"\xc6\xff\xff\xff\xff", 5}, std::string{"\xc7\x00\x01", 3},
        std::string(100, '\x91') + "\xc0"}) {
    CHECK_FALSE(RestValidateMessagePack(bytes));
    CHECK_FALSE(RestDecodeMessagePack(bytes));
    CHECK(HandleRestRequest(storage, "PUT", path, bytes, "application/msgpack")
              .status == 400);
    CHECK_FALSE(topic->lastValue);
  }
  // Valid MessagePack, but non-string map keys and duplicates cannot become
  // JSON.
  CHECK(RestValidateMessagePack(std::string_view{"\x81\x01\x02", 3}));
  CHECK_FALSE(RestDecodeMessagePack(std::string_view{"\x81\x01\x02", 3}));
  CHECK_FALSE(
      RestDecodeMessagePack(std::string_view{"\x82\xa1"
                                             "a"
                                             "\x01\xa1"
                                             "a"
                                             "\x02",
                                             7}));
  storage.SetValue(nullptr, topic, Value::MakeRaw(std::vector<uint8_t>{0xc1}));
  CHECK(Request("GET", {}, path).status == 422);
  CHECK(HandleRestRequest(storage, "GET", path, {}, {}, "application/msgpack")
            .status == 422);
}

TEST_CASE("REST MessagePack codec preserves numbers and bounds work",
          "[ntcore][rest]") {
  const std::string maxUint{"\xcf\xff\xff\xff\xff\xff\xff\xff\xff", 9};
  auto value = RestDecodeMessagePack(maxUint);
  REQUIRE(value);
  CHECK(value->get_uint() == std::numeric_limits<uint64_t>::max());
  CHECK(RestEncodeMessagePack(*value) == maxUint);
  CHECK_FALSE(RestDecodeMessagePack(std::string_view{"\xa1\xff", 2}));
  CHECK(RestValidateMessagePack(
      std::string_view{"\xcb\x7f\xf0\x00\x00\x00\x00\x00\x00", 9}));
  CHECK_FALSE(RestDecodeMessagePack(
      std::string_view{"\xcb\x7f\xf0\x00\x00\x00\x00\x00\x00", 9}));
  auto array = json::array();
  array.get_array().resize(128 * 1024);
  CHECK_FALSE(RestEncodeMessagePack(array));
  array.get_array().pop_back();
  auto encoded = RestEncodeMessagePack(array);
  REQUIRE(encoded);
  CHECK(RestValidateMessagePack(*encoded));
  auto nested = json::array();
  for (int i = 0; i < 64; ++i) {
    nested = json::array(std::move(nested));
  }
  CHECK_FALSE(RestEncodeMessagePack(nested));
}

TEST_CASE_METHOD(RestApiTest, "REST persistence file merges entries",
                 "[ntcore][rest]") {
  auto update =
      storage.CreateTopic(nullptr, "/update", "int",
                          json::object("persistent", true, "old", "remove"));
  auto keep = storage.CreateTopic(nullptr, "/keep", "int",
                                  json::object("persistent", true));
  auto temporary = storage.CreateTopic(nullptr, "/temporary", "int",
                                       json::object("retained", true));
  storage.SetValue(nullptr, update, Value::MakeInteger(1));
  storage.SetValue(nullptr, keep, Value::MakeInteger(2));
  storage.SetValue(nullptr, temporary, Value::MakeInteger(3));
  auto path = "/nt/v1/persistent.json";
  CHECK(Read(path).get_array().size() == 2);
  std::string dump;
  wpi::util::raw_string_ostream os{dump};
  storage.DumpPersistent(os);
  os.flush();
  CHECK(Request("GET", {}, path).body == dump);
  CHECK(Request("GET", {}, "/nt/persistent.json").body == dump);
  storage.PersistentChanged();
  REQUIRE(Request("PUT", R"([
    {"name":"/update","type":"int","value":10,"properties":{"persistent":true,"unit":"m"}},
    {"name":"/added","type":"string","value":"new","properties":{"persistent":true}}
  ])",
                  path)
              .status == 204);
  CHECK(storage.PersistentChanged());
  CHECK(update->lastValue.GetInteger() == 10);
  CHECK(update->properties == json::object("persistent", true, "unit", "m"));
  CHECK(keep->lastValue.GetInteger() == 2);
  CHECK(keep->persistent);
  CHECK(temporary->lastValue.GetInteger() == 3);
  CHECK_FALSE(temporary->persistent);
  REQUIRE(storage.GetTopic("/added"));
  CHECK(storage.GetTopic("/added")->lastValue.GetString() == "new");
  CHECK(Read(path).get_array().size() == 3);
  REQUIRE(Request("PUT", "[]", path).status == 204);
  CHECK_FALSE(storage.PersistentChanged());
  CHECK(Read(path).get_array().size() == 3);
}

TEST_CASE_METHOD(RestApiTest, "REST persistence preserves floating point files",
                 "[ntcore][rest]") {
  for (auto&& [type, value] :
       {std::pair{"float", Value::MakeFloat(std::numeric_limits<float>::max())},
        std::pair{"float",
                  Value::MakeFloat(std::numeric_limits<float>::infinity())},
        std::pair{"double",
                  Value::MakeDouble(-std::numeric_limits<double>::infinity())},
        std::pair{"float[]", Value::MakeFloatArray(std::array{
                                 std::numeric_limits<float>::max(),
                                 -std::numeric_limits<float>::infinity()})},
        std::pair{"double[]", Value::MakeDoubleArray(std::array{
                                  std::numeric_limits<double>::max(),
                                  std::numeric_limits<double>::infinity()})}}) {
    INFO(type);
    auto topic = storage.CreateTopic(nullptr, "/test", type,
                                     json::object("persistent", true));
    storage.SetValue(nullptr, topic, value);
    auto file = Request("GET", {}, "/nt/v1/persistent.json");
    REQUIRE(file.status == 200);
    REQUIRE(Request("DELETE").status == 204);
    REQUIRE(Request("PUT", file.body, "/nt/v1/persistent.json").status == 204);
    REQUIRE(storage.GetTopic("/test"));
    CHECK(storage.GetTopic("/test")->lastValue == value);
    ServerStorage restored{logger, [](auto, auto) {}};
    REQUIRE(restored.LoadPersistent(file.body).empty());
    REQUIRE(restored.GetTopic("/test"));
    CHECK(restored.GetTopic("/test")->lastValue == value);
    REQUIRE(Request("DELETE").status == 204);
  }
}

TEST_CASE_METHOD(RestApiTest,
                 "REST persistence upload validates the whole file",
                 "[ntcore][rest]") {
  auto path = "/nt/v1/persistent.json";
  auto entry = json::object("name", "/test", "type", "int", "value", 1,
                            "properties", json::object("persistent", true));
  REQUIRE(Request("PUT", json::array(entry).to_string(), path).status == 204);
  auto original = Request("GET", {}, path).body;
  storage.PersistentChanged();
  entry["value"] = 2;
  auto fresh = entry;
  fresh["name"] = "/fresh";
  for (auto [bad, status] :
       {std::pair{json{}, 400}, std::pair{json::object("name", "/bad"), 400},
        std::pair{json::object("name", "/bad", "type", "int", "value", 1,
                               "properties", json::object()),
                  400},
        std::pair{json::object("name", "/bad", "type", "int", "value", 1,
                               "properties", json::object("persistent", false)),
                  400},
        std::pair{json::object(
                      "name", "/bad", "type", "int", "value", 1, "properties",
                      json::object("persistent", true, "cached", false)),
                  400},
        std::pair{json::object("name", "/bad", "type", "int[]", "value",
                               json::array(1, "bad"), "properties",
                               json::object("persistent", true)),
                  400},
        std::pair{json::object("name", "/bad", "type", "raw", "value", "!",
                               "properties", json::object("persistent", true)),
                  400},
        std::pair{json::object("name", "$clients", "type", "int", "value", 1,
                               "properties", json::object("persistent", true)),
                  403},
        std::pair{json::object("name", "/test", "type", "double", "value", 1,
                               "properties", json::object("persistent", true)),
                  409}}) {
    INFO(bad.to_string());
    auto file = json::array(fresh, bad);
    // Include an update before the invalid record to check rollback-free
    // validation.
    if (status != 409) {
      file.get_array().insert(file.get_array().begin(), entry);
    }
    CHECK(Request("PUT", file.to_string(), path).status == status);
    CHECK(Request("GET", {}, path).body == original);
    CHECK_FALSE(storage.GetTopic("/fresh"));
    CHECK_FALSE(storage.PersistentChanged());
  }
  CHECK(Request("PUT", json::array(entry, entry).to_string(), path).status ==
        400);
  CHECK(Request("GET", {}, path).body == original);
  CHECK(Request("PUT", "{}", path).status == 400);
  CHECK(Request("PUT", "[", path).status == 400);
  CHECK(Request("PUT", "", path).status == 400);
  CHECK(Request("DELETE", {}, path).status == 405);
  CHECK(Request("OPTIONS", {}, path).allow == "GET, PUT, OPTIONS");
  CHECK(HandleRestRequest(storage, "PUT", path, "[]", "application/msgpack")
            .status == 415);
  CHECK(HandleRestRequest(storage, "GET", path, {}, {}, "application/msgpack")
            .status == 406);
  CHECK(HandleRestRequest(storage, "GET", path, {}, {},
                          "application/msgpack, application/json;q=0.5")
            .status == 200);
  CHECK(HandleRestRequest(storage, "GET", path, {}, {},
                          "application/json;q=0, */*")
            .status == 406);
}

TEST_CASE_METHOD(RestApiTest, "REST persistence imports update active topics",
                 "[ntcore][rest]") {
  ServerClientLocal client{storage, 0, logger};
  auto topic = storage.CreateTopic(nullptr, "/test", "int",
                                   json::object("retained", true));
  storage.SetValue(&client, topic,
                   Value::MakeInteger(1, std::numeric_limits<int64_t>::max()));
  auto file =
      R"([{"name":"/test","type":"int","value":2,"properties":{"persistent":true}}])";
  REQUIRE(Request("PUT", file, "/nt/persistent.json").status == 204);
  CHECK(topic->lastValue.GetInteger() == 2);
  CHECK(topic->persistent);
  storage.SetProperties(nullptr, topic, json::object("cached", false));
  CHECK_FALSE(topic->lastValue);
  REQUIRE(Request("PUT", file, "/nt/v1/persistent.json").status == 204);
  CHECK(topic->cached);
  CHECK(topic->lastValue.GetInteger() == 2);
  CHECK(topic->properties == json::object("persistent", true));
}
