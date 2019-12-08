/// @file Compiler + executor
#include <stdlib.h>

///@brief Define COMPILE if want to Compile before execution
#define COMPILE

#ifdef COMPILE
    #define MAKE(asm,cpu)       \
        system ("g++ " #asm ".cpp" " -o " #asm);   \
        system ("g++ " #cpu ".cpp" " -o " #cpu);   \
        system ("clear");    \
        system ("./"#asm);   \
        system ("./"#cpu);
#else
    #define MAKE(asm,cpu)       \
        system ("./"#asm);   \
        system ("./"#cpu);
#endif


int main()
{
    MAKE(asm,CPU)
}
