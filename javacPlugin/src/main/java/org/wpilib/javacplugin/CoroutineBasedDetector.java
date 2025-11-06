// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.javacplugin;

import com.sun.source.tree.CompilationUnitTree;
import com.sun.source.util.JavacTask;
import com.sun.source.util.TaskEvent;
import com.sun.source.util.TaskListener;
import com.sun.source.util.TreeScanner;
import java.util.HashSet;
import java.util.Set;
import javax.lang.model.type.TypeMirror;

/**
 * Base class for detectors that scan for code that uses coroutines. Subclasses should provide a
 * {@link TreeScanner} class that implements the logic for the detector.
 */
public abstract class CoroutineBasedDetector implements TaskListener {
  protected final JavacTask m_task;
  private final Set<CompilationUnitTree> m_visitedCUs = new HashSet<>();

  /**
   * The type of the Coroutine class. This is null if the commands v3 library is not on the
   * classpath, or if the field is read before the Java compiler finishes the analyze phase. Custom
   * scanners are only used if this is non-null; scanner code can safely assume that it is present.
   */
  protected TypeMirror m_coroutineType;

  protected CoroutineBasedDetector(JavacTask task) {
    m_task = task;
  }

  @Override
  public void finished(TaskEvent taskEvent) {
    var compilationUnit = taskEvent.getCompilationUnit();
    if (taskEvent.getKind() == TaskEvent.Kind.ANALYZE && m_visitedCUs.add(compilationUnit)) {
      m_coroutineType = getCoroutineType();

      if (m_coroutineType == null) {
        // Not using the commands library; nothing to scan for
        return;
      }

      compilationUnit.accept(createScanner(compilationUnit), null);
    }
  }

  protected abstract TreeScanner<?, ?> createScanner(CompilationUnitTree compilationUnit);

  private TypeMirror getCoroutineType() {
    var te = m_task.getElements().getTypeElement("org.wpilib.commands3.Coroutine");
    return te == null ? null : te.asType();
  }
}
