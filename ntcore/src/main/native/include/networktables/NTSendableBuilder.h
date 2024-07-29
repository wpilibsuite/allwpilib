// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>
#include <string_view>

#include <wpi/FunctionExtras.h>
#include <wpi/sendable/SendableBuilder.h>

#include "networktables/NetworkTable.h"
#include "networktables/Topic.h"

namespace nt {

/**
 * Helper class for building Sendable dashboard representations for
 * NetworkTables.
 */
class NTSendableBuilder : public wpi::SendableBuilder {
 public:
  /**
   * Set the function that should be called to update the network table
   * for things other than properties.  Note this function is not passed
   * the network table object; instead it should use the entry handles
   * returned by GetEntry().
   *
   * @param func    function
   */
  virtual void SetUpdateTable(wpi::unique_function<void()> func) = 0;

  /**
   * Add a property without getters or setters.  This can be used to get
   * entry handles for the function called by SetUpdateTable().
   *
   * @param key   property name
   * @return Network table topic
   */
  virtual Topic GetTopic(std::string_view key) = 0;

  /**
   * Get the network table.
   * @return The network table
   */
  virtual std::shared_ptr<NetworkTable> GetTable() = 0;

  /**
   * Gets the kind of backend being used.
   *
   * @return Backend kind
   */
  BackendKind GetBackendKind() const override;
};

}  // namespace nt
