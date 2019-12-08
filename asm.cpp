/// @file
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include "text_file_worker.hpp"

///@brief Path to file with ASM code
static const char* INPUT_FILE = "asm_code.txt";

///@brief Path to file for writing binary code
static const char* OUTPUT_FILE = "code.bin";

///@brief Max size of command (so uses for labels)
static const int MAX_COMMAND_SIZE = 15;

///@brief Max size of register (so uses for labels)
static const int MAX_REG_SIZE = 15;

///@brief Max size of label
static const int MAX_LABEL_LENGTH = 20;

///@brief Max number of labels
static const int MAX_LABEL_NUM = 500;

///@brief Translates ASM code to int buffer containing binary code
int* cmd_into_buf ( const file_info &command_file);

///@brief Writes buffer with binary code to the file
bool write_cmd (int* cmd_buf, const file_info &input_cmd);

///@brief Special string comporator for command determinating
int cmdcmp (const char* string1, const char* string2);

///@brief Determinates labels by ':'
bool islabel (char* string);

///@brief Searching for label's address (uses in jumps)
bool label_search (char* string, int &label_num);

///@brief Label-struct
struct label
{
///@brief Name of label
    char name[MAX_LABEL_LENGTH] = "";

///@brief Address which label points to
    long address = 0;
};

///@brief Global array of labels
label LABELS [MAX_LABEL_NUM] = {};

///@brief Counter of labels
long label_counter = 0;


///@brief Define DEBUG to execute debug code
//#define DEBUG

#ifdef DEBUG
    #define DEBUG_CODE(code) code;
#else
    #define DEBUG_CODE(code)
#endif


#define $assert(cond, code)                                                                  \
    if (!cond)                                                                               \
    {                                                                                        \
        fprintf (stderr, "something is not OK in %d string\n",  __LINE__);                     \
        code;                                                                                \
    }


int main ()
{
    const file_info input_cmd = file_worker(INPUT_FILE);

    int* cmd_buf = cmd_into_buf (input_cmd) ;
    $assert (cmd_buf, return 1);
    cmd_buf = cmd_into_buf (input_cmd) ;
    $assert (cmd_buf, return 1);
    write_cmd (cmd_buf, input_cmd);

    return 0;
}

int* cmd_into_buf ( const file_info &input_cmd)
{
    int* cmd_buf = (int*) calloc (input_cmd.number_of_strings,  2*sizeof (int));
    $assert(cmd_buf, return nullptr);

    char command_name[MAX_COMMAND_SIZE] = "";
    int command_code = 0;
    float value = 0;
    char reg[MAX_REG_SIZE] = "";
    int label_num = 0;

    #define DEF_CMD(name, num, argc, code)                       \
        else if (cmdcmp (#name, command_name) == 0)              \
        cmd_buf[2*cmd_counter] = num;

    for (long cmd_counter = 0; cmd_counter < input_cmd.number_of_strings; cmd_counter++)
    {
        reg[0] = 0;
        if ( input_cmd.stringpointer[cmd_counter].b_ptr[0] == '\0') continue;
        sscanf (input_cmd.stringpointer[cmd_counter].b_ptr, " %s", command_name);
        if (command_name[0] == ';') continue;
        if ( islabel (command_name) )
        {
            DEBUG_CODE ( printf ("Label detected: %s\n", command_name) )
            strcpy (LABELS [label_counter].name, command_name);
            LABELS [label_counter++].address = 2*(cmd_counter+1)*100;
            continue;
        }
        if (sscanf (input_cmd.stringpointer[cmd_counter].b_ptr, " %*[A-Za-z]%f", &value))
        {
            cmd_buf[2*cmd_counter+1] = value*100;
            value = 0;
        }
        else if (sscanf (input_cmd.stringpointer[cmd_counter].b_ptr, " %*[A-Za-z] %s", reg))
        {
            if (tolower (reg[1]) == 'x' && reg[2] == 0)
            {
                strcat (command_name, " ");
                strcat (command_name, reg);
                strcat (command_name, "\0");
                cmd_buf[2*cmd_counter+1] = (tolower (reg[0]) - 'a')*100;
            }
            else if (label_search (reg, label_num))
            {
                DEBUG_CODE ( printf ("Label set: %s\n", reg) )

                cmd_buf[2*cmd_counter+1] = LABELS [label_num].address;
                label_num = 0;
            }
            else
            {
                DEBUG_CODE ( printf ("Label wasn't found: %s\n", reg) )

                cmd_buf[2*cmd_counter+1] = -1;
            }

        }
        DEBUG_CODE ( printf ("Command [%ld]: %s\n", cmd_counter + 1, command_name) )

        if (false) ;

        #include "commands.hpp"

        else
        {
			fprintf (stderr, "\nWRONG COMMAND!\n"
                             "String of wrong command in file: %ld\n\n", cmd_counter+1);
            return nullptr;
        }
    }

    #undef DEF_CMD
    return cmd_buf;
}

bool write_cmd(int* cmd_buf, const file_info &input_cmd)
{
    assert (cmd_buf);

    FILE* output_cmd = fopen (OUTPUT_FILE, "wb");

    $assert(output_cmd, return false);

    fwrite (cmd_buf, 2*sizeof(int), input_cmd.number_of_strings, output_cmd);
    fclose(output_cmd);
    return 1;
}

bool islabel (char* string)
{
    int i = 0;
    for (i = 0; string [i]; i++)
        ;
    if (string[i-1] == ':')
    {
        string[i-1] = '\0';
        return true;
    }
    return false;
}

int cmdcmp (const char* string1, const char* string2)
{
    assert (string1);
    assert (string2);

    int i = 0;
    int j = 0;
    while (string1 [i] && string2 [j])
    {
        if (string1 [i] == '_')
        {
            i++;
            j++;
            continue;
        }
        else
            if ( lowercase_letter (string1 [i]) != lowercase_letter (string2 [j]) )
                return lowercase_letter (string1 [i]) - lowercase_letter (string2 [j]);
        i++;
        j++;
    }
    return lowercase_letter (string1 [i]) - lowercase_letter (string2 [j]);
}

bool label_search (char* string, int &label_num)
{
    for (int i = 0; i < label_counter; i++)
        if ( strcmp (LABELS[i].name, string) == 0 )
        {
            label_num = i;
            return true;
        }
    return false;
}
