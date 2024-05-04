package edu.wpi.first.epilogue.processor;

import javax.annotation.processing.ProcessingEnvironment;
import javax.lang.model.element.Element;
import javax.lang.model.type.TypeMirror;

public class MeasureHandler extends ElementHandler {
  private final TypeMirror m_measure;

  protected MeasureHandler(ProcessingEnvironment processingEnv) {
    super(processingEnv);

    m_measure =
        processingEnv
            .getTypeUtils()
            .erasure(
                processingEnv
                    .getElementUtils()
                    .getTypeElement("edu.wpi.first.units.Measure")
                    .asType());
  }

  @Override
  public boolean isLoggable(Element element) {
    return m_processingEnv.getTypeUtils().isAssignable(dataType(element), m_measure);
  }

  @Override
  public String logInvocation(Element element) {
    // DataLogger has builtin support for logging measures
    return "dataLogger.log(\"" + loggedName(element) + "\", " + elementAccess(element) + ")";
  }
}
