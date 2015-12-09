/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "tables/TableKeyNotDefinedException.h"

TableKeyNotDefinedException::TableKeyNotDefinedException(llvm::StringRef key)
    : msg("Unknown Table Key: ") {
  msg += key;
}

const char* TableKeyNotDefinedException::what() const NT_NOEXCEPT {
  return msg.c_str();
}

TableKeyNotDefinedException::~TableKeyNotDefinedException() NT_NOEXCEPT {}
