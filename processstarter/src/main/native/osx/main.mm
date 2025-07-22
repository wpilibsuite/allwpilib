// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#import <Foundation/Foundation.h>
#include "main.h"

#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
  (void)argc;
  (void)argv;
  NSString* exePathPlat = [[NSBundle mainBundle] bundlePath];
  NSString* identifier = [[NSBundle mainBundle] bundleIdentifier];
  if (identifier == nil) {
    exePathPlat = [[NSBundle mainBundle] executablePath];
  }

  std::filesystem::path exePath{[exePathPlat UTF8String]};
  if (exePath.empty()) {
    return 1;
  }

  if (!exePath.has_stem()) {
    return 1;
  }

  if (!exePath.has_parent_path()) {
    return 1;
  }

  if (exePath.stem() == (L"AdvantageScope")) {
    std::filesystem::path AdvantageScopePath{
        exePath.parent_path().parent_path() / L"advantagescope" /
        L"Advantagescope (WPILib).app" / L"Contents" / L"MacOS" /
        L"advantagescope"};
    return StartExeTool(AdvantageScopePath);
  } else if (exePath.stem() == (L"Elastic")) {
    std::filesystem::path ElasticPath{exePath.parent_path().parent_path() /
                                      L"elastic" / L"elastic_dashboard.app" /
                                      L"Contents" / L"MacOS" /
                                      L"elastic_dashboard"};
    return StartExeTool(ElasticPath);
  } else {
    return StartJavaTool(exePath);
  }
}

int StartJavaTool(std::filesystem::path& exePath) {
  std::filesystem::path jarPath{exePath};
  jarPath.replace_extension("jar");
  std::filesystem::path parentPath{exePath.parent_path()};

  if (!parentPath.has_parent_path()) {
    return 1;
  }
  std::filesystem::path toolsFolder{parentPath.parent_path()};

  std::filesystem::path java = toolsFolder / "jdk" / "bin" / "java";

  NSArray<NSString*>* Arguments =
      @[ @"-jar", [NSString stringWithFormat:@"%s", jarPath.c_str()] ];

  NSTask* task = [[NSTask alloc] init];
  task.launchPath = [NSString stringWithFormat:@"%s", java.c_str()];
  task.arguments = Arguments;
  task.terminationHandler = ^(NSTask* t) {
    (void)t;
    CFRunLoopStop(CFRunLoopGetMain());
  };

  if (![task launchAndReturnError:nil]) {
    task.terminationHandler = nil;

    NSString* javaHome =
        [[[NSProcessInfo processInfo] environment] objectForKey:@"JAVA_HOME"];
    task = [[NSTask alloc] init];
    task.launchPath = @"java";
    if (javaHome != nil) {
      std::filesystem::path javaHomePath{[javaHome UTF8String]};
      javaHomePath /= "bin";
      javaHomePath /= "java";
      task.launchPath = [NSString stringWithFormat:@"%s", javaHomePath.c_str()];
    }
    task.arguments = Arguments;
    task.terminationHandler = ^(NSTask* t) {
      (void)t;
      CFRunLoopStop(CFRunLoopGetMain());
    };

    if (![task launchAndReturnError:nil]) {
      return 1;
    }
  }

  CFRunLoopRunInMode(kCFRunLoopDefaultMode, 3, false);

  return task.running ? 0 : 1;
}

int StartExeTool(std::filesystem::path& exePath) {
  std::cout << "exePath: " << exePath.c_str() << std::endl;
  NSTask* task = [[NSTask alloc] init];
  task.launchPath = [NSString stringWithFormat:@"%s", exePath.c_str()];
  // task.arguments = Arguments;
  task.terminationHandler = ^(NSTask* t) {
    (void)t;
    CFRunLoopStop(CFRunLoopGetMain());
  };

  if (![task launchAndReturnError:nil]) {
    return 1;
  }

  CFRunLoopRunInMode(kCFRunLoopDefaultMode, 3, false);

  return task.running ? 0 : 1;
}
