#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "game.h"
#define WIDTH 60
#define HEIGHT 30
#define INIT_SNAKE_LEN 3
#define MAX_LEN 100  // 表达式最大长度
#define MAX_STACK 50 // 栈最大容量
#define N 9

void play_games(){
    int a;
    printf("欢迎来到游戏合集！请选择您想玩的游戏：\n");
    printf("1. 贪吃蛇游戏\n");
    printf("2. 数独游戏\n");
    printf("3. 计算器\n");
    printf("请输入游戏编号（1-3）：");
    scanf("%d", &a);
    if (a == 1){
        game_snake();
    }
    else if (a == 2){
        game_sudoku();
    }
    else if (a == 3){
        calculator();
    }
    else{
        printf("无效的选择，请重新运行程序并选择正确的游戏编号。\n");
    }
}

bool is_safe(int grid[N][N], int row, int col, int num) {
    for (int x = 0; x < N; x++) {
        if (grid[row][x] == num) return false; // 检查行
        if (grid[x][col] == num) return false; // 检查列
    }
    int startRow = row - row % 3;
    int startCol = col - col % 3;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (grid[i + startRow][j + startCol] == num)
                return false; // 检查3x3宫格
        }
    }
    return true;
}

// 在未填数字的格子中寻找解
bool solve_sudoku(int grid[N][N]) {
    int row = -1, col = -1;
    bool empty_found = false;

    // 找到一个空格（值为0）
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (grid[i][j] == 0) {
                row = i;
                col = j;
                empty_found = true;
                break;
            }
        }
        if (empty_found) break;
    }

    if (!empty_found)
        return true; // 已填满

    for (int num = 1; num <= 9; num++) {
        if (is_safe(grid, row, col, num)) {
            grid[row][col] = num;
            if (solve_sudoku(grid))
                return true;
            grid[row][col] = 0; // 回溯
        }
    }
    return false;
}

// 打印数独网格
void print_grid(int grid[N][N]) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (grid[i][j] == 0)
                printf(". ");
            else
                printf("%d ", grid[i][j]);
            if ((j + 1) % 3 == 0 && j != N - 1)
                printf("| ");
        }
        printf("\n");
        if ((i + 1) % 3 == 0 && i != N - 1)
            printf("------+-------+------\n");
    }
}

// 从标准输入读取9x9数独
void read_grid(int grid[N][N]) {
    printf("请输入数独（9行，每行9个元素，用空格隔开，用'.'表示空格）：\n");
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            char ch;
            scanf(" %c", &ch); // 读取一个字符（跳过空格）
            if (ch == '.') {
                grid[i][j] = 0;
            } else if (ch >= '1' && ch <= '9') {
                grid[i][j] = ch - '0';
            } else {
                printf("输入无效，请输入数字1-9或'.'。\n");
                j--; // 重读该位置
            }
        }
    }
}

// 主函数
void game_sudoku() {
    int grid[N][N];

    read_grid(grid);

    printf("\n原始数独:\n");
    print_grid(grid);

    if (solve_sudoku(grid)) {
        printf("\n求解成功:\n");
        print_grid(grid);
    } else {
        printf("\n无解。\n");
    }

}
void initStack(Stack *stack) {
    stack->top = -1;
}

// 判断栈空
int isEmpty(Stack *stack) {
    return stack->top == -1;
}

// 判断栈满
int isFull(Stack *stack) {
    return stack->top == MAX_STACK - 1;
}

// 入栈（数值栈）
void pushNum(Stack *stack, double num) {
    if (isFull(stack)) {
        printf("错误：栈溢出！\n");
        exit(1);
    }
    stack->data[++stack->top] = num;
}

// 入栈（运算符栈）
void pushOp(Stack *stack, char op) {
    if (isFull(stack)) {
        printf("错误：栈溢出！\n");
        exit(1);
    }
    stack->data[++stack->top] = op;
}

// 出栈（数值栈）
double popNum(Stack *stack) {
    if (isEmpty(stack)) {
        printf("错误：表达式格式错误（栈空）！\n");
        exit(1);
    }
    return stack->data[stack->top--];
}

// 出栈（运算符栈）
char popOp(Stack *stack) {
    if (isEmpty(stack)) {
        printf("错误：表达式格式错误（栈空）！\n");
        exit(1);
    }
    return (char)stack->data[stack->top--];
}

// 获取栈顶元素（运算符栈）
char getTopOp(Stack *stack) {
    if (isEmpty(stack)) {
        return '\0'; // 栈空返回空字符
    }
    return (char)stack->data[stack->top];
}

// 判断是否为运算符
int isOperator(char c) {
    return c == '+' || c == '-' || c == '*' || c == '/';
}

// 获取运算符优先级（括号最低，*、/ 高于 +、-）
int getPriority(char op) {
    if (op == '(' || op == ')') {
        return 0;
    } else if (op == '+' || op == '-') {
        return 1;
    } else if (op == '*' || op == '/') {
        return 2;
    }
    return -1;
}

// 计算两个数的运算结果
double calculate(double num1, double num2, char op) {
    switch (op) {
        case '+': return num1 + num2;
        case '-': return num1 - num2;
        case '*': return num1 * num2;
        case '/':
            if (num2 == 0) {
                printf("错误：除数不能为0！\n");
                exit(1);
            }
            return num1 / num2;
        default:
            printf("错误：不支持的运算符 '%c'！\n", op);
            exit(1);
    }
}

// 中缀表达式转后缀表达式（逆波兰式）并计算结果
double evaluateExpression(char *expr) {
    Stack opStack;    // 运算符栈
    Stack numStack;   // 数值栈
    initStack(&opStack);
    initStack(&numStack);

    int i = 0;
    int len = strlen(expr);
    while (i < len) {
        // 跳过空格
        if (isspace(expr[i])) {
            i++;
            continue;
        }

        // 读取数字（支持整数和浮点数）
        if (isdigit(expr[i]) || expr[i] == '.') {
            double num = 0.0;
            int decimal = 0; // 标记是否进入小数部分
            double divisor = 10.0;

            // 读取整数部分
            while (i < len && isdigit(expr[i])) {
                num = num * 10 + (expr[i] - '0');
                i++;
            }

            // 读取小数部分
            if (i < len && expr[i] == '.') {
                decimal = 1;
                i++;
                while (i < len && isdigit(expr[i])) {
                    if (decimal) {
                        num += (expr[i] - '0') / divisor;
                        divisor *= 10;
                    }
                    i++;
                }
            }

            pushNum(&numStack, num); // 数字直接入数值栈
        }
        // 读取左括号：直接入栈
        else if (expr[i] == '(') {
            pushOp(&opStack, expr[i]);
            i++;
        }
        // 读取右括号：弹出运算符栈直到遇到左括号
        else if (expr[i] == ')') {
            while (!isEmpty(&opStack) && getTopOp(&opStack) != '(') {
                char op = popOp(&opStack);
                double num2 = popNum(&numStack);
                double num1 = popNum(&numStack);
                double res = calculate(num1, num2, op);
                pushNum(&numStack, res);
            }
            if (isEmpty(&opStack)) { // 未找到匹配的左括号
                printf("错误：括号不匹配（缺少左括号）！\n");
                exit(1);
            }
            popOp(&opStack); // 弹出左括号（不参与计算）
            i++;
        }
        // 读取运算符：按优先级处理
        else if (isOperator(expr[i])) {
            char currentOp = expr[i];
            // 弹出栈中优先级 >= 当前运算符的运算符并计算
            while (!isEmpty(&opStack) && getPriority(getTopOp(&opStack)) >= getPriority(currentOp)) {
                char op = popOp(&opStack);
                double num2 = popNum(&numStack);
                double num1 = popNum(&numStack);
                double res = calculate(num1, num2, op);
                pushNum(&numStack, res);
            }
            pushOp(&opStack, currentOp);
            i++;
        }
        // 非法字符
        else {
            printf("错误：非法字符 '%c'！\n", expr[i]);
            exit(1);
        }
    }

    // 处理栈中剩余的运算符
    while (!isEmpty(&opStack)) {
        char op = popOp(&opStack);
        if (op == '(') { // 未找到匹配的右括号
            printf("错误：括号不匹配（缺少右括号）！\n");
            exit(1);
        }
        double num2 = popNum(&numStack);
        double num1 = popNum(&numStack);
        double res = calculate(num1, num2, op);
        pushNum(&numStack, res);
    }

    // 数值栈最终应只剩一个元素（结果）
    if (numStack.top != 0) {
        printf("错误：表达式格式错误！\n");
        exit(1);
    }

    return popNum(&numStack);
}

void calculator() {
    char expr[MAX_LEN];
    printf("=== 支持括号的多数字四则运算器 ===\n");
    printf("支持功能：整数/浮点数、任意多个数字、括号优先级（+、-、*、/）\n");
    printf("输入格式：例：(123 + 45.6) * 2 - 78 / 3  （数字和运算符间可加空格）\n");
    printf("输入 'q' 退出程序\n\n");

    while (1) {
        printf("请输入表达式：");
        fgets(expr, MAX_LEN, stdin);

        // 去除换行符
        expr[strcspn(expr, "\n")] = '\0';

        // 退出程序
        if (strcmp(expr, "q") == 0) {
            printf("程序退出！\n");
            break;
        }

        // 计算并输出结果（保留2位小数）
        double result = evaluateExpression(expr);
        printf("运算结果：%.2lf\n\n", result);
    }
}

typedef struct {
    int x, y;
} Point;

typedef struct {
    Point body[WIDTH * HEIGHT];
    int size;
    int dx, dy;
} Snake;

typedef struct {
    Point pos;
} Food;

void gotoxy(int x, int y) {
    COORD coord = { (SHORT)x, (SHORT)y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void draw_border() {
    for (int x = 0; x <= WIDTH; x++) {
        gotoxy(x, 0); putchar('#');
        gotoxy(x, HEIGHT); putchar('#');
    }
    for (int y = 0; y <= HEIGHT; y++) {
        gotoxy(0, y); putchar('#');
        gotoxy(WIDTH, y); putchar('#');
    }
}

void init_snake(Snake* snake) {
    snake->size = INIT_SNAKE_LEN;
    for (int i = 0; i < snake->size; i++) {
        snake->body[i].x = WIDTH / 2 - i;
        snake->body[i].y = HEIGHT / 2;
    }
    snake->dx = 1;
    snake->dy = 0;
}

void spawn_food(Food* food, Snake* snake) {
    int valid = 0;
    while (!valid) {
        food->pos.x = rand() % (WIDTH - 2) + 1;
        food->pos.y = rand() % (HEIGHT - 2) + 1;
        valid = 1;
        for (int i = 0; i < snake->size; i++) {
            if (snake->body[i].x == food->pos.x && snake->body[i].y == food->pos.y) {
                valid = 0;
                break;
            }
        }
    }
}

void draw_snake(const Snake* snake) {
    for (int i = 0; i < snake->size; i++) {
        gotoxy(snake->body[i].x, snake->body[i].y);
        putchar(i == 0 ? '@' : 'o');
    }
}

void draw_food(const Food* food) {
    gotoxy(food->pos.x, food->pos.y);
    putchar('$');
}

void clear_snake(const Snake* snake) {
    for (int i = 0; i < snake->size; i++) {
        gotoxy(snake->body[i].x, snake->body[i].y);
        putchar(' ');
    }
}

int check_collision(const Snake* snake) {
    // 撞墙
    if (snake->body[0].x <= 0 || snake->body[0].x >= WIDTH ||
        snake->body[0].y <= 0 || snake->body[0].y >= HEIGHT)
        return 1;
    // 撞自己
    for (int i = 1; i < snake->size; i++) {
        if (snake->body[0].x == snake->body[i].x &&
            snake->body[0].y == snake->body[i].y)
            return 1;
    }
    return 0;
}

void move_snake(Snake* snake) {
    for (int i = snake->size - 1; i > 0; i--)
        snake->body[i] = snake->body[i - 1];
    snake->body[0].x += snake->dx;
    snake->body[0].y += snake->dy;
}

void game_snake() {
    Snake snake;
    Food food;
    int score = 0;
    srand((unsigned)time(NULL));
    system("cls");
    draw_border();
    init_snake(&snake);
    spawn_food(&food, &snake);
    draw_food(&food);
    draw_snake(&snake);

    while (1) {
        // 控制方向
        if (_kbhit()) {
            char ch = _getch();
            if ((ch == 'w' || ch == 72) && snake.dy != 1) { snake.dx = 0; snake.dy = -1; }
            else if ((ch == 's' || ch == 80) && snake.dy != -1) { snake.dx = 0; snake.dy = 1; }
            else if ((ch == 'a' || ch == 75) && snake.dx != 1) { snake.dx = -1; snake.dy = 0; }
            else if ((ch == 'd' || ch == 77) && snake.dx != -1) { snake.dx = 1; snake.dy = 0; }
            else if (ch == 27) break; // ESC退出
        }
        clear_snake(&snake);
        move_snake(&snake);

        // 吃食物
        if (snake.body[0].x == food.pos.x && snake.body[0].y == food.pos.y) {
            snake.size++;
            snake.body[snake.size - 1] = snake.body[snake.size - 2];
            score++;
            spawn_food(&food, &snake);
        }

        // 撞墙或撞自己
        if (check_collision(&snake)) {
            gotoxy(WIDTH / 2 - 5, HEIGHT / 2);
            printf("Game Over! Score: %d", score);
            break;
        }

        draw_food(&food);
        draw_snake(&snake);

        gotoxy(0, HEIGHT + 1);
        printf("Score: %d   [WASD/方向键控制, ESC退出]", score);

        Sleep(100); // 控制速度
    }
    gotoxy(0, HEIGHT + 2);
    system("pause");
}

    













