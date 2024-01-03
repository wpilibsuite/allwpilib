// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "sysid/analysis/JSONConverter.h"

#include <stdexcept>
#include <string>

#include <fmt/core.h>
#include <fmt/format.h>
#include <wpi/Logger.h>
#include <wpi/MemoryBuffer.h>
#include <wpi/fmt/raw_ostream.h>
#include <wpi/json.h>
#include <wpi/raw_ostream.h>

#include "sysid/Util.h"
#include "sysid/analysis/AnalysisManager.h"
#include "sysid/analysis/AnalysisType.h"

// Sizes of the arrays for new sysid data.
inline constexpr size_t kDrivetrainSize = 9;
inline constexpr size_t kGeneralSize = 4;

// Indices for the old data.
inline constexpr size_t kTimestampCol = 0;
inline constexpr size_t kLVoltsCol = 3;
inline constexpr size_t kRVoltsCol = 4;
inline constexpr size_t kLPosCol = 5;
inline constexpr size_t kRPosCol = 6;
inline constexpr size_t kLVelCol = 7;
inline constexpr size_t kRVelCol = 8;

static wpi::json GetJSON(std::string_view path, wpi::Logger& logger) {
  std::error_code ec;
  std::unique_ptr<wpi::MemoryBuffer> fileBuffer =
      wpi::MemoryBuffer::GetFile(path, ec);
  if (fileBuffer == nullptr || ec) {
    throw std::runtime_error(fmt::format("Unable to read: {}", path));
  }

  wpi::json json = wpi::json::parse(fileBuffer->GetCharBuffer());
  WPI_INFO(logger, "Read frc-characterization JSON from {}", path);
  return json;
}

std::string sysid::ConvertJSON(std::string_view path, wpi::Logger& logger) {
  wpi::json ojson = GetJSON(path, logger);

  auto type = sysid::analysis::FromName(ojson.at("test").get<std::string>());
  auto factor = ojson.at("unitsPerRotation").get<double>();
  auto unit = ojson.at("units").get<std::string>();

  wpi::json json;
  for (auto&& key : AnalysisManager::kJsonDataKeys) {
    if (type == analysis::kDrivetrain) {
      // Get the old data; create a vector for the new data; reserve the
      // appropriate size for the new data.
      auto odata = ojson.at(key).get<std::vector<std::array<double, 10>>>();
      std::vector<std::array<double, kDrivetrainSize>> data;
      data.reserve(odata.size());

      // Transfer the data.
      for (auto&& pt : odata) {
        data.push_back(std::array<double, kDrivetrainSize>{
            pt[kTimestampCol], pt[kLVoltsCol], pt[kRVoltsCol], pt[kLPosCol],
            pt[kRPosCol], pt[kLVelCol], pt[kRVelCol], 0.0, 0.0});
      }
      json[key] = data;
    } else {
      // Get the old data; create a vector for the new data; reserve the
      // appropriate size for the new data.
      auto odata = ojson.at(key).get<std::vector<std::array<double, 10>>>();
      std::vector<std::array<double, kGeneralSize>> data;
      data.reserve(odata.size());

      // Transfer the data.
      for (auto&& pt : odata) {
        data.push_back(std::array<double, kGeneralSize>{
            pt[kTimestampCol], pt[kLVoltsCol], pt[kLPosCol], pt[kLVelCol]});
      }
      json[key] = data;
    }
  }
  json["units"] = unit;
  json["unitsPerRotation"] = factor;
  json["test"] = type.name;
  json["sysid"] = true;

  // Write the new file with "_new" appended to it.
  path.remove_suffix(std::string_view{".json"}.size());
  std::string loc = fmt::format("{}_new.json", path);

  sysid::SaveFile(json.dump(2), std::filesystem::path{loc});

  WPI_INFO(logger, "Wrote new JSON to: {}", loc);
  return loc;
}

std::string sysid::ToCSV(std::string_view path, wpi::Logger& logger) {
  wpi::json json = GetJSON(path, logger);

  auto type = sysid::analysis::FromName(json.at("test").get<std::string>());
  auto factor = json.at("unitsPerRotation").get<double>();
  auto unit = json.at("units").get<std::string>();
  std::string_view abbreviation = GetAbbreviation(unit);

  std::error_code ec;
  // Naming: {sysid-json-name}(Test, Units).csv
  path.remove_suffix(std::string_view{".json"}.size());
  std::string loc = fmt::format("{} ({}, {}).csv", path, type.name, unit);
  wpi::raw_fd_ostream outputFile{loc, ec};

  if (ec) {
    throw std::runtime_error("Unable to write to: " + loc);
  }

  fmt::print(outputFile, "Timestamp (s),Test,");
  if (type == analysis::kDrivetrain || type == analysis::kDrivetrainAngular) {
    fmt::print(
        outputFile,
        "Left Volts (V),Right Volts (V),Left Position ({0}),Right "
        "Position ({0}),Left Velocity ({0}/s),Right Velocity ({0}/s),Gyro "
        "Position (deg),Gyro Rate (deg/s)\n",
        abbreviation);
  } else {
    fmt::print(outputFile, "Volts (V),Position({0}),Velocity ({0}/s)\n",
               abbreviation);
  }
  outputFile << "\n";

  for (auto&& key : AnalysisManager::kJsonDataKeys) {
    if (type == analysis::kDrivetrain || type == analysis::kDrivetrainAngular) {
      auto tempData =
          json.at(key).get<std::vector<std::array<double, kDrivetrainSize>>>();
      for (auto&& pt : tempData) {
        fmt::print(outputFile, "{},{},{},{},{},{},{},{},{},{}\n",
                   pt[0],                           // Timestamp
                   key,                             // Test
                   pt[1], pt[2],                    // Left and Right Voltages
                   pt[3] * factor, pt[4] * factor,  // Left and Right Positions
                   pt[5] * factor, pt[6] * factor,  // Left and Right Velocity
                   pt[7], pt[8]  // Gyro Position and Velocity
        );
      }
    } else {
      auto tempData =
          json.at(key).get<std::vector<std::array<double, kGeneralSize>>>();
      for (auto&& pt : tempData) {
        fmt::print(outputFile, "{},{},{},{},{}\n",
                   pt[0],           // Timestamp,
                   key,             // Test
                   pt[1],           // Voltage
                   pt[2] * factor,  // Position
                   pt[3] * factor   // Velocity
        );
      }
    }
  }
  outputFile.flush();
  WPI_INFO(logger, "Wrote CSV to: {}", loc);
  return loc;
}
