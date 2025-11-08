// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.system.proto;

import edu.wpi.first.math.Matrix;
import edu.wpi.first.math.Nat;
import edu.wpi.first.math.Num;
import edu.wpi.first.math.proto.System.ProtobufLinearSystem;
import edu.wpi.first.math.proto.Wpimath.ProtobufMatrix;
import edu.wpi.first.math.system.LinearSystem;
import edu.wpi.first.util.protobuf.Protobuf;
import us.hebi.quickbuf.Descriptors.Descriptor;

public final class LinearSystemProto<States extends Num, Inputs extends Num, Outputs extends Num>
    implements Protobuf<LinearSystem<States, Inputs, Outputs>, ProtobufLinearSystem> {
  private final Nat<States> m_states;
  private final Nat<Inputs> m_inputs;
  private final Nat<Outputs> m_outputs;
  private final Protobuf<Matrix<States, States>, ProtobufMatrix> m_AProto;
  private final Protobuf<Matrix<States, Inputs>, ProtobufMatrix> m_BProto;
  private final Protobuf<Matrix<Outputs, States>, ProtobufMatrix> m_CProto;
  private final Protobuf<Matrix<Outputs, Inputs>, ProtobufMatrix> m_DProto;

  /**
   * Constructs the {@link Protobuf} implementation.
   *
   * @param states The number of states of the linear systems this serializer processes.
   * @param inputs The number of inputs of the linear systems this serializer processes.
   * @param outputs The number of outputs of the linear systems this serializer processes.
   */
  public LinearSystemProto(Nat<States> states, Nat<Inputs> inputs, Nat<Outputs> outputs) {
    m_states = states;
    m_inputs = inputs;
    m_outputs = outputs;
    m_AProto = Matrix.getProto(states, states);
    m_BProto = Matrix.getProto(states, inputs);
    m_CProto = Matrix.getProto(outputs, states);
    m_DProto = Matrix.getProto(outputs, inputs);
  }

  @Override
  public Class<LinearSystem<States, Inputs, Outputs>> getTypeClass() {
    @SuppressWarnings("unchecked")
    var clazz = (Class<LinearSystem<States, Inputs, Outputs>>) (Class<?>) LinearSystem.class;
    return clazz;
  }

  @Override
  public Descriptor getDescriptor() {
    return ProtobufLinearSystem.getDescriptor();
  }

  @Override
  public ProtobufLinearSystem createMessage() {
    return ProtobufLinearSystem.newInstance();
  }

  @Override
  public LinearSystem<States, Inputs, Outputs> unpack(ProtobufLinearSystem msg) {
    if (msg.getNumStates() != m_states.getNum()
        || msg.getNumInputs() != m_inputs.getNum()
        || msg.getNumOutputs() != m_outputs.getNum()) {
      throw new IllegalArgumentException(
          "Tried to unpack msg "
              + msg
              + " with "
              + msg.getNumStates()
              + " states and "
              + msg.getNumInputs()
              + " inputs and "
              + msg.getNumOutputs()
              + " outputs into LinearSystem with "
              + m_states.getNum()
              + " states "
              + m_inputs.getNum()
              + " inputs "
              + m_outputs.getNum()
              + " outputs");
    }
    return new LinearSystem<>(
        m_AProto.unpack(msg.getA()),
        m_BProto.unpack(msg.getB()),
        m_CProto.unpack(msg.getC()),
        m_DProto.unpack(msg.getD()));
  }

  @Override
  public void pack(ProtobufLinearSystem msg, LinearSystem<States, Inputs, Outputs> value) {
    msg.setNumStates(m_states.getNum())
        .setNumInputs(m_inputs.getNum())
        .setNumOutputs(m_outputs.getNum());
    m_AProto.pack(msg.getMutableA(), value.getA());
    m_BProto.pack(msg.getMutableB(), value.getB());
    m_CProto.pack(msg.getMutableC(), value.getC());
    m_DProto.pack(msg.getMutableD(), value.getD());
  }
}
