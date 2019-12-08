#ifndef CALC_TREE_COND_OPERATORS_INCLUDED
#define CALC_TREE_COND_OPERATORS_INCLUDED

#include <string.h>
#include <iostream>
#include <cassert>

const char* IF    = "Если";
const char* WHILE = "ДляЛюбого";

static const char* cond_operators[] = {IF, WHILE};

bool is_this_cond_op (char code, const char* func)
{   
    if ( ! strcmp ( cond_operators [code], func ) )
        return true;
    else
        return false;  
}

bool is_cond_operator (const char* potential_op)
{
    if ( strlen (potential_op) == 1)
        return false;

    for (int i = 0; i < sizeof (cond_operators)/sizeof (cond_operators[0]); i++)
        if ( ! strcmp (cond_operators[i], potential_op) )
            return true;

    return false;
}

char get_cond_operator_code (const char* func)
{
    for (char i = 0; i < sizeof (cond_operators)/sizeof (cond_operators[0]); i++)
    {
        if ( ! strcmp (cond_operators[i], func) )
        {
            return i;
        }
    }

    assert (false);
}

char* get_cond_op_by_code (char code)
{
    assert ( code < sizeof (cond_operators) / sizeof (cond_operators[0]) );
    char* res = strdup (cond_operators[code]);
    return res;
}

#endif