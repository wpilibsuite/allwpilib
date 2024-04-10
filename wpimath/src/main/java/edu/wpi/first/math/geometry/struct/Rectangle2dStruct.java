package edu.wpi.first.math.geometry.struct;

import java.nio.ByteBuffer;

import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.geometry.Rectangle2d;
import edu.wpi.first.util.struct.Struct;

public class Rectangle2dStruct implements Struct<Rectangle2d> {
  @Override
  public Class<Rectangle2d> getTypeClass() {
    return Rectangle2d.class;
  }

  @Override
  public String getTypeString() {
    return "struct:Rectangle2d";
  }

  @Override
  public int getSize() {
    return Pose2d.struct.getSize() + kSizeDouble + kSizeDouble;
  }

  @Override
  public String getSchema() {
    return "Pose2d center;double width; double height";
  }

  @Override
  public Struct<?>[] getNested() {
    return new Struct<?>[] {Pose2d.struct};
  }

  @Override
  public Rectangle2d unpack(ByteBuffer bb) {
    Pose2d center = Pose2d.struct.unpack(bb);
    double width = bb.getDouble();
    double height = bb.getDouble();
    return new Rectangle2d(center, width, height);
  }

  @Override
  public void pack(ByteBuffer bb, Rectangle2d value) {
    Pose2d.struct.pack(bb, value.getCenter());
    bb.putDouble(value.getWidth());
    bb.putDouble(value.getHeight());
  } 
}
