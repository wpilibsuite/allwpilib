// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.glass;

import edu.wpi.first.math.MathUtil;
import edu.wpi.first.util.struct.Struct;
import edu.wpi.first.wpilibj.util.Color8Bit;
import java.nio.ByteBuffer;

/**
 * Represents the data for a ngon element on a Canvas2d. To draw on a Canvas2d, use {@link
 * Canvas2d#drawNgon}.
 */
public class CanvasNgon2d {
  public final float m_x;
  public final float m_y;
  public final float m_radius;
  public final int m_numSides;
  public final float m_weight;
  public final boolean m_fill;
  public final Color8Bit m_color;
  public final int m_opacity;
  public final int m_zOrder;

  /**
   * Constructs a CanvasNgon2d.
   *
   * @param x The x coordinate of the center of the ngon
   * @param y The y coordinate of the center of the ngon
   * @param radius The radius of the ngon
   * @param numSides The number of sides of the ngon
   * @param weight The thickness of the ngon. For unfilled ngons.
   * @param fill Whether the ngon should be filled
   * @param color The color of the ngon
   * @param opacity The opacity of the ngon [0-255]
   * @param zOrder The z-order of the ngon
   */
  public CanvasNgon2d(
      float x,
      float y,
      float radius,
      int numSides,
      float weight,
      boolean fill,
      Color8Bit color,
      int opacity,
      int zOrder) {
    m_x = x;
    m_y = y;
    m_radius = radius;
    m_numSides = numSides;

    m_weight = weight;
    m_fill = fill;
    m_color = color;
    m_opacity = MathUtil.clamp(opacity, 0, 255);
    m_zOrder = zOrder;
  }

  public static class AStruct implements Struct<CanvasNgon2d> {
    @Override
    public Class<CanvasNgon2d> getTypeClass() {
      return CanvasNgon2d.class;
    }

    @Override
    public String getTypeString() {
      return "struct:CanvasNgon2d";
    }

    @Override
    public int getSize() {
      return kSizeFloat * 3 + kSizeInt32 + kSizeFloat + kSizeBool + kSizeInt8 * 4 + kSizeInt32;
    }

    @Override
    public String getSchema() {
      return "float x;float y;float radius;int32 numSides;float weight;bool fill;uint8 color[4];int32 zOrder";
    }

    @Override
    public CanvasNgon2d unpack(ByteBuffer bb) {
      float x = bb.getFloat();
      float y = bb.getFloat();
      float radius = bb.getFloat();
      int numSides = bb.getInt();
      float weight = bb.getFloat();
      boolean fill = bb.get() != 0;
      Color8Bit color = new Color8Bit(bb.get(), bb.get(), bb.get());
      int opacity = bb.get();
      int zOrder = bb.getInt();

      return new CanvasNgon2d(x, y, radius, numSides, weight, fill, color, opacity, zOrder);
    }

    @Override
    public void pack(ByteBuffer bb, CanvasNgon2d value) {
      bb.putFloat(value.m_x);
      bb.putFloat(value.m_y);
      bb.putFloat(value.m_radius);
      bb.putInt(value.m_numSides);
      bb.putFloat(value.m_weight);
      bb.put((byte) (value.m_fill ? 1 : 0));
      bb.put((byte) value.m_color.red);
      bb.put((byte) value.m_color.green);
      bb.put((byte) value.m_color.blue);
      bb.put((byte) value.m_opacity);
      bb.putInt(value.m_zOrder);
    }
  }

  public static final AStruct struct = new AStruct();
}
