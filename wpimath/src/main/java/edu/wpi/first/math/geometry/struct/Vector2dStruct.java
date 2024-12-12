package edu.wpi.first.math.geometry.struct;

import edu.wpi.first.math.geometry.Translation2d;
import edu.wpi.first.math.geometry.Vector2d;
import edu.wpi.first.util.struct.Struct;
import java.nio.ByteBuffer;

public class Vector2dStruct implements Struct<Vector2d> {
  @Override
  public Class<Vector2d> getTypeClass() {
    return Vector2d.class;
  }

  @Override
  public String getTypeName() {
    return "Vector2d";
  }

  @Override
  public int getSize() {
    return Translation2d.struct.getSize() * 2;
  }

  @Override
  public String getSchema() {
    return "Translation2d origin;Translation2d components";
  }

  @Override
  public Vector2d unpack(ByteBuffer bb) {
    var origin = Translation2d.struct.unpack(bb);
    var components = Translation2d.struct.unpack(bb);
    return new Vector2d(origin, components);
  }

  @Override
  public void pack(ByteBuffer bb, Vector2d value) {
    Translation2d.struct.pack(bb, value.getPosition());
    Translation2d.struct.pack(bb, value.getComponents());
  }

  @Override
  public Struct<?>[] getNested() {
    return new Struct<?>[] {Translation2d.struct};
  }
}
