// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.glass;

import java.util.ArrayList;
import java.util.List;

import edu.wpi.first.networktables.FloatArrayPublisher;
import edu.wpi.first.networktables.NTSendable;
import edu.wpi.first.networktables.NTSendableBuilder;
import edu.wpi.first.networktables.NetworkTable;
import edu.wpi.first.networktables.StructArrayPublisher;
import edu.wpi.first.wpilibj.util.Color8Bit;

public class Canvas2d implements NTSendable, AutoCloseable {
  private final float[] m_dims = new float[2];

  // TODO: Seperate list of each primitive type, each one with its own NT topic
  private final List<CanvasLine2d> m_lines = new ArrayList<>();

  private boolean m_sendableInitialized = false;
  private FloatArrayPublisher m_dimsPub;
  private StructArrayPublisher<CanvasLine2d> m_linesPub;

  private int m_currentZOrder = 0;

  public Canvas2d(float width, float length) {
    m_dims[0] = width;
    m_dims[1] = length;
  }

  public void draw(CanvasComplexObject2d object) {
    object.drawOn(this);
  }

  public void clear() {
    m_lines.clear();
  }

  public void drawLine(float x1, float y1, float x2, float y2, float thickness, Color8Bit color) {
    m_lines.add(new CanvasLine2d(x1, y1, x2, y2, thickness, color, m_currentZOrder++));
  }

  public void finishFrame() {
    if (!m_sendableInitialized) {
      return;
    }
    m_linesPub.set(m_lines.toArray(new CanvasLine2d[0]));
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
  }

  @Override
  public void close() {
    if (m_dimsPub != null) {
      m_dimsPub.close();
    }
    if (m_linesPub != null) {
      m_linesPub.close();
    }
  }
}
