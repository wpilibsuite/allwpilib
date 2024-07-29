//===- VersionTuple.h - Version Number Handling -----------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Defines the llvm::VersionTuple class, which represents a version in
/// the form major[.minor[.subminor]].
///
//===----------------------------------------------------------------------===//
#ifndef WPIUTIL_WPI_VERSIONTUPLE_H
#define WPIUTIL_WPI_VERSIONTUPLE_H

#include "wpi/DenseMapInfo.h"
#include "wpi/Hashing.h"
#include <optional>
#include <string>
#include <tuple>

namespace wpi {
template <typename HasherT, wpi::endianness Endianness> class HashBuilder;
class raw_ostream;

/// Represents a version number in the form major[.minor[.subminor[.build]]].
class VersionTuple {
  unsigned Major : 32;

  unsigned Minor : 31;
  unsigned HasMinor : 1;

  unsigned Subminor : 31;
  unsigned HasSubminor : 1;

  unsigned Build : 31;
  unsigned HasBuild : 1;

public:
  constexpr VersionTuple()
      : Major(0), Minor(0), HasMinor(false), Subminor(0), HasSubminor(false),
        Build(0), HasBuild(false) {}

  explicit constexpr VersionTuple(unsigned Major)
      : Major(Major), Minor(0), HasMinor(false), Subminor(0),
        HasSubminor(false), Build(0), HasBuild(false) {}

  explicit constexpr VersionTuple(unsigned Major, unsigned Minor)
      : Major(Major), Minor(Minor), HasMinor(true), Subminor(0),
        HasSubminor(false), Build(0), HasBuild(false) {}

  explicit constexpr VersionTuple(unsigned Major, unsigned Minor,
                                  unsigned Subminor)
      : Major(Major), Minor(Minor), HasMinor(true), Subminor(Subminor),
        HasSubminor(true), Build(0), HasBuild(false) {}

  explicit constexpr VersionTuple(unsigned Major, unsigned Minor,
                                  unsigned Subminor, unsigned Build)
      : Major(Major), Minor(Minor), HasMinor(true), Subminor(Subminor),
        HasSubminor(true), Build(Build), HasBuild(true) {}

  /// Determine whether this version information is empty
  /// (e.g., all version components are zero).
  bool empty() const {
    return Major == 0 && Minor == 0 && Subminor == 0 && Build == 0;
  }

  /// Retrieve the major version number.
  unsigned getMajor() const { return Major; }

  /// Retrieve the minor version number, if provided.
  std::optional<unsigned> getMinor() const {
    if (!HasMinor)
      return std::nullopt;
    return Minor;
  }

  /// Retrieve the subminor version number, if provided.
  std::optional<unsigned> getSubminor() const {
    if (!HasSubminor)
      return std::nullopt;
    return Subminor;
  }

  /// Retrieve the build version number, if provided.
  std::optional<unsigned> getBuild() const {
    if (!HasBuild)
      return std::nullopt;
    return Build;
  }

  /// Return a version tuple that contains only the first 3 version components.
  VersionTuple withoutBuild() const {
    if (HasBuild)
      return VersionTuple(Major, Minor, Subminor);
    return *this;
  }

  /// Return a version tuple that contains a different major version but
  /// everything else is the same.
  VersionTuple withMajorReplaced(unsigned NewMajor) const {
    return VersionTuple(NewMajor, Minor, Subminor, Build);
  }

  /// Return a version tuple that contains only components that are non-zero.
  VersionTuple normalize() const {
    VersionTuple Result = *this;
    if (Result.Build == 0) {
      Result.HasBuild = false;
      if (Result.Subminor == 0) {
        Result.HasSubminor = false;
        if (Result.Minor == 0)
          Result.HasMinor = false;
      }
    }
    return Result;
  }

  /// Determine if two version numbers are equivalent. If not
  /// provided, minor and subminor version numbers are considered to be zero.
  friend bool operator==(const VersionTuple &X, const VersionTuple &Y) {
    return X.Major == Y.Major && X.Minor == Y.Minor &&
           X.Subminor == Y.Subminor && X.Build == Y.Build;
  }

  /// Determine if two version numbers are not equivalent.
  ///
  /// If not provided, minor and subminor version numbers are considered to be
  /// zero.
  friend bool operator!=(const VersionTuple &X, const VersionTuple &Y) {
    return !(X == Y);
  }

  /// Determine whether one version number precedes another.
  ///
  /// If not provided, minor and subminor version numbers are considered to be
  /// zero.
  friend bool operator<(const VersionTuple &X, const VersionTuple &Y) {
    return std::tie(X.Major, X.Minor, X.Subminor, X.Build) <
           std::tie(Y.Major, Y.Minor, Y.Subminor, Y.Build);
  }

  /// Determine whether one version number follows another.
  ///
  /// If not provided, minor and subminor version numbers are considered to be
  /// zero.
  friend bool operator>(const VersionTuple &X, const VersionTuple &Y) {
    return Y < X;
  }

  /// Determine whether one version number precedes or is
  /// equivalent to another.
  ///
  /// If not provided, minor and subminor version numbers are considered to be
  /// zero.
  friend bool operator<=(const VersionTuple &X, const VersionTuple &Y) {
    return !(Y < X);
  }

  /// Determine whether one version number follows or is
  /// equivalent to another.
  ///
  /// If not provided, minor and subminor version numbers are considered to be
  /// zero.
  friend bool operator>=(const VersionTuple &X, const VersionTuple &Y) {
    return !(X < Y);
  }
};

} // end namespace wpi
#endif // WPIUTIL_WPI_VERSIONTUPLE_H
