// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.javacplugin;

import static com.google.testing.compile.CompilationSubject.assertThat;
import static com.google.testing.compile.Compiler.javac;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.wpilib.javacplugin.CompileTestUtils.JAVA_VERSION_OPTIONS;

import com.google.testing.compile.Compilation;
import com.google.testing.compile.JavaFileObjects;
import org.junit.jupiter.api.Test;

class AddStateAfterSwitchFromAnyDetectorTest {
  private static final String STATE_MACHINE_SOURCE =
      """
      package org.wpilib.command3;

      import java.util.function.BooleanSupplier;

      public final class StateMachine {
        public StateMachine(String name) {}

        public State addState(Object command) {
          return new State();
        }

        public TransitionBuilder switchFromAny(State... states) {
          return new TransitionBuilder();
        }

        public static class State {
          public TransitionBuilder switchTo(State other) {
            return new TransitionBuilder();
          }
        }

        public static class TransitionBuilder {
          public TransitionBuilder toExitStateMachine() {
            return this;
          }

          public TransitionBuilder to(State state) {
            return this;
          }

          public void when(BooleanSupplier condition) {}
          public void whenComplete() {}
        }
      }
      """;

  @Test
  void addStateBeforeSwitchFromAnySucceeds() {
    String source =
        """
        package wpilib.robot;

        import org.wpilib.command3.StateMachine;

        class Example {
          void setup() {
            StateMachine sm = new StateMachine("SM");
            var state1 = sm.addState(new Object());
            var state2 = sm.addState(new Object());
            sm.switchFromAny().toExitStateMachine().when(() -> true);
          }
        }
        """;

    Compilation compilation =
        javac()
            .withOptions(JAVA_VERSION_OPTIONS)
            .compile(
                JavaFileObjects.forSourceString(
                    "org.wpilib.command3.StateMachine", STATE_MACHINE_SOURCE),
                JavaFileObjects.forSourceString("wpilib.robot.Example", source));

    assertThat(compilation).succeededWithoutWarnings();
  }

  @Test
  void addStateAfterNoArgSwitchFromAnyFails() {
    String source =
        """
        package wpilib.robot;

        import org.wpilib.command3.StateMachine;

        class Example {
          void setup() {
            StateMachine sm = new StateMachine("SM");
            var state1 = sm.addState(new Object());
            sm.switchFromAny().toExitStateMachine().when(() -> true);
            var state2 = sm.addState(new Object());
          }
        }
        """;

    Compilation compilation =
        javac()
            .withOptions(JAVA_VERSION_OPTIONS)
            .compile(
                JavaFileObjects.forSourceString(
                    "org.wpilib.command3.StateMachine", STATE_MACHINE_SOURCE),
                JavaFileObjects.forSourceString("wpilib.robot.Example", source));

    assertThat(compilation).failed();
    assertEquals(1, compilation.errors().size());
    var error = compilation.errors().get(0);
    assertEquals(
        AddStateAfterSwitchFromAnyDetector.ERROR_MESSAGE_FORMAT.formatted("sm"),
        error.getMessage(null));
  }

  @Test
  void addStateAfterExplicitSwitchFromAnySucceeds() {
    String source =
        """
        package wpilib.robot;

        import org.wpilib.command3.StateMachine;

        class Example {
          void setup() {
            StateMachine sm = new StateMachine("SM");
            var state1 = sm.addState(new Object());
            var state2 = sm.addState(new Object());
            sm.switchFromAny(state1, state2).toExitStateMachine().when(() -> true);
            var state3 = sm.addState(new Object());
          }
        }
        """;

    Compilation compilation =
        javac()
            .withOptions(JAVA_VERSION_OPTIONS)
            .compile(
                JavaFileObjects.forSourceString(
                    "org.wpilib.command3.StateMachine", STATE_MACHINE_SOURCE),
                JavaFileObjects.forSourceString("wpilib.robot.Example", source));

    assertThat(compilation).succeededWithoutWarnings();
  }

  @Test
  void twoStateMachinesInScope() {
    String source =
        """
        package wpilib.robot;

        import org.wpilib.command3.StateMachine;

        class Example {
          void setup() {
            StateMachine sm1 = new StateMachine("SM1");
            StateMachine sm2 = new StateMachine("SM2");

            var state1 = sm1.addState(new Object());
            var state2 = sm2.addState(new Object());

            // sm1 is closed with no-arg switchFromAny
            sm1.switchFromAny().toExitStateMachine().when(() -> true);

            // sm2 is still open, so adding a state to sm2 is valid
            var state3 = sm2.addState(new Object());

            // adding state to sm1 is invalid
            var state4 = sm1.addState(new Object());
          }
        }
        """;

    Compilation compilation =
        javac()
            .withOptions(JAVA_VERSION_OPTIONS)
            .compile(
                JavaFileObjects.forSourceString(
                    "org.wpilib.command3.StateMachine", STATE_MACHINE_SOURCE),
                JavaFileObjects.forSourceString("wpilib.robot.Example", source));

    assertThat(compilation).failed();
    assertEquals(1, compilation.errors().size());
    var error = compilation.errors().get(0);
    assertEquals(
        AddStateAfterSwitchFromAnyDetector.ERROR_MESSAGE_FORMAT.formatted("sm1"),
        error.getMessage(null));
  }

  @Test
  void suppressedWarning() {
    String source =
        """
        package wpilib.robot;

        import org.wpilib.command3.StateMachine;

        class Example {
          @SuppressWarnings("WPILib.AddStateAfterSwitchFromAny")
          void setup() {
            StateMachine sm = new StateMachine("SM");
            var state1 = sm.addState(new Object());
            sm.switchFromAny().toExitStateMachine().when(() -> true);
            var state2 = sm.addState(new Object());
          }
        }
        """;

    Compilation compilation =
        javac()
            .withOptions(JAVA_VERSION_OPTIONS)
            .compile(
                JavaFileObjects.forSourceString(
                    "org.wpilib.command3.StateMachine", STATE_MACHINE_SOURCE),
                JavaFileObjects.forSourceString("wpilib.robot.Example", source));

    assertThat(compilation).succeededWithoutWarnings();
  }

  @Test
  void stateMachineFieldUsage() {
    String source =
        """
        package wpilib.robot;

        import org.wpilib.command3.StateMachine;

        class Example {
          StateMachine m_sm = new StateMachine("SM");

          void setup() {
            var state1 = m_sm.addState(new Object());
            m_sm.switchFromAny().toExitStateMachine().when(() -> true);
            var state2 = m_sm.addState(new Object());
          }
        }
        """;

    Compilation compilation =
        javac()
            .withOptions(JAVA_VERSION_OPTIONS)
            .compile(
                JavaFileObjects.forSourceString(
                    "org.wpilib.command3.StateMachine", STATE_MACHINE_SOURCE),
                JavaFileObjects.forSourceString("wpilib.robot.Example", source));

    assertThat(compilation).failed();
    assertEquals(1, compilation.errors().size());
    var error = compilation.errors().get(0);
    assertEquals(
        AddStateAfterSwitchFromAnyDetector.ERROR_MESSAGE_FORMAT.formatted("m_sm"),
        error.getMessage(null));
  }

  @Test
  void methodParameterUsage() {
    String source =
        """
        package wpilib.robot;

        import org.wpilib.command3.StateMachine;

        class Example {
          void configure(StateMachine sm) {
            var state1 = sm.addState(new Object());
            sm.switchFromAny().toExitStateMachine().when(() -> true);
            var state2 = sm.addState(new Object());
          }
        }
        """;

    Compilation compilation =
        javac()
            .withOptions(JAVA_VERSION_OPTIONS)
            .compile(
                JavaFileObjects.forSourceString(
                    "org.wpilib.command3.StateMachine", STATE_MACHINE_SOURCE),
                JavaFileObjects.forSourceString("wpilib.robot.Example", source));

    assertThat(compilation).failed();
    assertEquals(1, compilation.errors().size());
    var error = compilation.errors().get(0);
    assertEquals(
        AddStateAfterSwitchFromAnyDetector.ERROR_MESSAGE_FORMAT.formatted("sm"),
        error.getMessage(null));
  }

  @Test
  void independentMethodsDoNotLeakState() {
    String source =
        """
        package wpilib.robot;

        import org.wpilib.command3.StateMachine;

        class Example {
          StateMachine m_sm = new StateMachine("SM");

          void setup1() {
            var state1 = m_sm.addState(new Object());
            m_sm.switchFromAny().toExitStateMachine().when(() -> true);
          }

          void setup2() {
            var state2 = m_sm.addState(new Object());
          }
        }
        """;

    Compilation compilation =
        javac()
            .withOptions(JAVA_VERSION_OPTIONS)
            .compile(
                JavaFileObjects.forSourceString(
                    "org.wpilib.command3.StateMachine", STATE_MACHINE_SOURCE),
                JavaFileObjects.forSourceString("wpilib.robot.Example", source));

    assertThat(compilation).succeededWithoutWarnings();
  }

  @Test
  void emptyArrayArgCountsAsNoArg() {
    String source =
        """
        package wpilib.robot;

        import org.wpilib.command3.StateMachine;
        import org.wpilib.command3.StateMachine.State;

        class Example {
          void setup() {
            StateMachine sm = new StateMachine("SM");
            var state1 = sm.addState(new Object());
            sm.switchFromAny(new State[0]).toExitStateMachine().when(() -> true);
            var state2 = sm.addState(new Object());
          }
        }
        """;

    Compilation compilation =
        javac()
            .withOptions(JAVA_VERSION_OPTIONS)
            .compile(
                JavaFileObjects.forSourceString(
                    "org.wpilib.command3.StateMachine", STATE_MACHINE_SOURCE),
                JavaFileObjects.forSourceString("wpilib.robot.Example", source));

    assertThat(compilation).failed();
    assertEquals(1, compilation.errors().size());
    var error = compilation.errors().get(0);
    assertEquals(
        AddStateAfterSwitchFromAnyDetector.ERROR_MESSAGE_FORMAT.formatted("sm"),
        error.getMessage(null));
  }
}
