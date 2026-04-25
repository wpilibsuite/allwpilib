// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/filterdesigner/model/Spec.hpp"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <ios>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "wpi/util/json.hpp"

namespace wpi::filterdesigner {

namespace {

constexpr std::string_view kKindStrings[] = {
    "lowpass", "highpass", "bandpass", "bandstop", "notch", "movingaverage"};
constexpr std::string_view kFamilyStrings[] = {"butter", "cheby1", "cheby2",
                                               "ellip"};

std::string KindToString(StageKind k) {
  return std::string{kKindStrings[static_cast<int>(k)]};
}

std::string FamilyToString(Family f) {
  return std::string{kFamilyStrings[static_cast<int>(f)]};
}

bool ParseKindString(std::string_view s, StageKind& out) {
  for (size_t i = 0; i < std::size(kKindStrings); ++i) {
    if (s == kKindStrings[i]) {
      out = static_cast<StageKind>(i);
      return true;
    }
  }
  return false;
}

bool ParseFamilyString(std::string_view s, Family& out) {
  for (size_t i = 0; i < std::size(kFamilyStrings); ++i) {
    if (s == kFamilyStrings[i]) {
      out = static_cast<Family>(i);
      return true;
    }
  }
  return false;
}

// Read result for the optional-field helpers. Distinguishing absent from
// present-but-wrong-type lets the caller error out for malformed values
// while still falling back to a struct default for absent ones.
enum class ReadStatus { Absent, Ok, WrongType };

ReadStatus ReadNumber(const wpi::util::json& obj, std::string_view key,
                      double& out) {
  const auto* v = obj.lookup(key);
  if (!v) {
    return ReadStatus::Absent;
  }
  if (!v->is_number()) {
    return ReadStatus::WrongType;
  }
  out = v->get_number();
  return ReadStatus::Ok;
}

// Read an integer field. JSON has no separate int type, so we accept any
// numeric value whose double representation is exactly an integer (e.g. 4.0
// is fine; 4.7 is rejected). Catches typos in hand-edited spec files instead
// of silently truncating.
ReadStatus ReadInt(const wpi::util::json& obj, std::string_view key, int& out) {
  double tmp;
  ReadStatus s = ReadNumber(obj, key, tmp);
  if (s != ReadStatus::Ok) {
    return s;
  }
  if (std::trunc(tmp) != tmp) {
    return ReadStatus::WrongType;
  }
  out = static_cast<int>(tmp);
  return ReadStatus::Ok;
}

ReadStatus ReadString(const wpi::util::json& obj, std::string_view key,
                      std::string& out) {
  const auto* v = obj.lookup(key);
  if (!v) {
    return ReadStatus::Absent;
  }
  if (!v->is_string()) {
    return ReadStatus::WrongType;
  }
  out = v->get_string();
  return ReadStatus::Ok;
}

}  // namespace

std::string SerializeSpec(double sampleRate, std::span<const Stage> stages) {
  wpi::util::json::array_t stageArr;
  stageArr.reserve(stages.size());
  for (const auto& s : stages) {
    wpi::util::json obj;
    obj.set_object();
    obj["kind"] = KindToString(s.kind);
    if (KindUsesFamily(s.kind)) {
      obj["family"] = FamilyToString(s.family);
    }
    // Emit only the fields the kind actually uses — keeps the JSON compact
    // and self-documenting (a "lowpass" entry has no "q", a "notch" has no
    // "order", etc.).
    switch (s.kind) {
      case StageKind::LowPass:
      case StageKind::HighPass:
        obj["order"] = s.order;
        obj["f1"] = s.f1;
        break;
      case StageKind::BandPass:
      case StageKind::BandStop:
        obj["order"] = s.order;
        obj["f1"] = s.f1;
        obj["f2"] = s.f2;
        break;
      case StageKind::Notch:
        obj["f1"] = s.f1;
        obj["q"] = s.q;
        break;
      case StageKind::MovingAverage:
        obj["taps"] = s.taps;
        break;
    }
    if (KindUsesFamily(s.kind)) {
      const bool needsRipple =
          s.family == Family::Chebyshev1 || s.family == Family::Elliptic;
      const bool needsAtten =
          s.family == Family::Chebyshev2 || s.family == Family::Elliptic;
      if (needsRipple) {
        obj["passbandRippleDb"] = s.passbandRippleDb;
      }
      if (needsAtten) {
        obj["stopbandAttenDb"] = s.stopbandAttenDb;
      }
    }
    stageArr.push_back(std::move(obj));
  }

  wpi::util::json root;
  root.set_object();
  root["version"] = kSpecVersion;
  root["sampleRate"] = sampleRate;
  root["stages"] = wpi::util::json{std::move(stageArr)};
  return root.to_string_pretty();
}

ParseResult ParseSpec(std::string_view json) {
  ParseResult result;
  auto parsed = wpi::util::json::parse(json);
  if (!parsed) {
    result.error = std::string{"Malformed JSON: "} + parsed.error();
    return result;
  }
  const wpi::util::json& root = *parsed;
  if (!root.is_object()) {
    result.error = "Spec root must be a JSON object";
    return result;
  }

  // Schema versioning: missing version is treated as v1 (the only version
  // shipping today). Newer major versions are rejected so a future loader
  // can extend the schema without older builds silently dropping data.
  int version = kSpecVersion;
  ReadStatus versionStatus = ReadInt(root, "version", version);
  if (versionStatus == ReadStatus::WrongType) {
    result.error = "Top-level \"version\" must be an integer";
    return result;
  }
  if (version > kSpecVersion) {
    result.error = "Unsupported spec version " + std::to_string(version) +
                   " (this build understands up to " +
                   std::to_string(kSpecVersion) + ")";
    return result;
  }

  LoadedSpec loaded;
  ReadStatus sampleRateStatus =
      ReadNumber(root, "sampleRate", loaded.sampleRate);
  if (sampleRateStatus != ReadStatus::Ok) {
    result.error = "Missing or non-numeric \"sampleRate\"";
    return result;
  }

  const auto* stagesNode = root.lookup("stages");
  if (!stagesNode || !stagesNode->is_array()) {
    result.error = "Missing or non-array \"stages\"";
    return result;
  }
  const auto& stagesArr = stagesNode->get_array();

  for (size_t i = 0; i < stagesArr.size(); ++i) {
    const auto& entry = stagesArr[i];
    if (!entry.is_object()) {
      result.error = "Stage " + std::to_string(i) + " is not an object";
      return result;
    }

    Stage stage;  // Defaults from the struct definition.
    std::string kindStr;
    ReadStatus kindStatus = ReadString(entry, "kind", kindStr);
    if (kindStatus != ReadStatus::Ok) {
      result.error = "Stage " + std::to_string(i) + " missing \"kind\"";
      return result;
    }
    if (!ParseKindString(kindStr, stage.kind)) {
      result.error =
          "Stage " + std::to_string(i) + " has unknown kind: " + kindStr;
      return result;
    }

    if (KindUsesFamily(stage.kind)) {
      std::string familyStr;
      ReadStatus familyStatus = ReadString(entry, "family", familyStr);
      if (familyStatus != ReadStatus::Ok) {
        result.error =
            "Stage " + std::to_string(i) + " missing \"family\" for " + kindStr;
        return result;
      }
      if (!ParseFamilyString(familyStr, stage.family)) {
        result.error =
            "Stage " + std::to_string(i) + " has unknown family: " + familyStr;
        return result;
      }
    }

    // Per-field reads: absent is OK (struct default kicks in), but a present
    // value of the wrong type is an error rather than silently ignored.
    auto readOptional = [&](ReadStatus s, const char* field) -> bool {
      if (s == ReadStatus::WrongType) {
        result.error = "Stage " + std::to_string(i) + " field \"" + field +
                       "\" has wrong type";
        return false;
      }
      return true;
    };
    if (!readOptional(ReadInt(entry, "order", stage.order), "order") ||
        !readOptional(ReadInt(entry, "taps", stage.taps), "taps") ||
        !readOptional(ReadNumber(entry, "f1", stage.f1), "f1") ||
        !readOptional(ReadNumber(entry, "f2", stage.f2), "f2") ||
        !readOptional(ReadNumber(entry, "q", stage.q), "q") ||
        !readOptional(
            ReadNumber(entry, "passbandRippleDb", stage.passbandRippleDb),
            "passbandRippleDb") ||
        !readOptional(
            ReadNumber(entry, "stopbandAttenDb", stage.stopbandAttenDb),
            "stopbandAttenDb")) {
      return result;
    }

    loaded.stages.push_back(std::move(stage));
  }

  result.spec = std::move(loaded);
  return result;
}

std::string SaveSpecToFile(const std::string& path, double sampleRate,
                           std::span<const Stage> stages) {
  std::string text = SerializeSpec(sampleRate, stages);
  std::ofstream out(path, std::ios::binary | std::ios::trunc);
  if (!out) {
    return "Could not open for writing: " + path;
  }
  out << text;
  if (!out) {
    return "Write failed: " + path;
  }
  return {};
}

ParseResult LoadSpecFromFile(const std::string& path) {
  ParseResult result;
  std::ifstream in(path, std::ios::binary);
  if (!in) {
    result.error = "Could not open: " + path;
    return result;
  }
  std::ostringstream ss;
  ss << in.rdbuf();
  return ParseSpec(ss.str());
}

}  // namespace wpi::filterdesigner
