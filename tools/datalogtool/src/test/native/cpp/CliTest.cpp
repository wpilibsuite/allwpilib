// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "Cli.hpp"

#include <chrono>
#include <fstream>
#include <functional>
#include <string>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "wpi/datalog/DataLogWriter.hpp"
#include "wpi/util/fs.hpp"
#include "wpi/util/raw_ostream.hpp"

namespace {
struct CliTest {
  CliTest() {
    dir = fs::temp_directory_path() /
          ("datalogtool-test-" +
           std::to_string(
               std::chrono::steady_clock::now().time_since_epoch().count()));
    REQUIRE(fs::create_directory(dir));
  }
  ~CliTest() {
    std::error_code ec;
    fs::remove_all(dir, ec);
  }

  std::string WriteLog(
      std::string_view name,
      const std::function<void(wpi::log::DataLogWriter&)>& write) {
    auto path = (dir / name).string();
    std::error_code ec;
    wpi::log::DataLogWriter log{path, ec};
    REQUIRE(!ec);
    write(log);
    log.Stop();
    return path;
  }

  int Run(std::vector<std::string> args) {
    output.clear();
    errors.clear();
    wpi::util::raw_string_ostream out{output};
    wpi::util::raw_string_ostream err{errors};
    return dlt::RunCli(args, out, err);
  }

  std::string Read(std::string_view name) {
    std::ifstream file{dir / name, std::ios::binary};
    REQUIRE(file.is_open());
    return {std::istreambuf_iterator<char>{file},
            std::istreambuf_iterator<char>{}};
  }

  fs::path dir;
  std::string output;
  std::string errors;
};
}  // namespace

TEST_CASE_METHOD(CliTest, "CLI help and argument errors", "[datalogtool]") {
  CHECK(Run({}) == 0);
  CHECK(output.find("download") != std::string::npos);
  CHECK(Run({"--version"}) == 0);
  CHECK(!output.empty());
  for (const auto& command :
       {"export", "entries", "list", "download", "delete"}) {
    CAPTURE(command);
    CHECK(Run({command, "--help"}) == 0);
    CHECK(output.find("Usage:") != std::string::npos);
    CHECK(errors.empty());
  }
  CHECK(Run({"entries", "--", "--not-a-real-log"}) == 1);
  CHECK(errors.find("Unknown argument") == std::string::npos);
  CHECK(errors.find("--not-a-real-log") != std::string::npos);
  const std::vector<std::vector<std::string>> invalid{
      {"unknown"},
      {"export"},
      {"export", "--output-dir", dir.string()},
      {"export", "--style", "bogus", "--output-dir", dir.string(),
       "file.wpilog"},
      {"export", "--unknown"},
      {"export", "--style", "table", "--timestamp-fuzziness", "-1",
       "--output-dir", dir.string(), "file.wpilog"},
      {"export", "--style", "table", "--timestamp-fuzziness", "nan",
       "--output-dir", dir.string(), "file.wpilog"},
      {"export", "--style", "table", "--timestamp-fuzziness", "inf",
       "--output-dir", dir.string(), "file.wpilog"},
      {"export", "--style", "table", "--timestamp-fuzziness", "1e20",
       "--output-dir", dir.string(), "file.wpilog"},
      {"export", "--timestamp-fuzziness", "1", "--output-dir", dir.string(),
       "file.wpilog"},
      {"entries"},
      {"list"},
      {"list", "--host", "localhost", "--", "file.wpilog"},
      {"list", "--host", "localhost", "--port", "0"},
      {"list", "--host", "localhost", "--port", "65536"},
      {"list", "--host", "localhost", "--remote-dir", ""},
      {"download", "--host", "localhost", "--output-dir", dir.string()},
      {"download", "--host", "localhost", "--output-dir", dir.string(), "--all",
       "x.wpilog"},
      {"download", "--host", "localhost", "--output-dir", dir.string(),
       "../x.wpilog"},
      {"download", "--host", "localhost", "--output-dir", dir.string(),
       "x.txt"},
      {"delete", "--host", "localhost", "--all"},
      {"delete", "--host", "localhost", "--yes"}};
  for (auto&& args : invalid) {
    CAPTURE(args);
    CHECK(Run(args) == 2);
    CHECK(!errors.empty());
  }
}

TEST_CASE_METHOD(CliTest, "CLI exports list CSV with quoting and types",
                 "[datalogtool]") {
  auto filename = WriteLog("types.wpilog", [](auto& log) {
    log.AppendFloat(log.Start("float", "float", "", 1), 1.5f, 1'000'000'000);
    log.AppendString(log.Start("a,\"b", "string", "", 1), "one,\"two\"\nthree",
                     2'000'000'000);
    log.AppendBoolean(log.Start("bool", "boolean", "", 1), true, 3'000'000'000);
    std::vector<std::string> strings{"a\"", "b"};
    log.AppendStringArray(log.Start("strings", "string[]", "", 1), strings,
                          4'000'000'000);
    log.AppendRaw(log.Start("raw", "raw", "", 1), {}, 5'000'000'000);
  });
  REQUIRE(Run({"export", "--output-dir", dir.string(), filename}) == 0);
  CHECK(output.empty());
  CHECK(Read("types.csv") ==
        "Timestamp,Name,Value\n1,\"float\",1.5\n2,\"a,\"\"b\",\"one,"
        "\"\"two\"\"\nthree\"\n3,\"bool\",true\n4,\"strings\",\"a\"\";b\"\n5,"
        "\"raw\",<invalid>\n");
}

TEST_CASE_METHOD(CliTest, "CLI table fuzziness uses shared bounded rows",
                 "[datalogtool]") {
  auto filename = WriteLog("table.wpilog", [](auto& log) {
    auto b = log.Start("B", "int64", "", 1);
    auto a = log.Start("A", "double", "", 1);
    log.AppendInteger(b, 20, 1'000'000'000);
    log.AppendDouble(a, 1.5, 1'000'500'000);
    log.AppendInteger(b, 30, 1'001'000'000);
    log.AppendDouble(a, 2.5, 1'001'500'000);
  });
  REQUIRE(Run({"export", "--style", "table", "--timestamp-fuzziness", "1", "-o",
               dir.string(), filename}) == 0);
  CHECK(Read("table.csv") == "Timestamp,\"A\",\"B\"\n1,1.5,30\n1.0015,2.5,\n");
}

TEST_CASE_METHOD(CliTest, "CLI selects entries by exact name and prefix",
                 "[datalogtool]") {
  auto filename = WriteLog("select.wpilog", [](auto& log) {
    log.AppendInteger(log.Start("/Drive/A", "int64", "units", 1), 1,
                      1'000'000'000);
    log.AppendInteger(log.Start("/Drive/B", "int64", "", 1), 2, 1'000'000'000);
    log.AppendInteger(log.Start("Other", "int64", "", 1), 3, 1'000'000'000);
    log.AppendInteger(log.Start("Excluded", "int64", "", 1), 4, 1'000'000'000);
  });
  REQUIRE(
      Run({"export", "--style", "table", "--entry-prefix", "/Drive/", "--entry",
           "Other", "--entry", "Excluded", "--exclude", "/Drive/B",
           "--exclude-prefix", "Exclude", "-o", dir.string(), filename}) == 0);
  CHECK(Read("select.csv") == "Timestamp,\"/Drive/A\",\"Other\"\n1,1,3\n");
  REQUIRE(Run({"entries", "--entry", "/Drive/A", filename}) == 0);
  CHECK(output == "File,Name,Type,Metadata\n\"" + filename +
                      "\",\"/Drive/A\",\"int64\",\"units\"\n");
  CHECK(errors.find("8 records, 4 entries") != std::string::npos);
}

TEST_CASE_METHOD(CliTest,
                 "CLI preserves conflicting definitions and decodes each file",
                 "[datalogtool]") {
  auto first = WriteLog("first.wpilog", [](auto& log) {
    auto entry = log.Start("value", "int64", "first", 1);
    log.AppendInteger(entry, 5, 1'000'000'000);
    log.Finish(entry, 2'000'000'000);
    entry = log.Start("value", "string", "second", 3'000'000'000);
    log.AppendString(entry, "new", 4'000'000'000);
  });
  auto second = WriteLog("second.wpilog", [](auto& log) {
    log.AppendDouble(log.Start("value", "double", "third", 1), 2.5,
                     1'000'000'000);
  });
  REQUIRE(Run({"export", "-o", dir.string(), first, second}) == 0);
  CHECK(Read("first.csv") ==
        "Timestamp,Name,Value\n1,\"value\",5\n4,\"value\",\"new\"\n");
  CHECK(Read("second.csv") == "Timestamp,Name,Value\n1,\"value\",2.5\n");
  REQUIRE(Run({"entries", first, second}) == 0);
  CHECK(output.find("\"int64\",\"first\"") != std::string::npos);
  CHECK(output.find("\"string\",\"second\"") != std::string::npos);
  CHECK(output.find("\"double\",\"third\"") != std::string::npos);
}

TEST_CASE_METHOD(CliTest, "CLI reports file failures and continues the batch",
                 "[datalogtool]") {
  auto filename = WriteLog("good.wpilog", [](auto& log) {
    log.AppendInteger(log.Start("value", "int64", "", 1), 7, 1'000'000'000);
  });
  auto bad = (dir / "bad.wpilog").string();
  {
    std::ofstream file{bad};
    file << "invalid";
  }
  auto missing = (dir / "missing.wpilog").string();
  REQUIRE(Run({"export", "-o", dir.string(), bad, missing, filename}) == 1);
  CHECK(errors.find("not a valid datalog file") != std::string::npos);
  CHECK(fs::exists(dir / "good.csv"));
  CHECK(!fs::exists(dir / "bad.csv"));
  CHECK(!fs::exists(dir / "missing.csv"));
  auto csv = Read("good.csv");
  CHECK(Run({"export", "-o", dir.string(), filename}) == 1);
  CHECK(Read("good.csv") == csv);
  CHECK(Run({"export", "-o", (dir / "missing").string(), filename}) == 1);
}

TEST_CASE_METHOD(CliTest,
                 "CLI never overwrites another input or colliding output",
                 "[datalogtool]") {
  auto filename = WriteLog("same.csv", [](auto& log) {
    log.AppendInteger(log.Start("value", "int64", "", 1), 9, 1'000'000'000);
  });
  auto original = Read("same.csv");
  CHECK(Run({"export", "-o", dir.string(), filename}) == 1);
  CHECK(Read("same.csv") == original);
  auto other = WriteLog("same.wpilog", [](auto&) {});
  CHECK(Run({"export", "-o", dir.string(), other}) == 1);
  CHECK(Read("same.csv") == original);
}
