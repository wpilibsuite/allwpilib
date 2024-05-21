// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.glass;

import edu.wpi.first.math.MathUtil;
import edu.wpi.first.util.struct.Struct;
import edu.wpi.first.wpilibj.util.Color8Bit;
import java.nio.ByteBuffer;

/**
 * Represents the data for a text element on a Canvas2d. To draw on a Canvas2d, use {@link
 * Canvas2d#drawText}.
 */
class CanvasText2d {
  public final float m_x;
  public final float m_y;
  public final float m_fontSize;
  public final char[] m_text;
  public final float m_wrapWidth;
  public final Color8Bit m_color;
  public final int m_opacity;
  public final int m_zOrder;

  /**
   * Constructs a CanvasText2d.
   *
   * @param x The x coordinate of the text
   * @param y The y coordinate of the text
   * @param fontSize The size of the text
   * @param wrapWidth The width at which the text should wrap. 0 for no wrap
   * @param text The text. The max of length the text is 64 characters
   * @param color The color of the text
   * @param opacity The opacity of the text [0-255]
   * @param zOrder The z-order of the text
   */
  public CanvasText2d(
      float x,
      float y,
      float fontSize,
      String text,
      Color8Bit color,
      int opacity,
      float wrapWidth,
      int zOrder) {
    this(x, y, fontSize, text.toCharArray(), color, opacity, wrapWidth, zOrder);
  }

  /**
   * Constructs a CanvasText2d.
   *
   * @param x The x coordinate of the text
   * @param y The y coordinate of the text
   * @param size The size of the text
   * @param wrapWidth The width at which the text should wrap
   * @param text The text. The max of length the text is 64 characters
   * @param color The color of the text
   * @param opacity The opacity of the text [0-255]
   * @param zOrder The z-order of the text
   */
  public CanvasText2d(
      float x,
      float y,
      float size,
      char[] text,
      Color8Bit color,
      int opacity,
      float wrapWidth,
      int zOrder) {
    // TODO: Warn on long text?
    m_x = x;
    m_y = y;
    m_fontSize = size;
    m_text = new char[64];
    for (int i = 0; i < Math.min(text.length, m_text.length); i++) {
      m_text[i] = text[i];
    }
    m_color = color;
    m_opacity = MathUtil.clamp(opacity, 0, 255);
    m_wrapWidth = wrapWidth;
    m_zOrder = zOrder;
  }

  public static class AStruct implements Struct<CanvasText2d> {
    @Override
    public Class<CanvasText2d> getTypeClass() {
      return CanvasText2d.class;
    }

    @Override
    public String getTypeString() {
      return "struct:CanvasText2d";
    }

    @Override
    public int getSize() {
      return kSizeFloat * 3 + kSizeInt8 * 64 + kSizeFloat + kSizeInt8 * 4 + kSizeInt32;
    }

    @Override
    public String getSchema() {
      return "float x;float y;float fontSize;char text[64];uint8 color[4];float wrapWidth;int32 zOrder";
    }

    @Override
    public CanvasText2d unpack(ByteBuffer bb) {
      float x = bb.getFloat();
      float y = bb.getFloat();
      float fontSize = bb.getFloat();
      char[] text = new char[64];
      for (int i = 0; i < 64; i++) {
        text[i] = (char) bb.get();
      }
      Color8Bit color = new Color8Bit(bb.get(), bb.get(), bb.get());
      int opacity = bb.get();
      float wrapWidth = bb.getFloat();
      int zOrder = bb.getInt();

      return new CanvasText2d(x, y, fontSize, text, color, opacity, wrapWidth, zOrder);
    }

    @Override
    public void pack(ByteBuffer bb, CanvasText2d value) {
      bb.putFloat(value.m_x);
      bb.putFloat(value.m_y);
      bb.putFloat(value.m_fontSize);
      for (int i = 0; i < value.m_text.length; i++) {
        bb.put((byte) value.m_text[i]);
      }
      for (int i = value.m_text.length; i < 64; i++) {
        bb.put((byte) 0);
      }
      bb.put((byte) value.m_color.red);
      bb.put((byte) value.m_color.green);
      bb.put((byte) value.m_color.blue);
      bb.put((byte) value.m_opacity);
      bb.putFloat(value.m_wrapWidth);
      bb.putInt(value.m_zOrder);
    }
  }

  public static final AStruct struct = new AStruct();
}
