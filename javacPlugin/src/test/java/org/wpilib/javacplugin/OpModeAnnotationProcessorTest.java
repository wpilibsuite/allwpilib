package org.wpilib.javacplugin;

import static com.google.testing.compile.CompilationSubject.assertThat;
import static com.google.testing.compile.Compiler.javac;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.wpilib.javacplugin.CompileTestUtils.kJavaVersionOptions;

import com.google.common.collect.Sets;
import com.google.testing.compile.Compilation;
import com.google.testing.compile.JavaFileObjects;
import java.util.List;
import java.util.Set;
import java.util.stream.Stream;
import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.Arguments;
import org.junit.jupiter.params.provider.MethodSource;

class OpModeAnnotationProcessorTest {
  private enum ClassType {
    CONCRETE("class"),
    ABSTRACT("abstract class"),
    ENUM("enum"),
    INTERFACE("interface"),
    ;
    private final String declaration;

    ClassType(String declaration) {
      this.declaration = declaration;
    }
  }

  @ParameterizedTest(name = "@{0} [inherit={1}, classType={2}]")
  @MethodSource("configurations")
  void test(String annotationName, boolean inherit, ClassType classType) {
    String source = generateOpModeClassSource(annotationName, inherit, classType);

    Compilation compilation =
        javac()
            .withOptions(kJavaVersionOptions)
            .withProcessors(new OpModeAnnotationProcessor())
            .compile(JavaFileObjects.forSourceString("example.Example", source));

    final String nonAbstractErrorMessage =
        "The @" + annotationName + " OpMode annotation can only be used on non-abstract classes";
    final String implementationErrorMessage =
        "The @"
            + annotationName
            + " OpMode annotation can only be used on classes that implement the "
            + "org.wpilib.opmode.OpMode interface or extend a class that does";

    if (!inherit) {
      assertThat(compilation).failed();
      var errors = compilation.errors();
      if (classType != ClassType.CONCRETE) {
        assertEquals(2, errors.size());
        var error1 = errors.getFirst();
        var error2 = errors.getLast();
        assertEquals(nonAbstractErrorMessage, error1.getMessage(null));
        assertEquals(implementationErrorMessage, error2.getMessage(null));
      } else {
        assertEquals(1, errors.size());
        var error1 = errors.getFirst();
        assertEquals(implementationErrorMessage, error1.getMessage(null));
      }
    } else if (classType != ClassType.CONCRETE) {
      assertThat(compilation).failed();
      var errors = compilation.errors();
      assertEquals(1, errors.size());
      var error1 = errors.getFirst();
      assertEquals(nonAbstractErrorMessage, error1.getMessage(null));
    } else {
      // Both inheriting and a concrete class; should be valid
      assertThat(compilation).succeededWithoutWarnings();
    }
  }

  private static String generateOpModeClassSource(
      String annotationName, boolean inherit, ClassType classType) {
    String sourceTemplate =
        """
        package example;

        import org.wpilib.opmode.OpMode;
        import org.wpilib.opmode.Autonomous;
        import org.wpilib.opmode.Teleop;
        import org.wpilib.opmode.Utility;

        @%s
        public %s Example %s {
        }
        """;

    String extensionDeclaration =
        classType == ClassType.INTERFACE ? "extends OpMode" : "implements OpMode";

    return sourceTemplate.formatted(
        annotationName, classType.declaration, inherit ? extensionDeclaration : "");
  }

  private static Stream<Arguments> configurations() {
    Set<Object> annotations = Set.of("Autonomous", "Teleop", "Utility");
    Set<Object> inheritOptions = Set.of(true, false);
    Set<ClassType> classTypes = Set.of(ClassType.values());

    Set<List<Object>> allCombinations =
        Sets.cartesianProduct(annotations, inheritOptions, classTypes);

    return allCombinations.stream().map(args -> Arguments.arguments(args.toArray()));
  }
}
