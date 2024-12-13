package edu.wpi.first.math.geometry.struct;

import edu.wpi.first.math.geometry.BoundVector2d;
import edu.wpi.first.math.geometry.Translation2d;
import edu.wpi.first.util.struct.Struct;
import java.nio.ByteBuffer;

public class BoundVector2dStruct implements Struct<BoundVector2d> {
  @Override
  public Class<BoundVector2d> getTypeClass() {
    return BoundVector2d.class;
  }

  @Override
  public String getTypeName() {
    return "BoundVector2d";
  }

  @Override
  public int getSize() {
    return Translation2d.struct.getSize() * 2;
  }

  @Override
  public String getSchema() {
    return "Translation2d startPoint;Translation2d vector";
  }

  @Override
  public BoundVector2d unpack(ByteBuffer bb) {
    var startPoint = Translation2d.struct.unpack(bb);
    var vector = Translation2d.struct.unpack(bb);
    return new BoundVector2d(startPoint, vector);
  }

  @Override
  public void pack(ByteBuffer bb, BoundVector2d value) {
    Translation2d.struct.pack(bb, value.getStartPoint());
    Translation2d.struct.pack(bb, value.getVector());
  }

  @Override
  public Struct<?>[] getNested() {
    return new Struct<?>[] {Translation2d.struct};
  }
}
