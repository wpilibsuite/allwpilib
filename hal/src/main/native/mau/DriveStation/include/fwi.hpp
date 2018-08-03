#pragma once

#include <cstring>
#include <cstdlib>
#include <inttypes.h>

#ifndef FWIFUNC
    #if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
        #define FWIFUNC __declspec(dllexport)
    #else
        #define FWIFUNC
    #endif
#endif

#define FWI_MEM_VAL(type, ptr, offset) *(type *)(ptr + offset)

#define FWI_SET_BIT(expression, bit) (expression |= (1 << bit))
#define FWI_UNSET_BIT(expression, bit) (expression &= ~(1 << bit))
#define FWI_SET_BIT_TO(expression, bit, val) (expression ^= (-(val ? 1 : 0) ^ expression) & (1 << bit))
#define FWI_IS_BIT_SET(expression, bit) ((expression & (1 << bit)) != 0)

namespace FWI {
    struct Block {
        virtual ~Block() {};

        FWIFUNC virtual void allocate(bool zero = true) {
            if (zero)   _store = (char *)::calloc(SIZE, 1);
            else        _store = (char *)::malloc(SIZE);
            _update_ptr();
        }
        FWIFUNC virtual void free() {
            ::free(_store);
        }
        FWIFUNC virtual void map_to(char *memory) {
            _store = memory;
            _update_ptr();
        }
        FWIFUNC virtual int copy_to(char *memory) {
            ::memcpy(memory, _store, SIZE);
            return SIZE;
        }
        FWIFUNC char *get_store() {
            return _store;
        }
        virtual void _update_ptr() {};

        static const int SIZE = 0;
    protected:
        char *_store;
    };
}