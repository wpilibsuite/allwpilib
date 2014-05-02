#ifndef __SYM_MODULE_LINK_H__
#define __SYM_MODULE_LINK_H__

#include "HAL/HAL.hpp"

#ifdef __cplusplus
extern "C" {
#endif

extern STATUS    moduleNameFindBySymbolName
    (
        const char *    symbol,        /* symbol name to look for */
        char * module        /* where to return module name */
    );

#ifdef __cplusplus
}
#endif

#endif

