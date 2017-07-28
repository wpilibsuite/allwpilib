#include "llvm/StringRef.h"
#include <iostream>

int main() {
    llvm::StringRef v1("Hello");
    std::cout << v1.lower() << std::endl;
}
