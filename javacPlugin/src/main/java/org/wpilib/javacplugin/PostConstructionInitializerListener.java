// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.javacplugin;

import com.sun.source.tree.AssignmentTree;
import com.sun.source.tree.BlockTree;
import com.sun.source.tree.CompilationUnitTree;
import com.sun.source.tree.MemberSelectTree;
import com.sun.source.tree.MethodInvocationTree;
import com.sun.source.tree.NewClassTree;
import com.sun.source.util.JavacTask;
import com.sun.source.util.TaskEvent;
import com.sun.source.util.TaskListener;
import com.sun.source.util.TreePath;
import com.sun.source.util.TreeScanner;
import com.sun.source.util.Trees;
import java.util.HashMap;
import java.util.HashSet;
import java.util.LinkedHashSet;
import java.util.List;
import java.util.Map;
import java.util.SequencedSet;
import java.util.Set;
import java.util.stream.Collectors;
import javax.lang.model.element.AnnotationMirror;
import javax.lang.model.element.AnnotationValue;
import javax.lang.model.element.Element;
import javax.lang.model.element.ExecutableElement;
import javax.lang.model.element.Modifier;
import javax.lang.model.element.TypeElement;
import javax.lang.model.element.VariableElement;
import javax.lang.model.type.DeclaredType;
import javax.lang.model.type.TypeKind;
import javax.tools.Diagnostic;
import org.wpilib.annotation.PostConstructionInitializer;

/**
 * Ensures methods tagged with {@link org.wpilib.annotation.PostConstructionInitializer} are called
 * after the owning object is constructed.
 */
public class PostConstructionInitializerListener implements TaskListener {
  private final JavacTask m_task;
  private final Set<CompilationUnitTree> m_visitedCUs = new HashSet<>();

  public PostConstructionInitializerListener(JavacTask task) {
    m_task = task;
  }

  @Override
  public void finished(TaskEvent e) {
    // We override `finished` instead of `started` because we want to run after the
    // ANALYZE attribution phase has completed and assigned types to elements in the AST
    // Track the visited CUs to avoid re-processing the same CU multiple times when we call
    // `Trees.getElement()` on a tree path.
    var compilationUnit = e.getCompilationUnit();
    if (e.getKind() == TaskEvent.Kind.ANALYZE && m_visitedCUs.add(compilationUnit)) {
      var state = new State();
      compilationUnit.accept(new Scanner(compilationUnit), state);

      if (state.m_initializedObjects.isEmpty()) {
        // Good! No partially initialized objects were detected.
        return;
      }

      var trees = Trees.instance(m_task);

      for (InitializedObject partiallyInitializedObject : state.m_initializedObjects.values()) {
        var object = partiallyInitializedObject.object();
        var uncalledInitializers = partiallyInitializedObject.initializers();
        trees.printMessage(
            Diagnostic.Kind.ERROR,
            "Partially-initialized object `%s` is missing %s %s"
                .formatted(
                    object.getSimpleName(),
                    uncalledInitializers.size() == 1
                        ? "a call to initializer method"
                        : "calls to " + uncalledInitializers.size() + " initializer methods:",
                    uncalledInitializers.stream()
                        .map(i -> "`" + i.initializer().getSimpleName() + "()`")
                        .collect(Collectors.joining(", "))),
            trees.getTree(object),
            compilationUnit);
      }
    }
  }

  /**
   * Get all methods declared by the object's type, any supertypes, and any interfaces, filtering
   * only those annotated with {@link PostConstructionInitializer}.
   */
  private Set<RequiredInitializer> getRequiredInitializers(VariableElement object) {
    var type = object.asType();

    if (type.getKind() != TypeKind.DECLARED) {
      return Set.of();
    }

    TypeElement typeElement = (TypeElement) ((DeclaredType) type).asElement();
    if (typeElement == null) {
      return Set.of();
    }

    // Use a LinkedHashSet to maintain stable iteration order and deduplicate methods
    SequencedSet<ExecutableElement> methods = new LinkedHashSet<>();

    // Elements#getAllMembers returns all members including inherited ones (classes + interfaces)
    for (Element member : m_task.getElements().getAllMembers(typeElement)) {
      if (member instanceof ExecutableElement method) {
        // Skip static methods; the annotation docs disallow static use
        if (method.getModifiers().contains(Modifier.STATIC)) {
          continue;
        }
        if (typeElement.getModifiers().contains(Modifier.PUBLIC)) {
          if (!method.getModifiers().contains(Modifier.PUBLIC)) {
            // Method has lower visibility than the owning type
            // TODO: Compiler error?
            continue;
          }
        } else if (typeElement.getModifiers().contains(Modifier.PROTECTED)) {
          if (!method.getModifiers().contains(Modifier.PROTECTED)
              && !method.getModifiers().contains(Modifier.PUBLIC)) {
            // Method has lower visibility than the owning type
            // TODO: Compiler error?
            continue;
          }
        } else if (typeElement.getModifiers().contains(Modifier.PRIVATE)) {
          // doesn't matter what the method's visibility is
        } else {
          // package-private
          if (method.getModifiers().contains(Modifier.PRIVATE)) {
            // Method has lower visibility than the owning type
            // TODO: Compiler error?
            continue;
          }
        }

        // Check for the annotation
        if (method.getAnnotation(PostConstructionInitializer.class) != null) {
          methods.add(method);
        }
      }
    }

    if (methods.isEmpty()) {
      return Set.of();
    }

    Set<RequiredInitializer> result = new LinkedHashSet<>();
    for (ExecutableElement m : methods) {
      result.add(new RequiredInitializer(m));
    }

    return result;
  }

  private final class State {
    private final Map<VariableElement, InitializedObject> m_initializedObjects = new HashMap<>();

    void addMaybeInitializedObject(VariableElement object) {
      var requiredInitializers = getRequiredInitializers(object);
      if (requiredInitializers.isEmpty()) {
        return;
      }
      m_initializedObjects.put(object, new InitializedObject(object, requiredInitializers));
    }

    void removeFullyInitializedObjects() {
      m_initializedObjects
          .values()
          .removeIf(initializedObject -> initializedObject.initializers().isEmpty());
    }

    void removeInitializer(VariableElement object, ExecutableElement initializer) {
      if (!m_initializedObjects.containsKey(object)) {
        return;
      }

      m_initializedObjects.get(object).initializers().removeIf(i -> i.is(initializer));
    }

    void merge(State otherState) {
      if (otherState == null) {
        return;
      }

      otherState.m_initializedObjects.forEach(
          (object, initializedObject) -> {
            m_initializedObjects.putIfAbsent(object, initializedObject);
            m_initializedObjects
                .get(object)
                .initializers()
                .addAll(initializedObject.initializers());
          });
    }
  }

  /**
   * Tracks what initializer methods still need to be called for a given object. Elements are
   * removed from the {@link #initializers} set as they are found; once the set is empty, the object
   * is considered fully initialized and is removed from the tracking set.
   *
   * @param object The object to track
   * @param initializers The set of initializer methods that still need to be called on the object.
   *     This is mutable!
   */
  private record InitializedObject(VariableElement object, Set<RequiredInitializer> initializers) {}

  private record RequiredInitializer(ExecutableElement initializer) {
    boolean is(ExecutableElement check) {
      return initializer.equals(check);
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
    public State reduce(State r1, State r2) {
      if (r1 == null) {
        return r2;
      }
      r1.merge(r2);
      r1.removeFullyInitializedObjects();
      return r1;
    }

    @Override
    public State visitBlock(BlockTree node, State localState) {
      // Always operate on a non-null state
      State workingState = localState != null ? localState : new State();

      super.visitBlock(node, workingState);

      // Remove any objects that are now fully initialized within this block.
      workingState.removeFullyInitializedObjects();
      return workingState;
    }

    @Override
    public State visitNewClass(NewClassTree node, State localState) {
      // Always operate on a non-null state
      State workingState = localState != null ? localState : new State();

      TreePath path = m_trees.getPath(m_root, node);

      if (isSuppressed(path)) {
        // Warnings are suppressed in this context, ignore
        return super.visitNewClass(node, workingState);
      }

      var parentElement = m_trees.getElement(path.getParentPath());
      if (parentElement instanceof VariableElement v) {
        workingState.addMaybeInitializedObject(v);
      } else if (path.getParentPath().getLeaf() instanceof AssignmentTree assignment) {
        var lhsElement = m_trees.getElement(m_trees.getPath(m_root, assignment.getVariable()));
        if (lhsElement instanceof VariableElement v) {
          workingState.addMaybeInitializedObject(v);
        }
      }

      super.visitNewClass(node, workingState);
      return workingState;
    }

    @Override
    public State visitMethodInvocation(MethodInvocationTree node, State localState) {
      // Always operate on a non-null state
      State workingState = localState != null ? localState : new State();

      TreePath path = m_trees.getPath(m_root, node);
      var invokedElement = m_trees.getElement(path);
      if (!(invokedElement instanceof ExecutableElement executableElement)) {
        super.visitMethodInvocation(node, workingState);
        return workingState;
      }

      // The invoked method doesn't have our annotation, skip. It's not an initializer method.
      if (executableElement.getAnnotation(PostConstructionInitializer.class) == null) {
        super.visitMethodInvocation(node, workingState);
        return workingState;
      }

      if (node.getMethodSelect() instanceof MemberSelectTree variableTree) {
        var element = m_trees.getElement(m_trees.getPath(m_root, variableTree.getExpression()));
        if (element instanceof VariableElement v
            && workingState.m_initializedObjects.containsKey(v)) {
          workingState.removeInitializer(v, executableElement);
        }
      }
      workingState.removeFullyInitializedObjects();

      super.visitMethodInvocation(node, workingState);
      return workingState;
    }

    /**
     * Checks if an element at the given path is annotated with
     * {@code @SuppressWarnings("PostConstructionInitializer")} or {@code @SuppressWarnings("all")}.
     * Also walks up the tree to check if any parent elements are annotated.
     *
     * @param path The path to check.
     * @return True if the element is annotated with the suppression annotation. False otherwise.
     */
    private boolean isSuppressed(TreePath path) {
      TreePath currentPath = path;
      while (currentPath != null) {
        var element = m_trees.getElement(currentPath);
        currentPath = currentPath.getParentPath();

        if (element == null) {
          continue;
        }

        for (AnnotationMirror annotationMirror : element.getAnnotationMirrors()) {
          var annotationType = annotationMirror.getAnnotationType().toString();
          if (!"java.lang.SuppressWarnings".equals(annotationType)) {
            continue;
          }

          for (var entry : annotationMirror.getElementValues().entrySet()) {
            var name = entry.getKey().getSimpleName().toString();
            if (!"value".equals(name)) {
              continue;
            }

            Object value = entry.getValue().getValue();
            if (value instanceof List<?> list) {
              // eg @SuppressWarnings({"PostConstructionInitializer", "OtherSuppression"})
              for (Object valueEntry : list) {
                if (valueEntry instanceof AnnotationValue a
                    && a.getValue() instanceof String str
                    && isSuppressionString(str)) {
                  return true;
                }
              }
            } else if (value instanceof String str && isSuppressionString(str)) {
              // eg @SuppressWarnings("PostConstructionInitializer")
              return true;
            }
          }
        }
      }

      // No suppression annotations found on the element or any parent element.
      return false;
    }

    private boolean isSuppressionString(String str) {
      return "all".equals(str) || PostConstructionInitializer.SUPPRESSION_KEY.equals(str);
    }
  }
}
