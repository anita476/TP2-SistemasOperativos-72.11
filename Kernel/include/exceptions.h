#ifndef _EXCEPTIONS_H_
#define _EXCEPTIONS_H_

#include <stdint.h>

/*
    @brief Dispatches the exception
    @param exception The exception
    @param registers The registers
*/
void exception_dispatcher(int exception, uint64_t registers[17]);

#endif