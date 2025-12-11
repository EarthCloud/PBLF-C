#ifndef TODO_H
#define TODO_H

#include "common.h"

// 定义最大待办数和内容长度
#define MAX_TODOS 50
#define TODO_CONTENT_LEN 100

typedef struct {
    int id;
    char content[TODO_CONTENT_LEN];
    int is_completed;
} TodoItem;

// 初始化
void todo_init();

// 添加待办 (如果 content 为空，则内部交互读取)
int todo_add(const char* content);

// 列出待办 (结果写入 output)
void todo_list(char* output, int max_len, const char* filter);

// 完成待办
int todo_complete(int id);

// 删除待办
int todo_delete(int id);

#endif