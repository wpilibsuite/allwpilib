// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "Types_internal.h"

std::string_view nt::TypeToString(NT_Type type) {
  switch (type) {
    case NT_BOOLEAN:
      return "boolean";
    case NT_DOUBLE:
      return "double";
    case NT_STRING:
      return "string";
    case NT_BOOLEAN_ARRAY:
      return "boolean[]";
    case NT_DOUBLE_ARRAY:
      return "double[]";
    case NT_STRING_ARRAY:
      return "string[]";
    case NT_RPC:
      return "rpc";
    case NT_INTEGER:
      return "int";
    case NT_FLOAT:
      return "float";
    case NT_INTEGER_ARRAY:
      return "int[]";
    case NT_FLOAT_ARRAY:
      return "float";
    default:
      return "raw";
  }
}

NT_Type nt::StringToType(std::string_view typeStr) {
  if (typeStr == "boolean") {
    return NT_BOOLEAN;
  } else if (typeStr == "double") {
    return NT_DOUBLE;
  } else if (typeStr == "string" || typeStr == "json") {
    return NT_STRING;
  } else if (typeStr == "boolean[]") {
    return NT_BOOLEAN_ARRAY;
  } else if (typeStr == "double[]") {
    return NT_DOUBLE_ARRAY;
  } else if (typeStr == "string[]") {
    return NT_STRING_ARRAY;
  } else if (typeStr == "rpc") {
    return NT_RPC;
  } else if (typeStr == "int") {
    return NT_INTEGER;
  } else if (typeStr == "float") {
    return NT_FLOAT;
  } else if (typeStr == "int[]") {
    return NT_INTEGER_ARRAY;
  } else if (typeStr == "float[]") {
    return NT_FLOAT_ARRAY;
  } else {
    return NT_RAW;
  }
}

NT_Type nt::StringToType3(std::string_view typeStr) {
  if (typeStr == "boolean") {
    return NT_BOOLEAN;
  } else if (typeStr == "double" || typeStr == "int" || typeStr == "float") {
    return NT_DOUBLE;
  } else if (typeStr == "string" || typeStr == "json") {
    return NT_STRING;
  } else if (typeStr == "boolean[]") {
    return NT_BOOLEAN_ARRAY;
  } else if (typeStr == "double[]" || typeStr == "int[]" ||
             typeStr == "float[]") {
    return NT_DOUBLE_ARRAY;
  } else if (typeStr == "string[]") {
    return NT_STRING_ARRAY;
  } else if (typeStr == "rpc") {
    return NT_RPC;
  } else {
    return NT_RAW;
  }
}
