#define WIDE 60
#define HIGH 30
#define WIDTH 60
#define HEIGHT 30
#define INIT_SNAKE_LEN 3
#define MAX_LEN 100  // 表达式最大长度
#define MAX_STACK 50 // 栈最大容量
#define N 9


struct BODY {
    int x;
    int y;
};


void play_games();

void calculator();

void game_snake();

void game_sudoku();

typedef struct {
    double data[MAX_STACK];
    int top;
} Stack;

double calculate(double num1, double num2, char op);

double evaluateExpression(char *expr);

void initStack(Stack *stack);

int isEmpty(Stack *stack);

int isFull(Stack *stack);

void pushNum(Stack *stack, double num);

void pushOp(Stack *stack, char op);

double popNum(Stack *stack);

char popOp(Stack *stack);

char getTopOp(Stack *stack);

int getPriority(char op);

int isOperator(char ch);