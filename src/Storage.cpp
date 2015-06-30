/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Storage.h"

#include <string>
#include <tuple>

#include "llvm/StringExtras.h"
#include "Base64.h"

using namespace ntimpl;

Storage* Storage::m_instance = nullptr;

Storage::Storage() {}

Storage::~Storage() {}

/* Escapes and writes a string, including start and end double quotes */
static void WriteString(std::ostream& os, llvm::StringRef str) {
  os << '"';
  for (auto c : str) {
    switch (c) {
      case '\\':
        os << "\\\\";
        break;
      case '\t':
        os << "\\t";
        break;
      case '\n':
        os << "\\n";
        break;
      case '"':
        os << "\\\"";
        break;
      default:
        if (std::isprint(c)) {
          os << c;
          break;
        }

        // Write out the escaped representation.
        os << "\\x";
        os << llvm::hexdigit((c >> 4) & 0xF);
        os << llvm::hexdigit((c >> 0) & 0xF);
    }
  }
  os << '"';
}

void Storage::SavePersistent(std::ostream& os) const {
  std::string base64_encoded;

  // header
  os << "[NetworkTables Storage 3.0]\n";

  for (auto& i : m_entries) {
    const StorageEntry& entry = i.getValue();
    // only write persistent-flagged values
    if (!entry.IsPersistent()) continue;

    // type
    const Value& v = entry.value();
    switch (v.type()) {
      case NT_BOOLEAN:
        os << "boolean ";
        break;
      case NT_DOUBLE:
        os << "double ";
        break;
      case NT_STRING:
        os << "string ";
        break;
      case NT_RAW:
        os << "raw ";
        break;
      case NT_BOOLEAN_ARRAY:
        os << "array boolean ";
        break;
      case NT_DOUBLE_ARRAY:
        os << "array double ";
        break;
      case NT_STRING_ARRAY:
        os << "array string ";
        break;
      default:
        continue;
    }

    // name
    WriteString(os, i.getKey());

    // =
    os << '=';

    // value
    switch (v.type()) {
      case NT_BOOLEAN:
        os << (v.GetBoolean() ? "true" : "false");
        break;
      case NT_DOUBLE:
        os << v.GetDouble();
        break;
      case NT_STRING:
        WriteString(os, v.GetString());
        break;
      case NT_RAW:
        Base64Encode(v.GetRaw(), &base64_encoded);
        os << base64_encoded;
        break;
      case NT_BOOLEAN_ARRAY: {
        bool first = true;
        for (auto elem : v.GetBooleanArray()) {
          if (!first) {
            os << ',';
            first = false;
          }
          os << (elem ? "true" : "false");
        }
        break;
      }
      case NT_DOUBLE_ARRAY: {
        bool first = true;
        for (auto elem : v.GetDoubleArray()) {
          if (!first) {
            os << ',';
            first = false;
          }
          os << elem;
        }
        break;
      }
      case NT_STRING_ARRAY: {
        bool first = true;
        for (auto& elem : v.GetStringArray()) {
          if (!first) {
            os << ',';
            first = false;
          }
          WriteString(os, elem);
        }
        break;
      }
      default:
        break;
    }

    // eol
    os << '\n';
  }
}

/* Extracts an escaped string token.  Does not unescape the string.
 * If a string cannot be matched, an empty string is returned.
 * If the string is unterminated, an empty tail string is returned.
 * The returned token includes the starting and trailing quotes (unless the
 * string is unterminated).
 * Returns a pair containing the extracted token (if any) and the remaining
 * tail string.
 */
static std::pair<llvm::StringRef, llvm::StringRef> ReadStringToken(
    llvm::StringRef source) {
  // Match opening quote
  if (source.empty() || source.front() != '"')
    return std::make_pair(llvm::StringRef(), source);

  // Scan for ending double quote, checking for escaped as we go.
  std::size_t size = source.size();
  std::size_t pos;
  for (pos = 1; pos < size; ++pos) {
    if (source[pos] == '"' && source[pos - 1] != '\\') {
      ++pos;  // we want to include the trailing quote in the result
      break;
    }
  }
  return std::make_pair(source.slice(0, pos), source.substr(pos));
}

static int fromxdigit(char ch) {
  if (ch >= 'a' && ch <= 'f')
    return (ch - 'a' + 10);
  else if (ch >= 'A' && ch <= 'F')
    return (ch - 'A' + 10);
  else
    return ch - '0';
}

static void UnescapeString(llvm::StringRef source, std::string* dest) {
  assert(source.size() > 2 && source.front() == '"' && source.back() == '"');
  dest->clear();
  dest->reserve(source.size() - 2);
  for (auto s = source.begin() + 1, end = source.end() - 1; s != end; ++s) {
    if (*s != '\\') {
      dest->push_back(*s);
      continue;
    }
    switch (*s++) {
      case '\\':
      case '"':
        dest->push_back(s[-1]);
        break;
      case 't':
        dest->push_back('\t');
        break;
      case 'n':
        dest->push_back('\n');
        break;
      case 'x': {
        if (!isxdigit(*s)) {
          dest->push_back('x');  // treat it like a unknown escape
          break;
        }
        int ch = fromxdigit(*s);
        ++s;
        if (isxdigit(*s)) {
          ch <<= 4;
          ch |= fromxdigit(*s);
          ++s;
        }
        dest->push_back(static_cast<char>(ch));
        break;
      }
      default:
        dest->push_back(s[-1]);
        break;
    }
  }
}

bool Storage::LoadPersistent(std::istream& is,
                             void (*warn)(std::size_t line, const char* msg)) {
  std::string line_str;
  std::size_t line_num = 1;

  // declare these outside the loop to reduce reallocs
  std::string name, str;
  std::vector<int> boolean_array;
  std::vector<double> double_array;
  std::vector<StringValue> string_array;

  // ignore blank lines and lines that start with ; or # (comments)
  while (std::getline(is, line_str)) {
    llvm::StringRef line = llvm::StringRef(line_str).trim();
    if (!line.empty() && line.front() != ';' && line.front() != '#')
      break;
  }

  // header
  if (line_str != "[NetworkTables Storage 3.0]") {
    if (warn) warn(line_num, "header line mismatch, ignoring rest of file");
    return false;
  }

  while (std::getline(is, line_str)) {
    llvm::StringRef line = llvm::StringRef(line_str).trim();
    ++line_num;

    // ignore blank lines and lines that start with ; or # (comments)
    if (line.empty() || line.front() == ';' || line.front() == '#')
      continue;

    // type
    llvm::StringRef type_tok;
    std::tie(type_tok, line) = line.split(' ');
    NT_Type type = NT_UNASSIGNED;
    if (type_tok == "boolean") type = NT_BOOLEAN;
    else if (type_tok == "double") type = NT_DOUBLE;
    else if (type_tok == "string") type = NT_STRING;
    else if (type_tok == "raw") type = NT_RAW;
    else if (type_tok == "array") {
      llvm::StringRef array_tok;
      std::tie(array_tok, line) = line.split(' ');
      if (array_tok == "boolean") type = NT_BOOLEAN_ARRAY;
      else if (array_tok == "double") type = NT_DOUBLE_ARRAY;
      else if (array_tok == "string") type = NT_STRING_ARRAY;
    }
    if (type == NT_UNASSIGNED) {
      if (warn) warn(line_num, "unrecognized type");
      continue;
    }

    // name
    llvm::StringRef name_tok;
    std::tie(name_tok, line) = ReadStringToken(line);
    if (name_tok.empty()) {
      if (warn) warn(line_num, "missing name");
      continue;
    }
    if (name_tok.back() != '"') {
      if (warn) warn(line_num, "unterminated name string");
      continue;
    }
    UnescapeString(name_tok, &name);

    // =
    line = line.ltrim(" \t");
    if (line.empty() || line.front() != '=') {
      if (warn) warn(line_num, "expected = after name");
      continue;
    }
    line = line.drop_front().ltrim(" \t");

    // value
    StorageEntry entry;
    switch (type) {
      case NT_BOOLEAN:
        // only true or false is accepted
        if (line == "true")
          entry.value().SetBoolean(true);
        else if (line == "false")
          entry.value().SetBoolean(false);
        else {
          if (warn)
            warn(line_num, "unrecognized boolean value, not 'true' or 'false'");
          goto next_line;
        }
        break;
      case NT_DOUBLE: {
        // need to convert to null-terminated string for strtod()
        str.clear();
        str += line;
        char* end;
        double v = std::strtod(str.c_str(), &end);
        if (*end != '\0') {
          if (warn) warn(line_num, "invalid double value");
          goto next_line;
        }
        entry.value().SetDouble(v);
        break;
      }
      case NT_STRING: {
        llvm::StringRef str_tok;
        std::tie(str_tok, line) = ReadStringToken(line);
        if (str_tok.empty()) {
          if (warn) warn(line_num, "missing string value");
          goto next_line;
        }
        if (str_tok.back() != '"') {
          if (warn) warn(line_num, "unterminated string value");
          goto next_line;
        }
        UnescapeString(str_tok, &str);
        entry.value().SetString(str);
        break;
      }
      case NT_RAW:
        Base64Decode(line, &str);
        entry.value().SetRaw(str);
        break;
      case NT_BOOLEAN_ARRAY: {
        llvm::StringRef elem_tok;
        boolean_array.clear();
        while (!line.empty()) {
          std::tie(elem_tok, line) = line.split(',');
          elem_tok = elem_tok.trim(" \t");
          if (elem_tok == "true")
            boolean_array.push_back(1);
          else if (elem_tok == "false")
            boolean_array.push_back(0);
          else {
            if (warn)
              warn(line_num,
                   "unrecognized boolean value, not 'true' or 'false'");
            goto next_line;
          }
        }

        entry.value().SetBooleanArray(boolean_array);
        break;
      }
      case NT_DOUBLE_ARRAY: {
        llvm::StringRef elem_tok;
        double_array.clear();
        while (!line.empty()) {
          std::tie(elem_tok, line) = line.split(',');
          elem_tok = elem_tok.trim(" \t");
          // need to convert to null-terminated string for strtod()
          str.clear();
          str += elem_tok;
          char* end;
          double v = std::strtod(str.c_str(), &end);
          if (*end != '\0') {
            if (warn) warn(line_num, "invalid double value");
            goto next_line;
          }
          double_array.push_back(v);
        }

        entry.value().SetDoubleArray(double_array);
        break;
      }
      case NT_STRING_ARRAY: {
        llvm::StringRef elem_tok;
        double_array.clear();
        while (!line.empty()) {
          std::tie(elem_tok, line) = ReadStringToken(line);
          if (elem_tok.empty()) {
            if (warn) warn(line_num, "missing string value");
            goto next_line;
          }
          if (elem_tok.back() != '"') {
            if (warn) warn(line_num, "unterminated string value");
            goto next_line;
          }
          line = line.ltrim(" \t");
          if (line.empty()) break;
          if (line.front() != ',') {
            if (warn) warn(line_num, "expected comma between strings");
            goto next_line;
          }
          line = line.drop_front().ltrim(" \t");

          UnescapeString(elem_tok, &str);
          string_array.push_back(StringValue(str));
        }

        entry.value().SetStringArray(string_array);
        break;
      }
      default:
        break;
    }
next_line:
    ;
  }
  return true;
}
