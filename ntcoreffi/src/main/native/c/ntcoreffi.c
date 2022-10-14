#define AddFunctionToLink(FuncName) \
    extern void FuncName(void); \
    FuncName()

void DoNotCallMeImOnlyForLinking(void) {
#include "ExportedSymbols.h"
}
