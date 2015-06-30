/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/
#pragma once

// A macro for making a class move-only
#define DISALLOW_COPY_AND_ASSIGN(TypeName)       \
  TypeName(const TypeName&) = delete;            \
  TypeName& operator=(const TypeName&) = delete; \
  TypeName(TypeName&&) = default;                \
  TypeName& operator=(TypeName&&) = default

// A struct to use as a deleter when a std::shared_ptr must wrap a raw pointer
// that is being deleted by someone else.
// This should only be called in deprecated functions; using it anywhere else
// will throw warnings.
template<class T>
struct [[deprecated]] NullDeleter {
  void operator()(T *) const noexcept {};
};
