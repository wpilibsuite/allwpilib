// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.javacplugin;

import static com.google.testing.compile.CompilationSubject.assertThat;
import static com.google.testing.compile.Compiler.javac;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.wpilib.javacplugin.CompileTestOptions.kJavaVersionOptions;

import com.google.testing.compile.Compilation;
import com.google.testing.compile.JavaFileObjects;
import org.junit.jupiter.api.Test;

class ReturnValueUsedListenerTest {
  @Test
  void nodiscardReturnValueIsUsed() {
    String source =
        """
        package frc.robot;

        import org.wpilib.annotation.NoDiscard;

        class Example {
          @NoDiscard
          int getI() { return 0; }

          void usage() {
            int i = getI();
          }
        }
      """;

    Compilation compilation =
        javac()
            .withOptions(kJavaVersionOptions)
            .compile(JavaFileObjects.forSourceString("frc.robot.Example", source));

    assertThat(compilation).succeededWithoutWarnings();
  }

  @Test
  void nodiscardReturnValueUnused() {
    String source =
        """
        package frc.robot;

        import org.wpilib.annotation.NoDiscard;

        class Example {
          @NoDiscard
          int getI() { return 0; }

          void usage() {
            getI();
          }
        }
      """;

    Compilation compilation =
        javac()
            .withOptions(kJavaVersionOptions)
            .compile(JavaFileObjects.forSourceString("frc.robot.Example", source));

    assertThat(compilation).failed();
    assertEquals(1, compilation.errors().size());
    var error = compilation.errors().get(0);
    assertEquals("Result of @NoDiscard method is ignored", error.getMessage(null));
  }

  @Test
  void nodiscardCustomMessage() {
    String source =
        """
        package frc.robot;

        import org.wpilib.annotation.NoDiscard;

        class Example {
          @NoDiscard("Custom message")
          int getI() { return 0; }

          void usage() {
            getI();
          }
        }
      """;

    Compilation compilation =
        javac()
            .withOptions(kJavaVersionOptions)
            .compile(JavaFileObjects.forSourceString("frc.robot.Example", source));

    assertThat(compilation).failed();
    assertEquals(1, compilation.errors().size());
    var error = compilation.errors().get(0);
    assertEquals("Result of @NoDiscard method is ignored: Custom message", error.getMessage(null));
  }

  @Test
  void nodiscardMessageEmptyString() {
    String source =
        """
        package frc.robot;

        import org.wpilib.annotation.NoDiscard;

        class Example {
          @NoDiscard("")
          int getI() { return 0; }

          void usage() {
            getI();
          }
        }
      """;

    Compilation compilation =
        javac()
            .withOptions(kJavaVersionOptions)
            .compile(JavaFileObjects.forSourceString("frc.robot.Example", source));

    assertThat(compilation).failed();
    assertEquals(1, compilation.errors().size());
    var error = compilation.errors().get(0);
    assertEquals("Result of @NoDiscard method is ignored", error.getMessage(null));
  }

  @Test
  void nodiscardOnVoidMethod() {

    String source =
        """
        package frc.robot;

        import org.wpilib.annotation.NoDiscard;

        class Example {
          @NoDiscard
          void voidMethod() { }

          void usage() {
            voidMethod();
          }
        }
      """;

    Compilation compilation =
        javac()
            .withOptions(kJavaVersionOptions)
            .compile(JavaFileObjects.forSourceString("frc.robot.Example", source));

    assertThat(compilation).succeededWithoutWarnings();
  }

  @Test
  void commandsv2CommandFactoryResultIsAssigned() {
    String source =
        """
        package frc.robot;

        import edu.wpi.first.wpilibj2.command.Command;
        import edu.wpi.first.wpilibj2.command.Commands;
        import org.wpilib.annotation.NoDiscard;

        class Example {
          Command getCommand() {
            return Commands.print("");
          }

          void usage() {
            Command theCommand = getCommand();
          }
        }
      """;

    Compilation compilation =
        javac()
            .withOptions(kJavaVersionOptions)
            .compile(JavaFileObjects.forSourceString("frc.robot.Example", source));

    assertThat(compilation).succeededWithoutWarnings();
  }

  @Test
  void commandsv2CommandFactoryResultIsPassed() {
    String source =
        """
        package frc.robot;

        import edu.wpi.first.wpilibj2.command.Command;
        import edu.wpi.first.wpilibj2.command.Commands;
        import org.wpilib.annotation.NoDiscard;

        class Example {
          Command getCommand() {
            return Commands.print("");
          }

          void usage() {
            System.out.println(getCommand());
          }
        }
      """;

    Compilation compilation =
        javac()
            .withOptions(kJavaVersionOptions)
            .compile(JavaFileObjects.forSourceString("frc.robot.Example", source));

    assertThat(compilation).succeededWithoutWarnings();
  }

  @Test
  void commandsv2CommandFactoryResultIsChainedAndUsed() {
    String source =
        """
        package frc.robot;

        import edu.wpi.first.wpilibj2.command.Command;
        import edu.wpi.first.wpilibj2.command.Commands;
        import org.wpilib.annotation.NoDiscard;

        class Example {
          Command getCommand() {
            return Commands.print("");
          }

          void usage() {
            Command theCommand = getCommand().withName("The name");
          }
        }
      """;

    Compilation compilation =
        javac()
            .withOptions(kJavaVersionOptions)
            .compile(JavaFileObjects.forSourceString("frc.robot.Example", source));

    assertThat(compilation).succeededWithoutWarnings();
  }

  @Test
  void commandsv2CommandFactoryResultNotUsed() {
    String source =
        """
        package frc.robot;

        import edu.wpi.first.wpilibj2.command.Command;
        import edu.wpi.first.wpilibj2.command.Commands;
        import org.wpilib.annotation.NoDiscard;

        class Example {
          Command getCommand() {
            return Commands.print("");
          }

          void usage() {
            getCommand();
          }
        }
      """;

    Compilation compilation =
        javac()
            .withOptions(kJavaVersionOptions)
            .compile(JavaFileObjects.forSourceString("frc.robot.Example", source));

    assertThat(compilation).failed();
    assertEquals(1, compilation.errors().size());
    var error = compilation.errors().get(0);
    assertEquals("Return value of Command-returning method is ignored", error.getMessage(null));
  }

  @Test
  void commandsv2CommandFactoryResultIsChainedAndNotUsed() {
    String source =
        """
        package frc.robot;

        import edu.wpi.first.wpilibj2.command.Command;
        import edu.wpi.first.wpilibj2.command.Commands;
        import org.wpilib.annotation.NoDiscard;

        class Example {
          Command getCommand() {
            return Commands.print("");
          }

          void usage() {
            getCommand().withName("The name");
          }
        }
      """;

    Compilation compilation =
        javac()
            .withOptions(kJavaVersionOptions)
            .compile(JavaFileObjects.forSourceString("frc.robot.Example", source));

    assertThat(compilation).failed();
    assertEquals(1, compilation.errors().size());
    var error = compilation.errors().get(0);
    assertEquals("Return value of Command-returning method is ignored", error.getMessage(null));
  }

  @Test
  void commandsv2NewCommandInstanceNotUsed() {
    String source =
        """
        package frc.robot;

        import edu.wpi.first.wpilibj2.command.Command;
        import edu.wpi.first.wpilibj2.command.Commands;
        import edu.wpi.first.wpilibj2.command.WaitCommand;
        import org.wpilib.annotation.NoDiscard;

        class Example {
          void usage() {
            new WaitCommand(1);
          }
        }
      """;

    Compilation compilation =
        javac()
            .withOptions(kJavaVersionOptions)
            .compile(JavaFileObjects.forSourceString("frc.robot.Example", source));

    assertThat(compilation).failed();
    assertEquals(1, compilation.errors().size());
    var error = compilation.errors().get(0);
    assertEquals("Return value of Command-returning method is ignored", error.getMessage(null));
  }
}
