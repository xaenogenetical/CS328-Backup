#include "keys.h"

#define jump(label) (goto label)

// User is responsible for ensuring that start and end are of the form void** and point to valid labels (memory locations)
#define my_if(boolean_exp, start, end) ({ \
    \ 
    int a;                                \
    a = (boolean_exp && jump(start));     \
    jump(end);                            \
})
/*
void my_if(int boolean_exp, char *open_brace, char *close_brace)
{
    // boolean_exp will only be 0b0 or 0b1
    int a;
    a = boolean_exp && jump(open_brace);
    a = boolean_exp || jump(close_brace);
}*/

void func()
{
    // my_if(1, lav1, lav2);
}