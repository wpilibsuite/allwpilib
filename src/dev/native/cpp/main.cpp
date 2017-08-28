#include "llvm/StringRef.h"
#include "llvm/SmallVector.h"
#include <iostream>

#include "support/hostname.h"

int main() {
    llvm::StringRef v1("Hello");
    std::cout << v1.lower() << std::endl;
}
