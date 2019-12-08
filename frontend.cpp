///@file

#define DEBUG

#include "frontend.hpp"

#include "differ-tree.hpp"

#include "backend.hpp"

#include <ctime>

#include <stdexcept>


#ifdef DEBUG

    #define DEBUG_CODE(code) code;
    #define DEBUG_PRINT(print) printf ("\\\\debug: %s\n", #print);
#else
    #define DEBUG_CODE(code)
    #define DEBUG_PRINT(print) 
#endif

typedef CalcTree::Node_t Node;

class COMPILATOR_ERROR
{
    char* error;
    int line_number;

public:

    COMPILATOR_ERROR (const char* error, int line_number)
    {
        this -> error = strdup (error);
        this -> line_number = line_number;
    }
};


int main ()
{
    setlocale (LC_ALL, "rus");

    FILE* stream = open_file (INPUT_PROG);
    assert (stream);

    long file_size = size_of_file (stream);
    char* input = (char*)calloc(file_size + 2, sizeof(char));
	if (!input)
	{
		printf("Memory can't be allocated\n");
		assert(input);
	}
    file_size = fread (input, sizeof (char), file_size, stream);

    CalcTree tree;


    try
    {
        tree.head = Get_G (input);
    }
    catch(const char* error)
    {
        std::cerr << "ОШИБКА КОМПИЛЯЦИИ: " << error << '\n';
        std::cerr << "Строка: " << LINE_COUNTER << '\n';
        return 15;
    }

    try
    {
        tree.draw("open");
        write_to_asm (tree);
    }
    catch(const char* error)
    {
        std::cerr << error << '\n';
        return 1;
    }

    system ("g++ make.cpp -o make");
    system ("clear");
    system ("./make");

    return 0;
}
