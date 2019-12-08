#include "differ-tree.hpp"

void asm_operator (FILE* stream, CalcTree::Node_t *node);

void asm_cond_operator (FILE* stream, CalcTree::Node_t *node);

void write_to_asm (const CalcTree &code);

void asm_undertree (FILE* stream, CalcTree::Node_t *node);

void write_to_asm (const CalcTree &code)
{
    FILE* stream = fopen (OUTPUT_CODE, "w");
    
    srand ( time (NULL) );

    for (int i = 0; variables[i]; i++)
        fprintf (stream, "PUSH %d\n"
                         "PUSHR %d\n", rand (), i);
        fprintf (stream, ";ЗАБИЛИ ПЕРЕМЕННЫЕ МУСОРОМ\n\n");

    fprintf (stream, "CALL MAIN\n"
                     "END\n");

    asm_undertree (stream, code.head);

    fclose (stream);
}

void asm_undertree (FILE* stream, CalcTree::Node_t *node)
{
    switch (node -> node_data.type)
    {
        case OPERATOR:

            asm_operator (stream, node);
            break;

        case QUANTITY:

            fprintf (stream, "PUSH %g\n", node -> node_data.data.value);
            break;

        case BLOCK:

            asm_undertree (stream, node -> left);
            if (node -> right) 
                asm_undertree (stream, node -> right);
            break;

        case MAIN_DEF:

            fprintf (stream, "\nMAIN:\n");
            asm_undertree (stream, node -> right);
            fprintf (stream, "RET\n\n");
            break;

        case VARIABLE:

            fprintf (stream, "PUSHR %d\n", node -> node_data.data.code);
            break;

        case UN_FUNCTION:

            asm_undertree (stream, node -> right);
            fprintf (stream, "%s\n", un_unction_asm_cmd (node -> node_data.data.code) );
            break;

        case COND_OPERATOR:
    
            asm_cond_operator (stream, node);
            break;

        case DEF:

            fprintf (stream, "\nDEF%d:\n", node->node_data.data.code);
            asm_undertree (stream, node -> right);
            fprintf (stream, "RET\n\n");
            break;

        case CALL:
            fprintf (stream, "\nCALL DEF%d\n", node->node_data.data.code);
            break;
    } 
}

void asm_operator (FILE* stream, CalcTree::Node_t *node)
{
    char code = node->node_data.data.code;

    if (is_this_operator (code, ADD))
    {
        asm_undertree (stream, node -> left);
        asm_undertree (stream, node -> right);
        fprintf (stream, "ADD\n");
        return;
    }

    if (is_this_operator (code, SUB))
    {
        asm_undertree (stream, node -> left);
        asm_undertree (stream, node -> right);
        fprintf (stream, "SUB\n");
        return;
    }

    if (is_this_operator (code, DIV))
    {
        asm_undertree (stream, node -> left);
        asm_undertree (stream, node -> right);
        fprintf (stream, "DIV\n");
        return;
    }

    if (is_this_operator (code, MUL))
    { 
        asm_undertree (stream, node -> left);
        asm_undertree (stream, node -> right);
        fprintf (stream, "MUL\n");
        return;
    }

    if (is_this_operator (code, ASSGN))
    {
        asm_undertree (stream, node -> right);
        fprintf (stream, "POPR %d\n", node->left->node_data.data.code);
        return;   
    }

    if (is_this_operator (code, NOT_EQUAL))
    {
        asm_undertree (stream, node -> left);
        asm_undertree (stream, node -> right);

        fprintf(stream, "JNE COND_T%d\n"
                        "PUSH 0\n"
                        "JMP COND_F%d\n"
                        "COND_T%d:\n"
                        "PUSH 1\n"
                        "COND_F%d:\n", LABEL_COUNTER, LABEL_COUNTER, LABEL_COUNTER, LABEL_COUNTER);
        LABEL_COUNTER++;
        return;
    }

    if (is_this_operator (code, EQUAL))
    {
        asm_undertree (stream, node -> left);
        asm_undertree (stream, node -> right);

        fprintf(stream, "JE COND_T%d\n"
                        "PUSH 0\n"
                        "JMP COND_F%d\n"
                        "COND_T%d:\n"
                        "PUSH 1\n"
                        "COND_F%d:\n", LABEL_COUNTER, LABEL_COUNTER, LABEL_COUNTER, LABEL_COUNTER);
        LABEL_COUNTER++;
        return; 
    }

    throw "CANNOT ASM THIS OPERATOR";

}


void asm_cond_operator (FILE* stream, CalcTree::Node_t *node)
{
    if ( is_this_cond_op (node->node_data.data.code, "Если" ) )
    {
        asm_undertree (stream, node->left);
        fprintf (stream, "PUSH 0\n"
                         "JE COND_F%d\n", LABEL_COUNTER);
        asm_undertree (stream, node->right);
        fprintf (stream, "COND_F%d:\n", LABEL_COUNTER);
    }
}