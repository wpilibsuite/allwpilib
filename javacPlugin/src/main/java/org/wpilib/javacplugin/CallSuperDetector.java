// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.javacplugin;

import com.sun.source.tree.ClassTree;
import com.sun.source.tree.CompilationUnitTree;
import com.sun.source.tree.ExpressionTree;
import com.sun.source.tree.IdentifierTree;
import com.sun.source.tree.MemberSelectTree;
import com.sun.source.tree.MethodInvocationTree;
import com.sun.source.tree.MethodTree;
import com.sun.source.util.JavacTask;
import com.sun.source.util.TaskEvent;
import com.sun.source.util.TaskListener;
import com.sun.source.util.TreePath;
import com.sun.source.util.TreePathScanner;
import java.util.ArrayDeque;
import java.util.Deque;
import java.util.HashSet;
import java.util.Set;
import javax.lang.model.element.AnnotationMirror;
import javax.lang.model.element.Element;
import javax.lang.model.element.ElementKind;
import javax.lang.model.element.ExecutableElement;
import javax.lang.model.element.Modifier;
import javax.lang.model.element.TypeElement;
import javax.lang.model.type.DeclaredType;
import javax.lang.model.type.TypeKind;
import javax.lang.model.type.TypeMirror;
import javax.lang.model.util.ElementFilter;
import javax.lang.model.util.Elements;
import javax.lang.model.util.Types;

/**
 * Detects improper overrides of methods annotated with {@code @CallSuper}.
 */
public class CallSuperDetector implements TaskListener {
  private final JavacTask                m_task;
  private final Set<CompilationUnitTree> m_visitedCUs = new HashSet<>();

  private final Elements m_elements;
  private final Types    m_types;

  /**
   * Construct a CallSuperDetector.
   */
  public CallSuperDetector(JavacTask task) {
    m_task = task;
    m_elements = task.getElements();
    m_types = task.getTypes();
  }

  @Override
  public void finished(TaskEvent e) {
    // We override `finished` instead of `started` because we want to run after
    // the ANALYZE attribution phase has completed and assigned types to
    // elements in the AST.

    // Track the visited CUs to avoid re-processing the same CU multiple times
    // when we call `Trees.getElement()` on a tree path.

    if (e.getKind() == TaskEvent.Kind.ANALYZE && m_visitedCUs.add(e.getCompilationUnit())) {
      e.getCompilationUnit().accept(new Scanner(e.getCompilationUnit()), null);
    }
  }

  /**
   * Reports an error for every method that overrides a method annotated with
   * {@code @CallSuper} but does not invoke {@code super.method(...)} (or
   * {@code Iface.super.method(...)}) in its body.
   *
   * <p>The requirement is transitive: if {@code A.foo()} is {@code @CallSuper} and
   * {@code B.foo()} overrides it, then {@code C.foo()} (overriding
   * {@code B.foo()}) must also call {@code super.foo()}.
   */
  public final class Scanner extends WPILibTreeScanner<Void, Void> {
    private static final String CALL_SUPER_ANNOTATION = "org.wpilib.annotation.CallSuper";

    Scanner(CompilationUnitTree compilationUnit) {
      super(compilationUnit, CallSuperDetector.this.m_task);
    }

    @Override
    public Void visitMethod(MethodTree tree, Void unused) {
      checkMethod(tree);
      // Continue into the body so nested/anonymous/local classes are also checked.
      return super.visitMethod(tree, unused);
    }

    private void checkMethod(MethodTree tree) {
      if (tree.getBody() == null) {
        // abstract/native method: nothing to check
        return;
      }

      Element element = m_trees.getElement(getCurrentPath());
      if (!(element instanceof ExecutableElement method)) {
        return;
      }

      if (method.getKind() != ElementKind.METHOD
          || method.getModifiers().contains(Modifier.STATIC)
          || method.getModifiers().contains(Modifier.PRIVATE)) {
        // Constructors and static methods can't override anything.
        // Private methods can't be overridden.
        return;
      }
      if (!(method.getEnclosingElement() instanceof TypeElement owner)) {
        return;
      }

      // Check if method is required to call super implementation.
      Set<TypeElement> requiring = requiringSupertypes(method, owner);
      if (requiring.isEmpty()) {
        return;
      }

      // Ensure method calls super implementation.
      TreePath bodyPath = new TreePath(getCurrentPath(), tree.getBody());
      if (!new SuperCallFinder(method, owner).containsSuperCall(bodyPath)) {
        TypeElement firstRequiring = requiring.iterator().next();
        printError("Override of @CallSuper method `%s.%s` doesn't call `super.%s()`.".formatted(
          firstRequiring.getSimpleName(), method, method.getSimpleName()),
          tree,
          null // not suppressible
        );
      }
    }

    /**
     * Walks every supertype of {@code owner} (superclasses and interfaces, transitively)
     * and returns all types containing a {@code @CallSuper}-annotated method
     * that {@code method} overrides.
     */
    private Set<TypeElement> requiringSupertypes(ExecutableElement method, TypeElement owner) {
      Deque<TypeMirror> queue = new ArrayDeque<>(m_types.directSupertypes(owner.asType()));
      Set<TypeElement> seen = new HashSet<>();
      Set<TypeElement> requiringSupertypes = new HashSet<>();
      while (!queue.isEmpty()) {
        TypeMirror supertype = queue.poll();
        if (supertype.getKind() != TypeKind.DECLARED) {
          continue;
        }
        TypeElement superElement = (TypeElement) ((DeclaredType) supertype).asElement();
        if (!seen.add(superElement)) {
          continue;
        }
        for (ExecutableElement candidate : ElementFilter.methodsIn(
            superElement.getEnclosedElements())) {
          if (hasCallSuper(candidate)
              && !candidate.getModifiers()
                           .contains(Modifier.ABSTRACT) // nothing to call
              && m_elements.overrides(method, candidate, owner)) {
            requiringSupertypes.add(superElement);
          }
        }
        queue.addAll(m_types.directSupertypes(supertype));
      }
      return requiringSupertypes;
    }

    /**
     * Check if a method is annotated with {@code @CallSuper}.
     */
    private static boolean hasCallSuper(ExecutableElement method) {
      for (AnnotationMirror mirror : method.getAnnotationMirrors()) {
        Element annotationType = mirror.getAnnotationType()
                                       .asElement();
        if (annotationType instanceof TypeElement te
            && CALL_SUPER_ANNOTATION.contentEquals(te.getQualifiedName())) {
          return true;
        }
      }
      return false;
    }

    /**
     * Looks for {@code super.m(...)} / {@code Iface.super.m(...)} that resolves
     * to a super method.
     */
    private final class SuperCallFinder extends TreePathScanner<Void, Void> {
      private final ExecutableElement method;
      private final TypeElement       owner;
      private boolean                 found;

      SuperCallFinder(ExecutableElement method, TypeElement owner) {
        this.method = method;
        this.owner = owner;
      }

      boolean containsSuperCall(TreePath bodyPath) {
        scan(bodyPath, null);
        return found;
      }

      @Override
      public Void visitMethodInvocation(MethodInvocationTree node, Void unused) {
        ExpressionTree select = node.getMethodSelect();
        if (select instanceof MemberSelectTree member && isSuper(member.getExpression())) {
          Element invoked = m_trees.getElement(new TreePath(getCurrentPath(), select));
          // `super.foo()` resolves to the nearest implementation, which may be
          // an intermediate class's override rather than the @CallSuper method itself.

          // Check that we override it.
          if (invoked instanceof ExecutableElement exec
              && m_elements.overrides(method, exec, owner)) {
            found = true;
          }
        }
        return found ? null : super.visitMethodInvocation(node, unused);
      }

      @Override
      public Void visitClass(ClassTree node, Void unused) {
        // `super` inside a nested/anonymous class refers to that class's super,
        // not ours, so skip scanning anonymous classes.
        return null;
      }

      private static boolean isSuper(ExpressionTree expr) {
        if (expr instanceof IdentifierTree ident) {
          // super.foo()
          return ident.getName()
                      .contentEquals("super");
        }
        if (expr instanceof MemberSelectTree member) {
          // ParentType.super.foo() or OuterType.super.foo()
          return member.getIdentifier()
                       .contentEquals("super");
        }
        return false;
      }
    }
  }
}
