// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.glass;

import edu.wpi.first.networktables.FloatArrayPublisher;
import edu.wpi.first.networktables.NTSendable;
import edu.wpi.first.networktables.NTSendableBuilder;
import edu.wpi.first.networktables.NetworkTable;
import edu.wpi.first.networktables.StructArrayPublisher;
import edu.wpi.first.wpilibj.util.Color8Bit;
import java.util.ArrayList;
import java.util.List;

public class Canvas2d implements NTSendable, AutoCloseable {
  private final float[] m_dims = new float[2];

  private final List<CanvasLine2d> m_lines = new ArrayList<>();
  private final List<CanvasQuad2d> m_quads = new ArrayList<>();
  private final List<CanvasCircle2d> m_circles = new ArrayList<>();
  private final List<CanvasNgon2d> m_ngons = new ArrayList<>();
  private final List<CanvasText2d> m_texts = new ArrayList<>();

  private boolean m_sendableInitialized = false;
  private FloatArrayPublisher m_dimsPub;
  private StructArrayPublisher<CanvasLine2d> m_linesPub;
  private StructArrayPublisher<CanvasQuad2d> m_quadsPub;
  private StructArrayPublisher<CanvasCircle2d> m_circlesPub;
  private StructArrayPublisher<CanvasNgon2d> m_ngonsPub;
  private StructArrayPublisher<CanvasText2d> m_textsPub;

  private int m_currentZOrder = 0;

  /**
   * Constructs a Canvas2d.
   *
   * @param width The width of the canvas
   * @param length The length of the canvas
   */
  public Canvas2d(float width, float length) {
    m_dims[0] = width;
    m_dims[1] = length;
  }

  /** Clears the canvas. */
  public void clear() {
    m_currentZOrder = 0;
    m_lines.clear();
    m_quads.clear();
    m_circles.clear();
    m_ngons.clear();
    m_texts.clear();
  }

  /**
   * Draws a line on the canvas.
   *
   * @param x1 The x coordinate of the first point
   * @param y1 The y coordinate of the first point
   * @param x2 The x coordinate of the second point
   * @param y2 The y coordinate of the second point
   * @param weight The thickness of the line
   * @param color The color of the line
   * @param opacity The opacity of the line [0-255]
   */
  public void drawLine(
      float x1, float y1, float x2, float y2, float weight, Color8Bit color, int opacity) {
    m_lines.add(new CanvasLine2d(x1, y1, x2, y2, weight, color, opacity, m_currentZOrder++));
  }

  /**
   * Draws a rectangle on the canvas.
   *
   * @param x1 The x coordinate of the first point
   * @param y1 The y coordinate of the first point
   * @param x2 The x coordinate of the second point
   * @param y2 The y coordinate of the second point
   * @param weight The thickness of the outline. For unfilled rectangles.
   * @param fill Whether the rectangle should be filled
   * @param color The color of the rectangle
   * @param opacity The opacity of the rectangle [0-255]
   */
  public void drawRect(
      float x1,
      float y1,
      float x2,
      float y2,
      float weight,
      boolean fill,
      Color8Bit color,
      int opacity) {
    drawQuad(x1, y1, x2, y1, x2, y2, x1, y2, weight, fill, color, opacity);
  }

  /**
   * Draws a quad on the canvas.
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
   */
  public void drawQuad(
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
      int opacity) {
    m_quads.add(
        new CanvasQuad2d(
            x1, y1, x2, y2, x3, y3, x4, y4, weight, fill, color, opacity, m_currentZOrder++));
  }

  /**
   * Draws a circle on the canvas.
   *
   * @param x The x coordinate of the center of the circle
   * @param y The y coordinate of the center of the circle
   * @param radius The radius of the circle
   * @param weight The thickness of the circle. For unfilled circles.
   * @param fill Whether the circle should be filled
   * @param color The color of the circle
   * @param opacity The opacity of the circle [0-255]
   */
  public void drawCircle(
      float x, float y, float radius, float weight, boolean fill, Color8Bit color, int opacity) {
    m_circles.add(
        new CanvasCircle2d(x, y, radius, weight, fill, color, opacity, m_currentZOrder++));
  }

  /**
   * Draws an ngon on the canvas.
   *
   * @param x The x coordinate of the center of the ngon
   * @param y The y coordinate of the center of the ngon
   * @param radius The radius of the ngon
   * @param numSides The number of sides of the ngon
   * @param weight The thickness of the ngon. For unfilled ngons.
   * @param fill Whether the ngon should be filled
   * @param color The color of the ngon
   * @param opacity The opacity of the ngon [0-255]
   */
  public void drawNgon(
      float x,
      float y,
      float radius,
      int numSides,
      float weight,
      boolean fill,
      Color8Bit color,
      int opacity) {
    m_ngons.add(
        new CanvasNgon2d(x, y, radius, numSides, weight, fill, color, opacity, m_currentZOrder++));
  }

  /**
   * Draws text on the canvas.
   *
   * @param x The x coordinate of the text
   * @param y The y coordinate of the text
   * @param fontSize The size of the text
   * @param text The text. The max length of the text is 64 characters
   * @param color The color of the text
   * @param opacity The opacity of the text [0-255]
   * @param wrapWidth The width at which the text should wrap. 0 for no wrap
   */
  public void drawText(
      float x,
      float y,
      float fontSize,
      String text,
      Color8Bit color,
      int opacity,
      float wrapWidth) {
    m_texts.add(
        new CanvasText2d(x, y, fontSize, text, color, opacity, wrapWidth, m_currentZOrder++));
  }

  /** Finish and push the frame to Sendable. Clears the canvas after pushing the frame. */
  public void finishFrame() {
    finishFrame(true);
  }

  /**
   * Finish and push the frame to Sendable. If sendable is not initialized, this does nothing.
   *
   * @param clearCanvas Whether to clear the canvas after pushing the frame.
   */
  public void finishFrame(boolean clearCanvas) {
    if (!m_sendableInitialized) {
      return;
    }
    m_linesPub.set(m_lines.toArray(new CanvasLine2d[0]));
    m_quadsPub.set(m_quads.toArray(new CanvasQuad2d[0]));
    m_circlesPub.set(m_circles.toArray(new CanvasCircle2d[0]));
    m_ngonsPub.set(m_ngons.toArray(new CanvasNgon2d[0]));
    m_textsPub.set(m_texts.toArray(new CanvasText2d[0]));

    if (clearCanvas) {
      clear();
    }
  }

  @Override
  public void initSendable(NTSendableBuilder builder) {
    m_sendableInitialized = true;

    builder.setSmartDashboardType("Canvas2d");

    NetworkTable table = builder.getTable();
    if (m_dimsPub != null) {
      m_dimsPub.close();
    }
    m_dimsPub = table.getFloatArrayTopic("dims").publish();
    m_dimsPub.set(m_dims);

    if (m_linesPub != null) {
      m_linesPub.close();
    }
    m_linesPub = table.getStructArrayTopic("lines", CanvasLine2d.struct).publish();
    m_linesPub.set(new CanvasLine2d[0]);

    if (m_quadsPub != null) {
      m_quadsPub.close();
    }
    m_quadsPub = table.getStructArrayTopic("quads", CanvasQuad2d.struct).publish();
    m_quadsPub.set(new CanvasQuad2d[0]);

    if (m_circlesPub != null) {
      m_circlesPub.close();
    }
    m_circlesPub = table.getStructArrayTopic("circles", CanvasCircle2d.struct).publish();
    m_circlesPub.set(new CanvasCircle2d[0]);

    if (m_ngonsPub != null) {
      m_ngonsPub.close();
    }
    m_ngonsPub = table.getStructArrayTopic("ngons", CanvasNgon2d.struct).publish();
    m_ngonsPub.set(new CanvasNgon2d[0]);

    if (m_textsPub != null) {
      m_textsPub.close();
    }
    m_textsPub = table.getStructArrayTopic("texts", CanvasText2d.struct).publish();
    m_textsPub.set(new CanvasText2d[0]);
  }

  @Override
  public void close() {
    if (m_dimsPub != null) {
      m_dimsPub.close();
    }
    if (m_linesPub != null) {
      m_linesPub.close();
    }
    if (m_quadsPub != null) {
      m_quadsPub.close();
    }
    if (m_circlesPub != null) {
      m_circlesPub.close();
    }
    if (m_ngonsPub != null) {
      m_ngonsPub.close();
    }
    if (m_textsPub != null) {
      m_textsPub.close();
    }
  }
}
