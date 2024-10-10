package edu.wpi.first.epilogue.processor;

import static com.google.testing.compile.CompilationSubject.assertThat;
import static com.google.testing.compile.Compiler.javac;
import static org.junit.jupiter.api.Assertions.assertEquals;

import com.google.testing.compile.Compilation;
import com.google.testing.compile.JavaFileObjects;
import java.io.IOException;
import org.junit.jupiter.api.Test;

@SuppressWarnings("PMD.ConsecutiveLiteralAppends")
class LoggableHandlerTest {

  @Test
  void noSubtypes() {
    String source =
        """
                package edu.wpi.first.epilogue;

                @Logged
                class Example {}
                """;

    String expected =
        """
                if (Epilogue.shouldLog(Logged.Importance.DEBUG)) {
                  Epilogue.exampleLogger.tryUpdate(dataLogger.getSubLogger("example"), object, Epilogue.getConfig().errorHandler);
                }
                """;

    assertLogInvocation(source, expected);
  }

  @Test
  void withSubtypes() {
    String source =
        """
                package edu.wpi.first.epilogue;

                @Logged
                class Parent {}

                @Logged
                class Child extends Parent {}

                @Logged
                class Example {
                  Parent exampleField;
                }
                """;

    String expected =
        """
                if (Epilogue.shouldLog(Logged.Importance.DEBUG)) {
                  var obj = object.exampleField;
                  var logger = dataLogger.getSubLogger("exampleField");
                  if (obj instanceof edu.wpi.first.epilogue.Child) {
                    Epilogue.childLogger.tryUpdate(logger, (edu.wpi.first.epilogue.Child) obj, Epilogue.getConfig().errorHandler);
                  } else {
                    Epilogue.parentLogger.tryUpdate(logger, obj, Epilogue.getConfig().errorHandler);
                  }
                }
                """;

    // Additional debug information
    System.out.println("Expected Log Invocation:\n" + expected);

    assertLogInvocation(source, expected);
  }

  private void assertLogInvocation(String classContent, String expectedLogInvocation) {
    Compilation compilation =
        javac()
            .withProcessors(new AnnotationProcessor())
            .compile(
                JavaFileObjects.forSourceString("edu.wpi.first.epilogue.Example", classContent));

    assertThat(compilation).succeededWithoutWarnings();

    var generatedFile =
        compilation.generatedSourceFiles().stream()
            .filter(jfo -> jfo.getName().contains("ExampleLogger"))
            .findFirst()
            .orElseThrow(() -> new IllegalStateException("ExampleLogger file was not generated!"));

    try {
      var content = generatedFile.getCharContent(false).toString();
      System.out.println("Full generated content:\n" + content);

      String generatedLogInvocation = extractLogInvocation(content);
      assertEquals(
          expectedLogInvocation.strip().replace("\r\n", "\n"),
          generatedLogInvocation.strip().replace("\r\n", "\n"));
    } catch (IOException e) {
      throw new RuntimeException(e);
    }
  }

  private String extractLogInvocation(String loggerClassContent) {
    System.out.println("Generated Logger Class Content:\n" + loggerClassContent);

    // Find the start index of the main 'if' condition
    int startIndex = loggerClassContent.indexOf("if (Epilogue.shouldLog(Logged.Importance.DEBUG))");
    if (startIndex == -1) {
      throw new IllegalStateException(
          "Failed to locate log invocation in the generated logger class.");
    }

    // Initialize brace tracking from the start of the block
    int braceCount = 0;
    int endIndex = startIndex;

    // Traverse the content starting from the found 'if' block
    boolean startedTracking = false;

    while (endIndex < loggerClassContent.length()) {
      char currentChar = loggerClassContent.charAt(endIndex);

      if (currentChar == '{') {
        braceCount++;
        startedTracking = true; // Start tracking when the first '{' is encountered
      } else if (currentChar == '}') {
        braceCount--;
        // Stop when the block fully closes
        if (braceCount == 0 && startedTracking) {
          endIndex++; // Move one step past the closing brace
          break;
        }
      }
      endIndex++;
    }

    if (braceCount != 0) {
      throw new IllegalStateException("Failed to identify the complete log invocation block.");
    }

    // Extract the substring containing the complete log invocation block
    return loggerClassContent.substring(startIndex, endIndex).strip();
  }
}
