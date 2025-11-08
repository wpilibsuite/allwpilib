// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.system.struct;

import edu.wpi.first.math.Matrix;
import edu.wpi.first.math.Nat;
import edu.wpi.first.math.Num;
import edu.wpi.first.math.struct.MatrixStruct;
import edu.wpi.first.math.system.LinearSystem;
import edu.wpi.first.util.struct.Struct;
import java.nio.ByteBuffer;

public final class LinearSystemStruct<States extends Num, Inputs extends Num, Outputs extends Num>
    implements Struct<LinearSystem<States, Inputs, Outputs>> {
  private final int m_states;
  private final int m_inputs;
  private final int m_outputs;
  private final MatrixStruct<States, States> m_AStruct;
  private final MatrixStruct<States, Inputs> m_BStruct;
  private final MatrixStruct<Outputs, States> m_CStruct;
  private final MatrixStruct<Outputs, Inputs> m_DStruct;

  /**
   * Constructs the {@link Struct} implementation.
   *
   * @param states The number of states of the linear systems this serializer processes.
   * @param inputs The number of inputs of the linear systems this serializer processes.
   * @param outputs The number of outputs of the linear systems this serializer processes.
   */
  public LinearSystemStruct(Nat<States> states, Nat<Inputs> inputs, Nat<Outputs> outputs) {
    m_states = states.getNum();
    m_inputs = inputs.getNum();
    m_outputs = outputs.getNum();
    m_AStruct = Matrix.getStruct(states, states);
    m_BStruct = Matrix.getStruct(states, inputs);
    m_CStruct = Matrix.getStruct(outputs, states);
    m_DStruct = Matrix.getStruct(outputs, inputs);
  }

  @Override
  public Class<LinearSystem<States, Inputs, Outputs>> getTypeClass() {
    @SuppressWarnings("unchecked")
    var clazz = (Class<LinearSystem<States, Inputs, Outputs>>) (Class<?>) LinearSystem.class;
    return clazz;
  }

  @Override
  public String getTypeName() {
    return "LinearSystem__" + m_states + "_" + m_inputs + "_" + m_outputs;
  }

  @Override
  public int getSize() {
    return m_AStruct.getSize() + m_BStruct.getSize() + m_CStruct.getSize() + m_DStruct.getSize();
  }

  @Override
  public String getSchema() {
    return m_AStruct.getTypeName()
        + " a;"
        + m_BStruct.getTypeName()
        + " b;"
        + m_CStruct.getTypeName()
        + " c;"
        + m_DStruct.getTypeName()
        + " d";
  }

  @Override
  public Struct<?>[] getNested() {
    return new Struct<?>[] {m_AStruct, m_BStruct, m_CStruct, m_DStruct};
  }

  @Override
  public LinearSystem<States, Inputs, Outputs> unpack(ByteBuffer bb) {
    return new LinearSystem<>(
        m_AStruct.unpack(bb), m_BStruct.unpack(bb), m_CStruct.unpack(bb), m_DStruct.unpack(bb));
  }

  @Override
  public void pack(ByteBuffer bb, LinearSystem<States, Inputs, Outputs> value) {
    m_AStruct.pack(bb, value.getA());
    m_BStruct.pack(bb, value.getB());
    m_CStruct.pack(bb, value.getC());
    m_DStruct.pack(bb, value.getD());
  }
}
