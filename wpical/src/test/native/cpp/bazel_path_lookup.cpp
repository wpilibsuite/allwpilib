#include <string>

#ifdef __BAZEL__
#include "tools/cpp/runfiles/runfiles.h"

using bazel::tools::cpp::runfiles::Runfiles;

std::string LookupPath(std::string path) {
    std::string error;
    std::unique_ptr<Runfiles> runfiles(Runfiles::CreateForTest(&error));
    return runfiles->Rlocation("__main__/" + path);
}

#else
std::string LookupPath(std::string path) {
    return path;
}
#endif
