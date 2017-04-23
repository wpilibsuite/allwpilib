#include "ntcore.h"
#include <iostream>

int main() {
    auto myValue = nt::GetEntry(nt::GetDefaultInstance(), "MyValue");

    nt::SetEntryValue(myValue, nt::Value::MakeString("Hello World"));

    std::cout << nt::GetEntryValue(myValue)->GetString() << std::endl;
}
