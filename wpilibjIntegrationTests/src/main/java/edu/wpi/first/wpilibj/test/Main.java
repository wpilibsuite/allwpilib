// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.test;

import static org.junit.platform.engine.discovery.ClassNameFilter.includeClassNamePatterns;
import static org.junit.platform.engine.discovery.DiscoverySelectors.selectPackage;

import java.io.PrintWriter;
import java.nio.file.Paths;
import org.junit.platform.launcher.Launcher;
import org.junit.platform.launcher.core.LauncherDiscoveryRequestBuilder;
import org.junit.platform.launcher.core.LauncherFactory;
import org.junit.platform.launcher.listeners.SummaryGeneratingListener;
import org.junit.platform.reporting.legacy.xml.LegacyXmlReportGeneratingListener;

public class Main {
  public static void main(String[] args) {
    var request =
        LauncherDiscoveryRequestBuilder.request()
            .selectors(selectPackage("edu.wpi.first.wpilibj"))
            .filters(includeClassNamePatterns(".*Test"))
            .build();

    Launcher launcher = LauncherFactory.create();

    var summaryListener = new SummaryGeneratingListener();
    launcher.registerTestExecutionListeners(summaryListener);

    var xmlListener =
        new LegacyXmlReportGeneratingListener(Paths.get("."), new PrintWriter(System.out, true));
    launcher.registerTestExecutionListeners(xmlListener);

    launcher.execute(request);
  }
}
