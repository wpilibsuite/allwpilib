package org.wpilib.javacplugin;

import com.sun.source.tree.CompilationUnitTree;
import com.sun.source.tree.IdentifierTree;
import com.sun.source.tree.LambdaExpressionTree;
import com.sun.source.tree.MemberSelectTree;
import com.sun.source.tree.MethodInvocationTree;
import com.sun.source.tree.MethodTree;
import com.sun.source.tree.WhileLoopTree;
import com.sun.source.util.JavacTask;
import com.sun.source.util.TaskEvent;
import com.sun.source.util.TaskListener;
import com.sun.source.util.TreeScanner;
import com.sun.source.util.Trees;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Set;
import javax.lang.model.element.Element;
import javax.lang.model.element.ExecutableElement;
import javax.lang.model.element.VariableElement;
import javax.lang.model.type.TypeMirror;
import javax.tools.Diagnostic;

/**
 * Checks for {@code while} loops inside methods or lambda functions that accept coroutine
 * arguments. If a loop does not call {@code yield()} on one of the most local coroutine objects, a
 * compiler error will be emitted for that loop element. This check cannot be silenced.
 */
// Note: cannot be silenced because annotations cannot be placed on loops.
// This is not legal Java:
//   @SuppressWarnings("UnsafeCoroutineUsage")
//   while (true) { ... }
// Placing it at a higher level (lambda or method declaration) would silence ALL unsafe usage in
// that expression; it's impossible to do on a case-by-case basis.
public class CoroutineYieldInLoopDetector implements TaskListener {
  private final JavacTask m_task;
  private final Set<CompilationUnitTree> m_visitedCUs = new HashSet<>();

  public CoroutineYieldInLoopDetector(JavacTask task) {
    m_task = task;
  }

  @Override
  public void finished(TaskEvent e) {
    if (e.getKind() == TaskEvent.Kind.ANALYZE && m_visitedCUs.add(e.getCompilationUnit())) {
      if (getCoroutineType() == null) {
        // Not using the commands library; nothing to scan for
        return;
      }

      e.getCompilationUnit().accept(new Scanner(e.getCompilationUnit()), null);
    }
  }

  private TypeMirror getCoroutineType() {
    var te = m_task.getElements().getTypeElement("org.wpilib.commands3.Coroutine");
    return te != null ? te.asType() : null;
  }

  /**
   * Tracks the state of a while loop while traversing the AST. These are initially created when
   * encountering a method or lambda function declaration, after checking that the method or lambda
   * accepts at least one Coroutine argument. All Coroutine arguments on the method or lambda
   * function will be added to m_availableCoroutines. If a nested lambda function is found that
   * accepts coroutines, then a new state will be created for that lambda and its coroutine
   * arguments will be the ones that need to be yielded on.
   *
   * <p>If a `while` loop is not encountered while further traversing the tree, then the initial
   * object will not be modified and will be discarded, unused. But if a `while` loop _is_
   * encountered, then m_loop will be assigned to that loop element and further traversal will
   * occur. Any calls to `yield()` on one of the coroutine arguments declared by the enclosing
   * method or lambda function will be detected and added to m_yieldCalls. Any `while` loops
   * encountered while m_loop is set are child loops, and will be parsed standalone and given new
   * LoopState objects, which will then be added to m_children. Error reporting is only done by the
   * root state object once its entire AST has been traversed, to ensure that inner loops do not
   * report errors first and appearing out of order in the compiler output.
   *
   * <p>Note: this is a mutable type so that a single object may be updated as the tree traversal
   * reaches points of interest (lambda definition, loop declarations, and so on) and have its state
   * checked at a higher level when inner AST traversal is finished.
   */
  private static final class LoopState {
    /** The loop element being tracked. */
    WhileLoopTree m_loop;

    /**
     * All discovered calls to Coroutine.yield(). Only applies to calls to coroutines in
     * m_availableCoroutines.
     */
    final List<Element> m_yieldCalls = new ArrayList<>();

    /**
     * The set of the most local coroutine arguments. eg for nested lambdas, the coroutine arguments
     * to the innermost lambda will be present, but none of the coroutine arguments from any of the
     * enclosing lambdas.
     */
    final List<VariableElement> m_availableCoroutines = new ArrayList<>();

    /**
     * All `while` loops nested inside m_loop. Only applies to direct children; loops in
     * conditionals, switch blocks, and the like will be present, but not loops in other nested
     * loops, nor loops declared inside a lambda inside a loop.
     */
    final List<LoopState> m_children = new ArrayList<>();
  }

  private final class Scanner extends TreeScanner<LoopState, LoopState> {
    private final CompilationUnitTree m_root;
    private final Trees m_trees;
    private final TypeMirror m_coroutineType;

    Scanner(CompilationUnitTree compilationUnit) {
      m_root = compilationUnit;
      m_trees = Trees.instance(m_task);
      m_coroutineType = getCoroutineType();
    }

    @Override
    public LoopState visitMethod(MethodTree node, LoopState loopState) {
      var path = m_trees.getPath(m_root, node);
      var el = m_trees.getElement(path);
      if (!(el instanceof ExecutableElement ex)) {
        return super.visitMethod(node, loopState);
      }

      // Scan parameters to see if any are coroutines
      List<? extends VariableElement> coroutineParameters =
          ex.getParameters().stream()
              .filter(
                  parameter -> m_task.getTypes().isSameType(parameter.asType(), m_coroutineType))
              .toList();

      if (coroutineParameters.isEmpty()) {
        // This method does not take coroutine arguments
        return super.visitMethod(node, loopState);
      }

      // Note: We only want to allow yield() calls to the MOST local coroutines possible, so nested
      //       methods or lambdas don't accidentally call yield on the coroutine of an outer command
      //       (which would be a RuntimeError, since that coroutine would not be mounted).
      //       Therefore, we do not copy the available coroutines from the parent state to the new
      //       child state.
      var localState = new LoopState();
      localState.m_availableCoroutines.addAll(coroutineParameters);
      if (loopState != null) {
        loopState.m_children.add(localState);
      }

      return super.visitMethod(node, localState);
    }

    @Override
    public LoopState visitLambdaExpression(LambdaExpressionTree node, LoopState loopState) {
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
        return super.visitLambdaExpression(node, loopState);
      }

      // Note: we only want to allow yield() calls to the MOST local coroutines possible, so nested
      //       methods or lambdas don't accidentally call yield on the coroutine of an outer command
      //       (which would be a RuntimeError, since that coroutine would not be mounted).
      //       Therefore, we do not copy the available coroutines from the parent state to the new
      //       child state.
      var localState = new LoopState();
      localState.m_availableCoroutines.addAll(coroutineParameters);
      if (loopState != null) {
        loopState.m_children.add(localState);
      }

      return super.visitLambdaExpression(node, localState);
    }

    @Override
    public LoopState visitWhileLoop(WhileLoopTree node, LoopState loopState) {
      if (loopState == null) {
        // Not inside a coroutine-accepting method or lambda function; bail
        return super.visitWhileLoop(node, null);
      }

      if (loopState.m_loop == null) {
        loopState.m_loop = node;
        var result = super.visitWhileLoop(node, loopState);
        printErrors(loopState);
        return result;
      } else {
        // Nested loop; split off a new child with the same available coroutines
        var localState = new LoopState();
        localState.m_loop = node;
        localState.m_availableCoroutines.addAll(loopState.m_availableCoroutines);
        loopState.m_children.add(localState);

        // Don't print errors now - we'll handle that when we finish the parent loop
        // Otherwise, errors would be printed by the innermost loops first and appear out of order,
        // which is confusing
        return super.visitWhileLoop(node, localState);
      }
    }

    private void printErrors(LoopState state) {
      if (state == null) {
        return;
      }
      if (state.m_availableCoroutines.isEmpty() || state.m_loop == null) {
        // Technically, this state should never be reachable.
        // But to prevent errors in the compiler plugin from preventing user code from building,
        // we silently ignore this bad state.
        return;
      }

      if (state.m_yieldCalls.isEmpty()) {
        m_trees.printMessage(
            Diagnostic.Kind.ERROR, buildErrorMessageForLoop(state), state.m_loop, m_root);
      }

      // Recurse over children
      for (var childLoop : state.m_children) {
        printErrors(childLoop);
      }
    }

    private CharSequence buildErrorMessageForLoop(LoopState state) {
      StringBuilder messageBuilder = new StringBuilder(64).append("Missing call to ");
      var suggestedFunctionCalls =
          state.m_availableCoroutines.stream()
              .map(e -> "`" + e.getSimpleName().toString() + ".yield()`")
              .toList();

      // Join function calls with commas, and inserting an "or" before the final item
      // No commas are used for lists of 2 items or fewer
      switch (suggestedFunctionCalls.size()) {
        case 1 -> messageBuilder.append(suggestedFunctionCalls.get(0));
        case 2 ->
            messageBuilder
                .append(suggestedFunctionCalls.get(0))
                .append(" or ")
                .append(suggestedFunctionCalls.get(1));
        default -> {
          for (int i = 0; i < suggestedFunctionCalls.size(); i++) {
            messageBuilder.append(suggestedFunctionCalls.get(i));
            if (i < suggestedFunctionCalls.size() - 2) {
              // early items
              messageBuilder.append(", ");
            } else if (i == suggestedFunctionCalls.size() - 2) {
              // penultimate item
              messageBuilder.append(", or ");
            } else {
              // last item - append nothing
            }
          }
        }
      }

      messageBuilder.append(" inside loop");

      return messageBuilder;
    }

    @Override
    public LoopState visitMethodInvocation(MethodInvocationTree node, LoopState loopState) {
      if (loopState == null) {
        // not inside a coroutine-accepting method or lambda function; bail
        return super.visitMethodInvocation(node, null);
      }

      var sel = node.getMethodSelect();
      if (sel instanceof MemberSelectTree ms && ms.getIdentifier().contentEquals("yield")) {
        // calling yield() on something (may not be a coroutine object)
        var recv = ms.getExpression();
        if (recv instanceof IdentifierTree id) {
          var idPath = m_trees.getPath(m_root, id);
          var el = m_trees.getElement(idPath);
          if (el instanceof VariableElement ve && loopState.m_availableCoroutines.contains(ve)) {
            // definitely calling yield on a local (method or lambda argument) coroutine object
            loopState.m_yieldCalls.add(el);
          }
        }
      }

      return super.visitMethodInvocation(node, loopState);
    }
  }
}
