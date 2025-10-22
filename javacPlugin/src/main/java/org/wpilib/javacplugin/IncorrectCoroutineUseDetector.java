// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.javacplugin;

import com.sun.source.tree.AssignmentTree;
import com.sun.source.tree.CompilationUnitTree;
import com.sun.source.tree.IdentifierTree;
import com.sun.source.tree.LambdaExpressionTree;
import com.sun.source.tree.MemberSelectTree;
import com.sun.source.tree.MethodInvocationTree;
import com.sun.source.util.JavacTask;
import com.sun.source.util.TreeScanner;
import com.sun.source.util.Trees;
import java.util.ArrayList;
import java.util.List;
import javax.lang.model.element.TypeElement;
import javax.lang.model.element.VariableElement;
import javax.tools.Diagnostic;

/**
 * Detects usage of an incorrect coroutine object. For example, nesting commands with multiple
 * coroutines in scope need to use the most local coroutine object and not reference external ones.
 *
 * <pre>{@code
 * mech.run(outerCoroutine -> {
 *   mech.run(innerCoroutine -> {
 *     outerCoroutine.yield(); // ERROR
 *   })
 * })
 * }</pre>
 *
 * <p>Additional checks for escaping coroutines are also performed:
 *
 * <pre>{@code
 * private Coroutine coroutineField;
 *
 * mech.run(coroutine -> coroutineField = coroutine); // ERROR
 * }</pre>
 */
public class IncorrectCoroutineUseDetector extends CoroutineBasedDetector {
  public IncorrectCoroutineUseDetector(JavacTask task) {
    super(task);
  }

  @Override
  protected TreeScanner<?, ?> createScanner(CompilationUnitTree compilationUnit) {
    return new Scanner(compilationUnit);
  }

  private static final class State {
    /** Tracks captured coroutine parameters from external lambda expressions. */
    private final List<? extends VariableElement> m_nonlocalCoroutineParameters;

    /**
     * Tracks coroutine parameters from the most local lambda expression. Captures from outer lambda
     * expressions will be in {@link #m_nonlocalCoroutineParameters}.
     */
    private final List<? extends VariableElement> m_localCoroutineParameters;

    private State(
        List<? extends VariableElement> nonlocalCoroutineParameters,
        List<? extends VariableElement> coroutineParameters) {
      m_nonlocalCoroutineParameters = List.copyOf(nonlocalCoroutineParameters);
      m_localCoroutineParameters = List.copyOf(coroutineParameters);
    }

    boolean isLocalCoroutine(VariableElement el) {
      return m_localCoroutineParameters.contains(el);
    }

    boolean isCapturedCoroutine(VariableElement el) {
      return m_nonlocalCoroutineParameters.contains(el) || m_localCoroutineParameters.contains(el);
    }
  }

  private final class Scanner extends TreeScanner<State, State> {
    private final CompilationUnitTree m_root;
    private final Trees m_trees;

    Scanner(CompilationUnitTree compilationUnit) {
      m_root = compilationUnit;
      m_trees = Trees.instance(m_task);
    }

    @Override
    public State visitLambdaExpression(LambdaExpressionTree node, State state) {
      // Scan parameters to see if any are coroutines
      List<? extends VariableElement> coroutineParameters =
          node.getParameters().stream()
              .filter(
                  parameter -> {
                    var paramPath = m_trees.getPath(m_root, parameter.getType());
                    var paramTypeMirror = m_trees.getTypeMirror(paramPath);
                    return m_task.getTypes().isSameType(paramTypeMirror, m_coroutineType);
                  })
              .map(
                  variableTree -> {
                    return (VariableElement)
                        m_trees.getElement(m_trees.getPath(m_root, variableTree));
                  })
              .toList();

      if (coroutineParameters.isEmpty()) {
        // This lambda function does not take coroutine arguments
        return super.visitLambdaExpression(node, state);
      }

      State localState;
      if (state == null) {
        localState = new State(List.of(), coroutineParameters);
      } else {
        localState =
            new State(
                concat(state.m_nonlocalCoroutineParameters, state.m_localCoroutineParameters),
                coroutineParameters);
      }

      return super.visitLambdaExpression(node, localState);
    }

    @Override
    public State visitMethodInvocation(MethodInvocationTree node, State state) {
      if (state == null || state.m_localCoroutineParameters.isEmpty()) {
        // No coroutines in scope, do nothing
        return super.visitMethodInvocation(node, state);
      }

      // Check for calling a method on a nonlocal coroutine (eg `outerCoroutine.yield()`)
      var sel = node.getMethodSelect();
      if (sel instanceof MemberSelectTree ms && ms.getExpression() instanceof IdentifierTree id) {
        var idPath = m_trees.getPath(m_root, id);
        var el = m_trees.getElement(idPath);
        if (el instanceof VariableElement ve
            && ve.asType().equals(m_coroutineType)
            && !state.isLocalCoroutine(ve)) {
          m_trees.printMessage(
              Diagnostic.Kind.ERROR, nonlocalCoroutineUsageMessage(ve, state), node, m_root);
        }
      }

      // Check for passing a nonlocal coroutine to a method (eg `foo(outerCoroutine)`)
      for (var arg : node.getArguments()) {
        var argPath = m_trees.getPath(m_root, arg);
        var el = m_trees.getElement(argPath);
        if (el instanceof VariableElement ve
            && ve.asType().equals(m_coroutineType)
            && !state.isLocalCoroutine(ve)) {
          m_trees.printMessage(
              Diagnostic.Kind.ERROR, nonlocalCoroutineUsageMessage(ve, state), node, m_root);
        }
      }

      return super.visitMethodInvocation(node, state);
    }

    private String nonlocalCoroutineUsageMessage(VariableElement ve, State state) {
      StringBuilder optionsBuilder = new StringBuilder();
      if (state.m_localCoroutineParameters.size() == 1) {
        optionsBuilder
            .append('`')
            .append(state.m_localCoroutineParameters.get(0).getSimpleName())
            .append('`');
      } else if (state.m_localCoroutineParameters.size() == 2) {
        optionsBuilder
            .append('`')
            .append(state.m_localCoroutineParameters.get(0).getSimpleName())
            .append("` or `")
            .append(state.m_localCoroutineParameters.get(1).getSimpleName())
            .append('`');
      } else {
        for (int i = 0; i < state.m_localCoroutineParameters.size(); i++) {
          var coroutineParameter = state.m_localCoroutineParameters.get(i);
          optionsBuilder.append('`').append(coroutineParameter.getSimpleName()).append('`');
          if (i < state.m_localCoroutineParameters.size() - 2) {
            optionsBuilder.append(", ");
          } else if (i == state.m_localCoroutineParameters.size() - 2) {
            optionsBuilder.append(", or ");
          }
        }
      }

      return "Coroutine `%s` may not be in scope. Consider using %s"
          .formatted(ve.getSimpleName(), optionsBuilder);
    }

    @Override
    public State visitAssignment(AssignmentTree node, State state) {
      if (state == null || state.m_localCoroutineParameters.isEmpty()) {
        return super.visitAssignment(node, state);
      }

      // Check if the assignment is to a field, and if the expression is a local or nonlocal
      // coroutine.
      var variable = node.getVariable();
      var variablePath = m_trees.getPath(m_root, variable);
      var variableElement = m_trees.getElement(variablePath);
      if (variableElement instanceof VariableElement ve
          && ve.getEnclosingElement() instanceof TypeElement
          && ve.asType().equals(m_coroutineType)) {
        // Assigning to a coroutine-typed field. Check the expression to see if we're assigning
        // a captured coroutine.
        var expression = node.getExpression();
        var expressionPath = m_trees.getPath(m_root, expression);
        var expressionElement = m_trees.getElement(expressionPath);
        if (expressionElement instanceof VariableElement ve2 && state.isCapturedCoroutine(ve2)) {
          m_trees.printMessage(
              Diagnostic.Kind.ERROR,
              "Captured coroutines may not be stored in fields",
              node,
              m_root);
        }
      }

      return super.visitAssignment(node, state);
    }

    // Concatenates two lists and returns the result.
    // Why is this not in the standard library?
    private static <E> List<E> concat(List<? extends E> a, List<? extends E> b) {
      var result = new ArrayList<E>(a.size() + b.size());
      result.addAll(a);
      result.addAll(b);
      return result;
    }
  }
}
