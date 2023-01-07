#import <Foundation/Foundation.h>
#include <string>
#include <filesystem>

int main(int argc, char *argv[]) {
    (void)argc;
    (void)argv;
    // Get process name with argv
    NSString *exePathPlat = [[NSBundle mainBundle] bundlePath];

    std::filesystem::path exePath{[exePathPlat UTF8String]};
    if (exePath.empty()) {
        printf("Exe path is empty, should be %s\n", [exePathPlat UTF8String]);
        return 1;
    }

    if (!exePath.has_stem()) {
        printf("Exe does not have stem? %s\n", exePath.c_str());
        return 1;
    }

    if (!exePath.has_parent_path()) {
        printf("Exe does not have parent path? %s\n", exePath.c_str());
        return 1;
    }

    std::filesystem::path JarPath = exePath.replace_extension("jar");
    std::filesystem::path ParentPath = JarPath.parent_path();

    if (!ParentPath.has_parent_path()) {
        printf("Exe directory does not have parent path? %s\n", exePath.c_str());
        return 1;
    }
    std::filesystem::path ToolsFolder = ParentPath.parent_path();

    printf("%s %s %s\n", exePath.c_str(), exePath.c_str(), JarPath.c_str());

    std::filesystem::path Java = ToolsFolder / "jdk" / "bin" / "java";

    std::string ToRun = " -jar \"";
    ToRun += JarPath;
    ToRun += "\"";

    printf("%s\n%s\n", Java.c_str(), ToRun.c_str());

   NSArray<NSString *> * Arguments = @[
    @"-jar",
    [NSString stringWithFormat:@"\"%s\"", ToRun.c_str()]
   ];

    NSTask *task = [[NSTask alloc] init];
    task.launchPath = [NSString stringWithFormat:@"%s", Java.c_str()];
    task.arguments = Arguments;
    task.terminationHandler = ^(NSTask* t){
        (void)t;
        printf("Running term handler");
        CFRunLoopStop(CFRunLoopGetMain());
    };

    NSLog(@"Launching %@", task.launchPath);
    if (![task launchAndReturnError: nil]) {
        printf("Failed to start. Trying home or path");
        task.terminationHandler = nil;

        NSString* javaHome = [[[NSProcessInfo processInfo] environment] objectForKey:@"JAVA_HOME"];
        task = [[NSTask alloc] init];
        task.launchPath = @"java";
        if (javaHome != nil) {
            std::filesystem::path javaHomePath{[javaHome UTF8String]};
            javaHomePath /= "bin";
            javaHomePath /= "java";
            task.launchPath = [NSString stringWithFormat:@"%s", javaHomePath.c_str()];
        }
        task.arguments = Arguments;
        task.terminationHandler = ^(NSTask* t){
            (void)t;
            CFRunLoopStop(CFRunLoopGetMain());
        };

        NSLog(@"Launching %@", task.launchPath);
        if (![task launchAndReturnError: nil]) {
            printf("Failed 2nd launch");
            return 1;
        }
    }

    CFRunLoopRunInMode(kCFRunLoopDefaultMode, 3, false);

    return task.running ? 0 : 1;
}
