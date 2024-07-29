// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.kinematics.struct;

import edu.wpi.first.math.geometry.Translation2d;
import edu.wpi.first.math.kinematics.SwerveDriveKinematics;
import edu.wpi.first.util.struct.Struct;
import java.nio.ByteBuffer;

public final class SwerveDriveKinematicsStruct implements Struct<SwerveDriveKinematics> {
  private final int m_numModules;

  /**
   * Constructs the {@link Struct} implementation.
   *
   * @param numModules the number of modules of the kinematics objects this serializer processes.
   */
  public SwerveDriveKinematicsStruct(int numModules) {
    m_numModules = numModules;
  }

  @Override
  public Class<SwerveDriveKinematics> getTypeClass() {
    return SwerveDriveKinematics.class;
  }

  @Override
  public String getTypeName() {
    return "SwerveDriveKinematics__" + m_numModules;
  }

  @Override
  public int getSize() {
    return Translation2d.struct.getSize() * m_numModules;
  }

  @Override
  public String getSchema() {
    return "Translation2d modules[" + m_numModules + "]";
  }

  @Override
  public Struct<?>[] getNested() {
    return new Struct<?>[] {Translation2d.struct};
  }

  @Override
  public SwerveDriveKinematics unpack(ByteBuffer bb) {
    return new SwerveDriveKinematics(Struct.unpackArray(bb, m_numModules, Translation2d.struct));
  }

  @Override
  public void pack(ByteBuffer bb, SwerveDriveKinematics value) {
    Struct.packArray(bb, value.getModules(), Translation2d.struct);
  }
}
