// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.glass;

import edu.wpi.first.math.MathUtil;
import edu.wpi.first.util.struct.Struct;
import edu.wpi.first.wpilibj.util.Color8Bit;
import java.nio.ByteBuffer;

/**
 * Represents the data for a line element on a Canvas2d. To draw on a Canvas2d, use {@link
 * Canvas2d#drawLine}.
 */
public class CanvasLine2d {
  public final float m_x1;
  public final float m_y1;
  public final float m_x2;
  public final float m_y2;
  public final float m_weight;
  public final Color8Bit m_color;
  public final int m_opacity;
  public final int m_zOrder;

  /**
   * Constructs a CanvasLine2d.
   *
   * @param x1 The x coordinate of the first point
   * @param y1 The y coordinate of the first point
   * @param x2 The x coordinate of the second point
   * @param y2 The y coordinate of the second point
   * @param weight The thickness of the line
   * @param color The color of the line
   * @param opacity The opacity of the line [0-255]
   * @param zOrder The z-order of the line
   */
  public CanvasLine2d(
      float x1,
      float y1,
      float x2,
      float y2,
      float weight,
      Color8Bit color,
      int opacity,
      int zOrder) {
    m_x1 = x1;
    m_y1 = y1;
    m_x2 = x2;
    m_y2 = y2;

    m_weight = weight;
    m_color = color;
    m_opacity = MathUtil.clamp(opacity, 0, 255);
    m_zOrder = zOrder;
  }

  public static class AStruct implements Struct<CanvasLine2d> {
    @Override
    public Class<CanvasLine2d> getTypeClass() {
      return CanvasLine2d.class;
    }

    @Override
    public String getTypeString() {
      return "struct:CanvasLine2d";
    }

    @Override
    public int getSize() {
      return kSizeFloat * 5 + kSizeInt8 * 4 + kSizeInt32;
    }

    @Override
    public String getSchema() {
      return "float x1;float y1;float x2;float y2;float weight;uint8 color[4];int32 zOrder";
    }

    @Override
    public CanvasLine2d unpack(ByteBuffer bb) {
      float x1 = bb.getFloat();
      float y1 = bb.getFloat();
      float x2 = bb.getFloat();
      float y2 = bb.getFloat();
      float weight = bb.getFloat();
      Color8Bit color = new Color8Bit(bb.get(), bb.get(), bb.get());
      int opacity = bb.get();
      int zOrder = bb.getInt();

      return new CanvasLine2d(x1, y1, x2, y2, weight, color, opacity, zOrder);
    }

    @Override
    public void pack(ByteBuffer bb, CanvasLine2d value) {
      bb.putFloat(value.m_x1);
      bb.putFloat(value.m_y1);
      bb.putFloat(value.m_x2);
      bb.putFloat(value.m_y2);
      bb.putFloat(value.m_weight);
      bb.put((byte) value.m_color.red);
      bb.put((byte) value.m_color.green);
      bb.put((byte) value.m_color.blue);
      bb.put((byte) value.m_opacity);
      bb.putInt(value.m_zOrder);
    }
  }

  public static final AStruct struct = new AStruct();
}
