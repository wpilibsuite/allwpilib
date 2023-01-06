#import <Foundation/Foundation.h>
#include <mutex>
#include <string>
#include <libgen.h>

int main(int argc, char *argv[]) {
    (void)argc;
    // Get process name with argv
    char* exeName = basename(argv[0]);

    NSTask* task;
}
