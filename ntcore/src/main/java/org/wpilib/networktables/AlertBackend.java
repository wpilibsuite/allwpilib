// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.networktables;

import java.util.Objects;

/** Native NetworkTables publication of ordinary {@link org.wpilib.util.Alert} objects. */
public final class AlertBackend {
  /**
   * Installs the native alert backend once at process startup.
   *
   * <p>Call before creating any alerts or alert readers. Reinstallation and changing backends while
   * the alert system is in use are unsupported, including after data reset or instance shutdown.
   * These usage restrictions are not checked at runtime. Existing alerts and handles are not
   * migrated to the new backend.
   *
   * <p>Topics are {@code <root>/<group>/<level>/<id>/text} (string) and {@code /active} (integer).
   * Active is zero when inactive, otherwise the activation time in nanoseconds from the local WPI
   * monotonic clock. The text topic's {@code mrcAlertIdentity} property contains {@code group},
   * {@code uniqueId}, and {@code level}. Empty names and slashes are preserved in group and ID;
   * identities mapping to the same path cannot coexist. Reserve this subtree for the backend.
   *
   * <p>Root normalization adds a leading slash, collapses repeated slashes, and removes trailing
   * slashes. The root must contain a non-slash character and no embedded NULs. The instance must be
   * valid and must not be concurrently reset or closed during installation.
   *
   * <p>The instance should outlive all alerts. Closing or resetting it unpublishes and invalidates
   * owned alerts, rejects future alert creation, and leaves readers readable with removal events
   * followed by no changes. Disconnection alone retains local alerts. The caller configures the NT
   * connection independently.
   *
   * @param instance explicit NetworkTables instance
   * @param publicationRoot exclusive publication subtree; no default is supplied
   * @throws NullPointerException if either argument is null
   */
  public static void install(NetworkTableInstance instance, String publicationRoot) {
    Objects.requireNonNull(instance, "instance");
    Objects.requireNonNull(publicationRoot, "publicationRoot");
    installImpl(instance.getHandle(), publicationRoot);
  }

  private static native void installImpl(int instance, String publicationRoot);

  private AlertBackend() {}
}
