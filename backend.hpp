#include "differ-tree.hpp"

void asm_operator (FILE* stream, CalcTree::Node_t *node);

void asm_cond_operator (FILE* stream, CalcTree::Node_t *node);

void write_to_asm (const CalcTree &code);

void asm_undertree (FILE* stream, CalcTree::Node_t *node);

void asm_un_function (FILE* stream, CalcTree::Node_t *node);

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

            asm_un_function (stream, node);
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
        {
            DEBUG_PRINT (Вызываю функцию);

            Node* curr_node = node;
            int local_counter = 0;
            while (curr_node -> right)
            {
                curr_node = curr_node -> right;
                asm_undertree (stream, curr_node -> left);
                fprintf (stream, "POPR %d\n", VAR_NUMBER + ARG_COUNTER * node->node_data.data.code + local_counter++);  
            }
            fprintf (stream, "\nCALL DEF%d\n", node->node_data.data.code);

            break;
        }

        case ARGUMENT:
        {
            DEBUG_PRINT (Пишу использование аргумента);
            int number_of_def = -1;

            Node* parent_search = node;
            while (parent_search->node_data.type != DEF)
                parent_search = parent_search -> father;

            number_of_def = parent_search->node_data.data.code;

            fprintf (stream, "PUSHR %d\n", VAR_NUMBER + ARG_COUNTER * number_of_def + node -> node_data.data.code);
            break;
        }

        throw "CANNOT ASM THIS TYPE";
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
        if ( node->left->node_data.type != VARIABLE)
            throw "Могу присвоить только переменной!";
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

    if (is_this_operator (code, LESS))
    {
        asm_undertree (stream, node -> left);
        asm_undertree (stream, node -> right);

        fprintf(stream, "JB COND_T%d\n"
                        "PUSH 0\n"
                        "JMP COND_F%d\n"
                        "COND_T%d:\n"
                        "PUSH 1\n"
                        "COND_F%d:\n", LABEL_COUNTER, LABEL_COUNTER, LABEL_COUNTER, LABEL_COUNTER);
        LABEL_COUNTER++;
        return; 
    }

    if (is_this_operator (code, MORE))
    {
        asm_undertree (stream, node -> left);
        asm_undertree (stream, node -> right);

        fprintf(stream, "JA COND_T%d\n"
                        "PUSH 0\n"
                        "JMP COND_F%d\n"
                        "COND_T%d:\n"
                        "PUSH 1\n"
                        "COND_F%d:\n", LABEL_COUNTER, LABEL_COUNTER, LABEL_COUNTER, LABEL_COUNTER);
        LABEL_COUNTER++;
        return; 
    }

    printf ("%s\n", operators[node->node_data.data.code]);
    throw "CANNOT ASM THIS OPERATOR";

}


void asm_cond_operator (FILE* stream, CalcTree::Node_t *node)
{
    if ( is_this_cond_op (node->node_data.data.code, "Если" ) )
    {
        int label = LABEL_COUNTER;
        LABEL_COUNTER++;

        asm_undertree (stream, node->left);
        fprintf (stream, "PUSH 0\n"
                         "JE COND_F%d\n", label);
        asm_undertree (stream, node->right);
        fprintf (stream, "COND_F%d:\n", label);
    }
}

void asm_un_function (FILE* stream, CalcTree::Node_t *node)
{
    if ( is_this_un_func (node->node_data.data.code, PRINT) )
    {
        asm_undertree (stream, node->right);
        fprintf (stream, "OUT\n" );
        return;
    }
    else if ( is_this_un_func (node->node_data.data.code, SIN) )
    {
        asm_undertree (stream, node->right);
        fprintf (stream, "SIN\n" );
        return;
    }
    else if ( is_this_un_func (node->node_data.data.code, SQRT) )
    {
        asm_undertree (stream, node->right);
        fprintf (stream, "SQRT\n" );
        return;
    }
    else if ( is_this_un_func (node->node_data.data.code, RETURN) )
    {
        asm_undertree (stream, node->right);
        fprintf (stream, "RET\n" );
        return;
    }
    else if ( is_this_un_func (node->node_data.data.code, INPUT) )
    {
        fprintf (stream, "IN\n" );
        fprintf (stream, "POPR %d\n", node->right->node_data.data.code);
        if (node->right->node_data.type != VARIABLE)
            throw "Могу считать только в переменную!";
        return;
    }
    else
        throw "Не могу ASM эту унарную функцию";
}