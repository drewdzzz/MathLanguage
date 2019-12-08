#ifndef FRONTEND_MODULE_INCLUDED
#define FRONTEND_MODULE_INCLUDED

static const char* OUTPUT_CODE = "asm_code.txt";

const int VAR_NUMBER = 100;

const int DEF_NUMBER = 20;

const int ARG_NUMBER = 10;

char *variables [VAR_NUMBER] = {};

char *def [DEF_NUMBER] = {};

char* DEF_ARGUMENTS[DEF_NUMBER][ARG_NUMBER] = {};


#include "differ-tree.hpp"

static int LINE_COUNTER = 1;
static int LABEL_COUNTER = 1;
static int DEF_COUNTER = 0;
static int VAR_COUNTER = 0;
static int ARG_COUNTER = 0;

bool is_variable (char* potentional_var);

int get_variable_num (char* var);

bool is_def (char* potentional_var);

int get_def_num (char* var);

bool is_argument (char* potentional_arg, int func_num);

int get_argument_num (char* arg, int func_num);

#ifdef DEBUG
    #define DEBUG_CODE(code) code;
    #define DEBUG_PRINT(print) printf ("\\\\debug: %s\n", #print);
#else
    #define DEBUG_CODE(code)
    #define DEBUG_PRINT(print) 
#endif

const char* INPUT_PROG = "programm.drew";

static const int BUFFERSIZE = 100;

typedef CalcTree::Node_t Node;

Node* Get_N (char *&input);

Node* Get_F (char *&input, int func_num);

Node* Get_P (char *&input, int func_num);

Node* Get_K (char *&input, int func_num);

Node* Get_T (char *&input, int func_num);

Node* Get_E (char *&input, int func_num);

Node* Get_Cond (char *&input, int func_num);

Node* Get_ASSGN (char* &input, int func_num);

Node* Get_Cond_Op (char* &input, int func_num);

Node* GetLogic (char* &input, int func_num);

Node* GetDef (char* &input);

Node* GetMain (char* &input);

Node* GetArgs (char* &input, int func_num);

void GetVars (char* &input);

Node *Get_G (char* input);

FILE* open_file(const char* input_file_name);

long size_of_file (FILE* stream);

void kill_spaces (char* &input);

///Проверяет на предмет того, что буква в вводе соотвествует нужной, и проталкивает указатель вперёд, если результат положительный
bool check (char* &input, const char letter);

///Проверяет на предмет того, что строка в вводе соотвествует нужной, и проталкивает указатель вперёд, если результат положительный
bool check (char* &input, const char* string);












bool check (char* &input, const char* string)
{
    char readed [BUFFERSIZE] = {};
    int read_count = 0;
    if ( sscanf (input, " %s%n", readed, &read_count) )
        if ( ! strcmp (readed, string) ) 
        {
            input += read_count;
            return true;
        }

    return false;
}

bool check (char* &input, const char letter)
{
    if ( *(input) == letter )
    {
        input++;
        return true;
    }
    
    return false;
}



Node *Get_G (char* input)
{ 
    char func[BUFFERSIZE] = {};
    Node *main_node = new Node;
    main_node -> node_data.type = BLOCK;

    Node *current_node = main_node;
    
    DEBUG_PRINT ( gonna get variables );

    GetVars (input);

    DEBUG_PRINT ( variables were set );

    while (1)
    {
        current_node -> left = GetDef (input);

        DEBUG_PRINT (I got a function);
        if ( current_node -> left )
        {
            DEBUG_PRINT (Function was valid);
            current_node -> left -> father = current_node;
            current_node -> right = new Node;
                current_node -> right -> node_data.type = BLOCK;
                current_node -> right -> father = current_node; 
            current_node = current_node -> right;
        }
        else
            break;
    }

    DEBUG_PRINT ( gonna read main );

    current_node -> left = GetMain (input);
    kill_spaces (input);
 
    if (*input)
        throw "Main должен быть последней функцией в программе";


    return main_node;
}


void GetVars (char* &input)
{
    int read_count = 0;
    char readed [BUFFERSIZE] = {};
    kill_spaces (input);

    if ( check (input, "НетАксиоматики") )
        return;
    else
    if ( check (input, "Аксиоматика") )
    {
        DEBUG_PRINT ( Аксиоматика checked )

        while ( ! check (input, "НеОбосновывать") )
        {
            kill_spaces (input);
            DEBUG_PRINT (Аксиоматика was found);
            sscanf (input, "%[^ \n\t\r,.:;^+-*/]%n", readed, &read_count);
            input += read_count;
            
            DEBUG_CODE ( printf ("Input in get_vars: %s\n", input) );



            variables[VAR_COUNTER++] = strdup (readed);  //УДАЛИТЬ ПОСЛЕ ИСПОЛЬЗОВАНИЯ!!

            kill_spaces(input);
            if ( *input == ',' )
                input++;
            else
                throw "Ошибка в блоке объявления переменных";

            kill_spaces(input);
        }

            DEBUG_CODE 
            (
                std::cout<<"Variables:\n";
                for (int i = 0; i < VAR_NUMBER && variables[i]; i++)
                    std::cout<<i<<": "<<variables[i]<<std::endl<<std::endl; 
            );

        return;
    }
    
    throw "Нет Блока Объявления переменных";
}


Node* GetDef (char* &input)
{
    Node* def_node = new Node;
    def_node -> node_data.type = DEF;
    def_node -> node_data.data.code = DEF_COUNTER;

    kill_spaces (input);

    if ( ! check (input, "Лемма") )
    {
        return nullptr;
    }

    kill_spaces (input);

    DEBUG_PRINT (Приступаю к считыванию аргументов функции);

    int read_count = 0;
    char readed [BUFFERSIZE] = {};  

    if ( ! sscanf (input, "%[^ \n\t\r,.:;^+-*/]%n", readed, &read_count) || ! strcmp (readed, "Доказательство"))
        throw "Функция без идентификатора";
  
    input += read_count;

    def [DEF_COUNTER] = strdup (readed);

    kill_spaces (input);

    if ( check (input, ','))
    {
        Node* curr_node = def_node;

        kill_spaces (input);

        if (! check (input, "Доказывают" ) )
            throw "Ожидалось слово \"Доказывают\" после детерминирующего символа в обьявлении функции";


        while (1)
        {
            kill_spaces (input);
            if ( !sscanf (input, "%[^ \n\t\r,.:;^+-*/]%n", readed, &read_count) )
                break;

            curr_node->left = new Node;
            curr_node->left->father = curr_node;
            curr_node = curr_node->left;

            curr_node->node_data.type = ARGUMENT;
            curr_node->node_data.data.code = ARG_COUNTER;
            DEF_ARGUMENTS[DEF_COUNTER][ARG_COUNTER++] = strdup (readed);           

            input += read_count;

            kill_spaces (input);
            if ( ! check (input, ',') )
                break;
        }
        
    }




    DEBUG_PRINT (Считал аргументы функции);

    DEBUG_CODE ( printf ("input после считывания аргументов: %s\n", input) )

    kill_spaces (input);

    if (! check (input, "Доказательство"))
        throw "Не могу найти тело Функции";

    DEBUG_PRINT (Доказательство was found);


    DEBUG_PRINT (Gonna set logic);

    kill_spaces (input);

    def_node -> right = GetLogic (input, DEF_COUNTER);

    def_node -> right -> father = def_node;

    DEBUG_PRINT (Logic was set);

    kill_spaces (input);

    DEBUG_CODE (printf ("Input in GetDef: %s\n", input) );

    DEBUG_PRINT (MAIN was parsed);

    DEF_COUNTER++;
    ARG_COUNTER = 0;

    return def_node;

}


Node* GetMain (char* &input)
{

    kill_spaces (input);

    if ( ! check (input, "Теорема") )
    {
        DEBUG_PRINT (There is no Теорема word);
        
        throw "Не могу найти main-функцию";
    }

    kill_spaces (input);
    if (! check (input, "Доказательство"))
        throw "Не могу найти тело Теоремы";

    DEBUG_PRINT (Доказательство was found);


    Node* main_func = new Node;
    main_func -> node_data.type = MAIN_DEF;

    DEBUG_PRINT (Gonna set logic);

    kill_spaces (input);

    main_func -> right = GetLogic (input, -1);

    DEBUG_PRINT (Logic was set);

    kill_spaces (input);

    DEBUG_CODE (printf ("Input in Get_Main: %s\n", input) );

    DEBUG_PRINT (MAIN was parsed);

    return main_func;

}


Node* GetLogic (char* &input, int func_num)
{
    kill_spaces (input);
    DEBUG_PRINT (Making logic);

    char end_check [BUFFERSIZE] = {};

    Node* main_node = new Node;
    Node* current_node = main_node;
    while ( ! check (input, "Доказано"))
    {
        current_node -> node_data.type = BLOCK;

        DEBUG_PRINT (Gonna get Conditional operator);
        current_node -> left = Get_Cond_Op (input, func_num);//ЗОКОНЧИ!
        current_node -> left -> father = current_node;

        DEBUG_PRINT (Got Conditional operator);

        if ( ! current_node -> left)
            break;

        kill_spaces (input);
        if ( *input != ',' )
        {
            throw "Ожидалась запятая в конце инструкции (возможно, ваша инструкция не была распознана)";        //ИЛИ НЕОПОЗНАННАЯ ИНСТРУКЦИЯ!!!!
            return nullptr;
        }
        input++;

        DEBUG_CODE (printf ("Input after clearing comma in GetLogic: %s\n", input));

        current_node -> right = new Node;
        current_node -> right -> father = current_node;
        current_node = current_node -> right;
        
        kill_spaces (input);
    }

        current_node = current_node -> father;
        delete current_node -> right;
        current_node -> right = nullptr;


    DEBUG_PRINT (Доказано was found);

    return main_node;
}


Node* Get_Cond_Op (char* &input, int func_num)
{

    int read_count = 0;
    char letters[BUFFERSIZE] = {};

    kill_spaces (input);

    if ( ! sscanf (input, "%[^ \n\t\r.,=+0-9:;^+-*/]%n", letters, &read_count) )
    {
        assert (false);                          
    }

    DEBUG_CODE ( printf ("letters: %s\n", letters) );

    if ( is_cond_operator (letters) ) 
    {
        Node* cond_block = new Node;
        cond_block -> node_data.type = COND_OPERATOR;
        cond_block -> node_data.data.code = get_cond_operator_code (letters);

        DEBUG_CODE (printf ( "Conditional operator: %s\n", letters )); 

        DEBUG_PRINT (Cond operator detected);

        input += read_count;

        cond_block -> left = Get_Cond (input, func_num);
        cond_block -> left -> father = cond_block;

        kill_spaces (input);
        
        if ( *input == ':' )
            input++;
        else
            throw "Требуется двоеточие после Условного оператора";

        cond_block -> right = Get_ASSGN (input, func_num);
        cond_block -> right -> father = cond_block;

        return cond_block;

    }
    else
    {
        DEBUG_PRINT (gonna get assignment);
        return Get_ASSGN (input, func_num);
    }

    throw "Block get cond op error";

}


Node* Get_ASSGN (char* &input, int func_num)
{
    kill_spaces (input);
    Node* node = Get_Cond (input, func_num); //Отдельно проверить на то, что слева от присваивания переменная

    kill_spaces (input);

    char letters[BUFFERSIZE] = {};
    int read_count = 0;

    sscanf (input, "%[^ \n\t\r.,=+0-9:;^+-*/]%n", letters, &read_count);

    Node* new_node = nullptr;
    while ( ! strcmp (letters, ASSGN) )
    {
        DEBUG_PRINT (Assignment detected);
        DEBUG_CODE (printf ("Assignment word: %s\n", letters));

        new_node = new Node;

        new_node -> node_data.type = OPERATOR;
        new_node -> node_data.data.code = get_operator_code (letters);

        new_node -> left = node;
        node -> father = new_node;
        node = new_node;
        new_node = nullptr;

        DEBUG_CODE (printf ("Input before assignment: %s\n", input));
        input += read_count;
        DEBUG_CODE (printf ("Input after assignment: %s\n", input));

        node -> right = Get_Cond (input, func_num);
        node -> right -> father = node;

        letters[0] = 0;
    }
    DEBUG_PRINT (Returning assignment);
    return node; 
}


Node* Get_Cond (char *&input, int func_num)
{
    kill_spaces (input);
    if (*input == ',')
        return nullptr;

    Node* node = Get_E (input, func_num);

    DEBUG_PRINT (Got E in Get_cond);

    Node* new_node = nullptr;
    kill_spaces (input);

    char letters[BUFFERSIZE] = {};
    int read_count = 0;

    sscanf (input, "%[^ \n\t\r.,=+0-9:;^+-*/]%n", letters, &read_count);

    if ( ! strcmp (letters, EQUAL) || ! strcmp (letters, NOT_EQUAL) || ! strcmp (letters, LESS) || ! strcmp (letters, MORE) )
    {
        DEBUG_PRINT (Condition was detected);
        new_node = new Node;

        new_node -> node_data.type = OPERATOR;
        new_node -> node_data.data.code = get_operator_code (letters);

        new_node -> left = node;
        node -> father = new_node;
        node = new_node;
        new_node = nullptr;

        input += read_count;

        node -> right = Get_E (input, func_num);
        node -> right -> father = node;
    }

    DEBUG_PRINT (Returning get cond);

    return node;
}

Node* Get_E (char *&input, int func_num)
{
    kill_spaces (input);
    if (*input == ',')
        return nullptr;

    Node* node = Get_T (input, func_num);

    DEBUG_PRINT (Got T);

    Node* new_node = nullptr;
    kill_spaces (input);

    char letters[BUFFERSIZE] = {};
    int read_count = 0;

    sscanf (input, "%[^ \n\t\r.,=+0-9:;^+-*/]%n", letters, &read_count);

    while ( ! strcmp (letters, ADD) || ! strcmp (letters, SUB) )
    {

        DEBUG_PRINT (ADD or SUB detected);

        new_node = new Node;

        new_node -> node_data.type = OPERATOR;
        new_node -> node_data.data.code = get_operator_code (letters);

        new_node -> left = node;
        node -> father = new_node;
        node = new_node;
        new_node = nullptr;

        input += read_count;

        node -> right = Get_T (input, func_num);
        node -> right -> father = node;

        letters[0] = 0;
    }
    return node;
}

Node* Get_T (char *&input, int func_num)
{
    kill_spaces (input);
    Node* node = Get_K (input, func_num);

    DEBUG_PRINT (Got K);

    Node* new_node = nullptr;
    kill_spaces (input);

    char letters[BUFFERSIZE] = {};
    int read_count = 0;

    sscanf (input, "%[^ \n\t\r.,=+0-9:;^+-*/]%n", letters, &read_count);

    while ( ! strcmp (letters, MUL) || ! strcmp (letters, DIV) )
    {
        new_node = new Node;

        new_node -> node_data.type = OPERATOR;
        new_node -> node_data.data.code = get_operator_code (letters);

        new_node -> left = node;
        node -> father = new_node;
        node = new_node;
        new_node = nullptr;

        input += read_count;

        node -> right = Get_K (input, func_num);
        node -> right -> father = node;

        letters[0] = 0;
    }
    return node;     
}

Node* Get_K (char *&input, int func_num)
{
    kill_spaces (input);
    Node* node = Get_P (input, func_num);

    DEBUG_PRINT (Got P);

    Node* new_node = nullptr;
    kill_spaces (input);

    DEBUG_CODE ( printf ("input in K: %s", input) );

    char letters[BUFFERSIZE] = {};
    int read_count = 0;

    sscanf (input, "%[^ \n\t\r.,=+0-9:;^+-*/]%n", letters, &read_count);

    while ( ! strcmp (letters, POW) )
    {
        new_node = new Node;

        new_node -> node_data.type = OPERATOR;
        new_node -> node_data.data.code = get_operator_code (letters);

        new_node -> left = node;
        node -> father = new_node;
        node = new_node;
        new_node = nullptr;

        input += read_count;

        node -> right = Get_P (input, func_num);
        node -> right -> father = node;

        letters[0] = 0;
    }
    return node;     
}

Node* Get_P (char *&input, int func_num)
{
    kill_spaces (input);
    DEBUG_CODE ( printf ("Input in P: %s\n", input) );

    if ( *input == '(' )
    {
        input++;
        Node* node = Get_E (input, func_num);
        if ( *input == ')' )
        {
            input++;
        }
        else
        {
            assert (false);
            return nullptr;
        }
        return node;
    }
    else if ( (*input >= '0' && *input <= '9') || (*input == '-') )
    {
        DEBUG_PRINT (Gonna get N);
        return Get_N (input);
    }
    else
    {
        DEBUG_PRINT (Gonna get F);
        return Get_F (input, func_num);
    }    
}

Node* Get_F (char *&input, int func_num)
{
    kill_spaces (input);
    char letters[BUFFERSIZE] = {};
    int read_count = 0;
    Node *new_node = new Node;
    Node *expression = nullptr;

    kill_spaces (input);
    if ( ! sscanf (input, "%[^ \n\t\r.,=+0-9:;^+-*/]%n", letters, &read_count) )
    {
        assert (false);                      
    }
    input += read_count;

    kill_spaces (input);
    
    DEBUG_CODE ( printf ("letters in F: %s\n", letters) );

    if ( is_un_function (letters) ) 
    {
        DEBUG_PRINT (Unary function detected);

            expression = Get_E (input, func_num);

        new_node -> node_data.type = UN_FUNCTION;
        new_node -> node_data.data.code = get_un_function_code (letters);
        new_node -> right = expression;
        expression -> father = new_node;

        return new_node;
    }
    else if ( is_def (letters) )
    {
        DEBUG_PRINT (DEF detected);

        new_node -> node_data.type = CALL;
        new_node -> node_data.data.code = get_def_num (letters);

        new_node -> right = GetArgs (input, func_num);
        new_node -> right -> father = new_node;

        return new_node;
    }
    else 
    {
        if (func_num == -1)
        {
            if ( is_variable (letters) )
            {
                DEBUG_PRINT (Variable detected);

                new_node -> node_data.type = VARIABLE;
                new_node -> node_data.data.code = get_variable_num (letters);
                
                return new_node;
            }
        }
        else
        {
            if ( is_argument (letters, func_num) )
            {
                DEBUG_PRINT (Argument detected);

                new_node -> node_data.type = ARGUMENT;
                new_node -> node_data.data.code = get_argument_num (letters, func_num);
                
                return new_node;
            }
        }
    }



    DEBUG_PRINT (Cannot detect variable or un_function);   //ПРОДОЛЖИТЬ
    
    delete new_node;
    std::cout<<LINE_COUNTER<<std::endl;
    std::cout<<input;

    throw "CANNOT DETECT FUNCTION OR VARIABLE";

}

Node* Get_N (char *&input)
{
    kill_spaces (input);
    float value = 0.0;
    int read_count = 0;
    if ( ! sscanf (input, "%f%n", &value, &read_count) )
    {
        assert (false);
    }
    input += read_count;
    Node *new_node = new Node;

    DEBUG_CODE (printf ("Input after get N: %s\n", input) );

    new_node -> node_data.data.code = QUANTITY;
    new_node -> node_data.data.value = value;
    return new_node;
}


Node* GetArgs (char *&input, int func_num)
{
    Node* main_node = new Node;

    main_node->node_data.type = BLOCK;

    Node* curr_node = main_node;

    kill_spaces (input);
    
    if (*input == ',')
        return nullptr;
    
    if ( ! check (input, ':') )
        throw "Неопознанная инструкция";

    kill_spaces (input);

    if ( ! check (input, "кДоске") )
        throw "Неопознанная инструкция";

    for ( kill_spaces (input) ; *input != ','; kill_spaces (input) )
    {
        curr_node -> left = Get_E (input, func_num);
        curr_node -> left -> father = curr_node;

        kill_spaces (input);

        if ( check (input, '/') )
        {
            input++;
            curr_node -> right = new Node;
            curr_node -> right -> father = curr_node;
            curr_node = curr_node -> right;
            curr_node -> node_data.type = BLOCK;
            continue;
        }
        else if (*input == ',')
            break;

        std::cout<<input<<'\n';

        throw "Ожидалась запятая в конце инструкции";

    }
    return main_node;
}


FILE* open_file(const char* input_file_name)
{
    FILE* stream = NULL;
    if (!(stream = fopen(input_file_name, "r")))
    {
        fprintf(stderr, "Input file is not open\n");
        return nullptr;
    }
        return stream;
}

long size_of_file (FILE* stream)
{
    assert ( stream );

    fseek ( stream, 0, SEEK_END);
    const long filesize = ftell (stream);
    fseek ( stream, 0, SEEK_SET);
    return filesize;
}

void kill_spaces (char* &input)
{
    while ( isspace (input[0]) )
    {   
        if (*input == '\n')
            LINE_COUNTER++;

        input++;
    }
}




bool is_variable (char* potentional_var)
{
    DEBUG_PRINT (VAR COMPARSION...)
    for (int i = 0; i < VAR_NUMBER && variables[i]; i++)
        if ( ! strcmp (potentional_var, variables[i]) )
            return true;

    return false;
}

int get_variable_num (char* var)
{
    for (int i = 0; i < VAR_NUMBER && variables[i]; i++)
        if ( ! strcmp (var, variables[i]) )
            return i;

    assert (false);
}

bool is_def (char* potentional_def)
{
    DEBUG_PRINT (DEF COMPARSION...)
    for (int i = 0; i < DEF_NUMBER && def[i]; i++)
        if ( ! strcmp (potentional_def, def[i]) )
            return true;

    return false;
}

int get_def_num (char* nummed_def)
{
    for (int i = 0; i < DEF_NUMBER && def[i]; i++)
        if ( ! strcmp (nummed_def, def[i]) )
            return i;

    assert (false);
}

bool is_argument (char* potentional_arg, int func_num)
{
    DEBUG_PRINT (ARG COMPARSION...)
    for (int i = 0; i < ARG_NUMBER && DEF_ARGUMENTS[func_num][i]; i++)
        if ( ! strcmp (potentional_arg, DEF_ARGUMENTS[func_num][i]) )
            return true;

    return false;
}

int get_argument_num (char* arg, int func_num)
{
    for (int i = 0; i < ARG_NUMBER && DEF_ARGUMENTS[func_num][i]; i++)
        if ( ! strcmp (arg, DEF_ARGUMENTS[func_num][i]) )
            return i;

    assert (false);
}

#endif