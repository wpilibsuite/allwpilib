// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.glass;

import edu.wpi.first.math.MathUtil;
import edu.wpi.first.util.struct.Struct;
import edu.wpi.first.wpilibj.util.Color8Bit;
import java.nio.ByteBuffer;

/**
 * Represents a quad element on a Canvas2d. To draw on a Canvas2d, use {@link Canvas2d#drawQuad} or
 * {@link Canvas2d#drawRect}.
 */
public class CanvasQuad2d {
  public final float m_x1;
  public final float m_y1;
  public final float m_x2;
  public final float m_y2;
  public final float m_x3;
  public final float m_y3;
  public final float m_x4;
  public final float m_y4;
  public final float m_weight;
  public final boolean m_fill;
  public final Color8Bit m_color;
  public final int m_opacity;
  public final int m_zOrder;

  /**
   * Constructs a CanvasQuad2d.
   *
   * @param x1 The x coordinate of the first point
   * @param y1 The y coordinate of the first point
   * @param x2 The x coordinate of the second point
   * @param y2 The y coordinate of the second point
   * @param x3 The x coordinate of the third point
   * @param y3 The y coordinate of the third point
   * @param x4 The x coordinate of the fourth point
   * @param y4 The y coordinate of the fourth point
   * @param weight The thickness of the outline. For unfilled quads.
   * @param fill Whether the quad should be filled
   * @param color The color of the quad
   * @param opacity The opacity of the quad [0-255]
   * @param zOrder The z-order of the quad
   */
  public CanvasQuad2d(
      float x1,
      float y1,
      float x2,
      float y2,
      float x3,
      float y3,
      float x4,
      float y4,
      float weight,
      boolean fill,
      Color8Bit color,
      int opacity,
      int zOrder) {
    m_x1 = x1;
    m_y1 = y1;
    m_x2 = x2;
    m_y2 = y2;
    m_x3 = x3;
    m_y3 = y3;
    m_x4 = x4;
    m_y4 = y4;

    m_weight = weight;
    m_fill = fill;
    m_color = color;
    m_opacity = MathUtil.clamp(opacity, 0, 255);
    m_zOrder = zOrder;
  }

  public static class AStruct implements Struct<CanvasQuad2d> {
    @Override
    public Class<CanvasQuad2d> getTypeClass() {
      return CanvasQuad2d.class;
    }

    @Override
    public String getTypeString() {
      return "struct:CanvasQuad2d";
    }

    @Override
    public int getSize() {
      return kSizeFloat * 9 + kSizeBool + kSizeInt8 * 4 + kSizeInt32;
    }

    @Override
    public String getSchema() {
      return "float x1;float y1;float x2;float y2;float x3;float y3;float x4;float y4;float weight;bool fill;uint8 color[4];int32 zOrder";
    }

    @Override
    public CanvasQuad2d unpack(ByteBuffer bb) {
      float x1 = bb.getFloat();
      float y1 = bb.getFloat();
      float x2 = bb.getFloat();
      float y2 = bb.getFloat();
      float x3 = bb.getFloat();
      float y3 = bb.getFloat();
      float x4 = bb.getFloat();
      float y4 = bb.getFloat();
      float weight = bb.getFloat();
      boolean fill = bb.get() != 0;
      Color8Bit color = new Color8Bit(bb.get(), bb.get(), bb.get());
      int opacity = bb.get();
      int zOrder = bb.getInt();

      return new CanvasQuad2d(x1, y1, x2, y2, x3, y3, x4, y4, weight, fill, color, opacity, zOrder);
    }

    @Override
    public void pack(ByteBuffer bb, CanvasQuad2d value) {
      bb.putFloat(value.m_x1);
      bb.putFloat(value.m_y1);
      bb.putFloat(value.m_x2);
      bb.putFloat(value.m_y2);
      bb.putFloat(value.m_x3);
      bb.putFloat(value.m_y3);
      bb.putFloat(value.m_x4);
      bb.putFloat(value.m_y4);
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
