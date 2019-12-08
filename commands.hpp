#define protected_act(stack_act)                  \
    err_code = stack_act;                        \
    if (err_code != 0)                            \
    {                                             \
        processor.diagnostic (err_code);          \
        printf("Number of wrong string:%d", cmd_counter/2+1); \
        abort();                                  \
    }

#define RAM_correct(a)                           \
        if (a < 0 || a >= RAM_SIZE)                \
        {                                          \
            printf("You are out of RAM in string:%d", cmd_counter/2+1); \
            abort();                                     \
        }

#define VRAM_correct(a)                            \
        if (a < 0 || a >= VRAM_SIZE)                      \
        {                                                  \
            printf("You are out of VRAM in string:%d", cmd_counter/2+1); \
            abort();                                     \
        }                                                \

DEF_CMD (PUSH, 1, 1,
{
    protected_act ( processor.push(code[cmd_counter+1]) );
})

DEF_CMD (ADD, 2, 0,
{
	int a = 0;
	int b = 0;
	protected_act ( processor.pop (a) );
	protected_act ( processor.pop (b) );
	protected_act ( processor.push (a + b) );
})

DEF_CMD (SUB, 3, 0,
{
	int a = 0;
	int b = 0;
	protected_act ( processor.pop(a) );
	protected_act ( processor.pop(b) );
	protected_act ( processor.push(b - a) );
})

DEF_CMD (MUL, 4, 0,
{
	int a = 0;
	int b = 0;
	protected_act ( processor.pop(a) );
	protected_act ( processor.pop(b) );
	protected_act ( processor.push( (a * b)/100) );
})

DEF_CMD (DIV, 5, 0,
{
	int a = 0;
	int b = 0;
	float c = 0;
	protected_act ( processor.pop(a) );
	protected_act ( processor.pop(b) );
	c = (float) b / a;
	protected_act ( processor.push( c * 100 ) );
})

DEF_CMD (SQRT, 9, 0,
{
	int a = 0;
	protected_act ( processor.pop (a) );
	float b = a/100;
	b = sqrt(b);
	a = b * 100;
	protected_act ( processor.push(a) );
})

DEF_CMD (IN, 10, 0,
{
	float a = 0;
	printf ("Print a value to PUSH: ");
	scanf ("%f", &a);
	protected_act ( processor.push(a*100) );
})

DEF_CMD (OUT, 11, 0,
{
	int a = 0;
	protected_act ( processor.pop (a) );
	printf ("Programm print: " );
	if (a < 0) printf ("-");
    a = abs (a);
    printf ("%d", a/100);
    if (a%100 != 0) printf (".%d", a%100);
    printf ("\n");
})

DEF_CMD (SIN, 15, 0,
{
	int a = 0;
	protected_act ( processor.pop (a) );
	float b = a/100;
	b = sin(b);
	a = b * 100;
	protected_act ( processor.push(a) );
})

DEF_CMD (PUSH _X, 100, 1,
{
    protected_act ( processor.push(r [ code [cmd_counter+1]/100 ] ) );
})

DEF_CMD (POP _X, 110, 1,
{
    protected_act ( processor.pop (r [ code [cmd_counter+1]/100 ] ) );
})

DEF_CMD (POPR, 120, 1,
{
	int a = 0;
	protected_act ( processor.pop(a) );
    int b = code [ cmd_counter + 1 ] / 100;
	RAM_correct (b)
	RAM [ b ] = a;
})

DEF_CMD (POPR _X, 121, 1,
{
	int a = 0;
	protected_act ( processor.pop(a) );
	int b = r [ code [ cmd_counter + 1 ] ] / 100;
	RAM_correct (b)
	RAM [ b ] = a;
})

DEF_CMD (PUSHR, 122, 1,
{
    int b = code [ cmd_counter + 1 ] / 100;
	RAM_correct (b)
	int a = RAM [ b ];
	protected_act ( processor.push(a) );
})

DEF_CMD (PUSHR _X, 123, 1,
{
    int b = r [ code [ cmd_counter + 1 ] ] / 100;
	RAM_correct (b)
	int a = RAM [ b ];
	protected_act ( processor.push(a) );
})

DEF_CMD (POPV, 130, 1,
{
	int a = 0;
	protected_act ( processor.pop(a) );
	int b = code [ cmd_counter + 1 ] / 100;
	VRAM_correct (b)
	VRAM [ b ] = a;
})

DEF_CMD (POPV _X, 131, 1,
{
	int a = 0;
	protected_act ( processor.pop(a) );
	int b = r [ code [ cmd_counter + 1 ] / 100 ] /100;
	VRAM_correct (b)
	VRAM [ b ] = a;
})

DEF_CMD (PUSHV, 132, 1,
{
    int b = code [ cmd_counter + 1 ] / 100;
	VRAM_correct (b)
	int a = VRAM [ b ];
	protected_act ( processor.push(a) );
})

DEF_CMD (PUSHV _X, 133, 1,
{
    int b = r [ code [ cmd_counter + 1 ] / 100] /100;
	VRAM_correct (b)
	int a = VRAM [ b ];
	protected_act ( processor.push(a) );
})

DEF_CMD (JMP, 210, 1,
{
    cmd_counter = code[cmd_counter+1]/100 - 2;
})

DEF_CMD (JA, 220, 1,
{
    int a = 0;
	int b = 0;
	protected_act ( processor.pop(a) );
	protected_act ( processor.pop(b) );
	if (a < b)
    cmd_counter = code[cmd_counter+1]/100 - 2;
})

DEF_CMD (JAE, 221, 1,
{
    int a = 0;
	int b = 0;
	protected_act ( processor.pop(a) );
	protected_act ( processor.pop(b) );
	if (a <= b)
    cmd_counter = code[cmd_counter+1]/100 - 2;
})

DEF_CMD (JB, 222, 1,
{
    int a = 0;
	int b = 0;
	protected_act ( processor.pop(a) );
	protected_act ( processor.pop(b) );
	if (a > b)
    cmd_counter = code[cmd_counter+1]/100 - 2;
})

DEF_CMD (JBE, 223, 1,
{
    int a = 0;
	int b = 0;
	protected_act ( processor.pop(a) );
	protected_act ( processor.pop(b) );
	if (a >= b)
    cmd_counter = code[cmd_counter+1]/100 - 2;
})

DEF_CMD (JE, 224, 1,
{
    int a = 0;
	int b = 0;
	protected_act ( processor.pop(a) );
	protected_act ( processor.pop(b) );
	if (a == b)
    cmd_counter = code[cmd_counter+1]/100 - 2;
})

DEF_CMD (JNE, 225, 1,
{
    int a = 0;
	int b = 0;
	protected_act ( processor.pop(a) );
	protected_act ( processor.pop(b) );
	if (a != b)
    cmd_counter = code[cmd_counter+1]/100 - 2;
})

DEF_CMD (CALL, 230, 1,
{
    protected_act ( call_stack.push(cmd_counter) );
    cmd_counter = code[cmd_counter+1]/100 - 2;
})

DEF_CMD (RET, 231, 1,
{
    protected_act ( call_stack.pop (cmd_counter) );
})

DEF_CMD (END, 255, 0,
{
    goto end;
})
