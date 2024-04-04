// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command;

import static org.junit.jupiter.api.Assertions.assertArrayEquals;

import edu.wpi.first.networktables.BooleanPublisher;
import edu.wpi.first.networktables.NetworkTableInstance;
import edu.wpi.first.wpilibj.smartdashboard.SmartDashboard;
import java.util.stream.Stream;
import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.Arguments;
import org.junit.jupiter.params.provider.MethodSource;

class SendableChooserCommandTest extends CommandTestBase {
  private NetworkTableInstance m_inst;
  private BooleanPublisher m_publish;
  private static final String kBasePath = "/SmartDashboard/chooser/";

  @BeforeEach
  void setUp() {
    m_inst = NetworkTableInstance.create();
    SmartDashboard.setNetworkTableInstance(m_inst);
    m_publish = m_inst.getBooleanTopic("/SmartDashboard/chooser").publish();
    SmartDashboard.updateValues();
  }

  @ParameterizedTest(name = "options[{index}]: {0}")
  @MethodSource
  void optionsAreCorrect(
      @SuppressWarnings("unused") String testName, Command[] commands, String[] names) {
    try (var optionsSubscriber =
        m_inst.getStringArrayTopic(kBasePath + "options").subscribe(new String[] {})) {
      @SuppressWarnings("unused")
      var command = Commands.choose(c -> SmartDashboard.putData("chooser", c), commands);
      SmartDashboard.updateValues();
      assertArrayEquals(names, optionsSubscriber.get());
    }
  }

  static Stream<Arguments> optionsAreCorrect() {
    return Stream.of(
        Arguments.of("empty", new Command[] {}, new String[] {}),
        Arguments.of(
            "duplicateName",
            new Command[] {commandNamed("a"), commandNamed("b"), commandNamed("a")},
            new String[] {"a", "b"}),
        Arguments.of(
            "happyPath",
            new Command[] {commandNamed("a"), commandNamed("b"), commandNamed("c")},
            new String[] {"a", "b", "c"}));
  }

  @AfterEach
  void tearDown() {
    m_publish.close();
    m_inst.close();
    SmartDashboard.setNetworkTableInstance(NetworkTableInstance.getDefault());
  }

  private static Command commandNamed(String name) {
    return Commands.print(name).withName(name);
  }
}
