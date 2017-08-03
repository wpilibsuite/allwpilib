#include "ntcore.h"
#include "nt_Value.h"
#include <iostream>

int main() {
    nt::SetEntryValue("MyValue", nt::Value::MakeString("Hello World"));

    std::cout << nt::GetEntryValue("MyValue")->GetString() << std::endl;
}
