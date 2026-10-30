// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string_view>

#include "wpi/nt/NetworkTableInstance.hpp"

namespace wpi::nt {

/**
 * Installs native publication of wpiutil alerts on an explicit NT instance.
 *
 * Call once at process startup, before creating any alerts or alert readers.
 * Reinstallation and changing backends while the alert system is in use are
 * unsupported, including after data reset or instance shutdown. These usage
 * restrictions are not checked at runtime. Existing alerts and handles are not
 * migrated to the new backend.
 *
 * Topics are `<root>/<group>/<level>/<id>/text` (string) and `/active`
 * (integer). Active is zero when inactive, otherwise the activation time in
 * nanoseconds from the local WPI monotonic clock. The text topic's
 * mrcAlertIdentity property contains group, uniqueId, and level. Group and ID
 * are preserved verbatim, including empty strings and slashes. Distinct
 * identities mapping to the same topic path cannot coexist; creation returns
 * WPI_ALERT_ALREADY_ALLOCATED. The caller must reserve the publication subtree
 * for this backend.
 *
 * Root normalization adds a leading slash, collapses repeated slashes, and
 * removes trailing slashes. The root must contain a non-slash character and no
 * embedded NULs. The instance must be valid and must not be concurrently reset
 * or destroyed during installation. Levels 0 through 255 are supported.
 *
 * The instance should outlive all alerts. Its destruction or reset unpublishes
 * and invalidates owned alerts, and rejects future creation. Existing readers
 * remain readable, observe removals subject to bounded-history semantics, and
 * subsequently report no changes. Instance handle reuse cannot resume
 * publication. Network connection setup and shutdown remain the caller's
 * responsibility; disconnection alone does not destroy alerts or stop local
 * observation.
 *
 * @param instance NetworkTables instance
 * @param publicationRoot exclusive publication subtree; no default is supplied
 */
void InstallAlertBackend(NetworkTableInstance instance,
                         std::string_view publicationRoot);

}  // namespace wpi::nt
