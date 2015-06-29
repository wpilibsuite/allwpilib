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
  std::string name;

  // header
  std::getline(is, line_str);
  if (line_str != "[NetworkTables Storage 3.0]") {
    if (warn) warn(line_num, "header line mismatch, ignoring rest of file");
    return false;
  }

  while (std::getline(is, line_str)) {
    llvm::StringRef line = llvm::StringRef(line_str).trim();
    ++line_num;

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
        //os << (v.data.v_boolean ? "true" : "false");
        //if (line == "true")
          //entry.value.
        break;
      case NT_DOUBLE:
        //os << v.data.v_double;
        break;
      case NT_STRING:
        //WriteString(os, MakeStringRef(v.data.v_string));
        break;
      case NT_RAW: {
        //char* buf = new char[Base64EncodeLen(v.data.v_raw.len)];
        //Base64Encode(buf,
                     //reinterpret_cast<const unsigned char*>(v.data.v_raw.str),
                     //v.data.v_raw.len);
        //os << buf;
        //delete[] buf;
        break;
      }
      case NT_BOOLEAN_ARRAY:
        //for (size_t i = 0; i < v.data.arr_boolean.size; ++i) {
          //os << (v.data.arr_boolean.arr[i] ? "true" : "false");
          //if (i != (v.data.arr_boolean.size - 1))
            //os << ',';
        //}
        break;
      case NT_DOUBLE_ARRAY:
        //for (size_t i = 0; i < v.data.arr_double.size; ++i) {
          //os << v.data.arr_double.arr[i];
          //if (i != (v.data.arr_double.size - 1))
            //os << ',';
        //}
        break;
      case NT_STRING_ARRAY:
        //for (size_t i = 0; i < v.data.arr_double.size; ++i) {
          //WriteString(os, MakeStringRef(v.data.arr_string.arr[i]));
          //if (i != (v.data.arr_double.size - 1))
            //os << ',';
        //}
        break;
      default:
        break;
    }

  }
  return true;
}
