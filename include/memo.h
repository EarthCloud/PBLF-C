#ifndef MEMO_H
#define MEMO_H

#include "common.h"

#define MAX_MEMOS 100
#define MEMO_TITLE_LEN 50
#define MEMO_CONTENT_LEN 200
#define MEMO_FILE "data/memos.txt" // 数据文件路径

typedef struct {
    int id;
    char title[MEMO_TITLE_LEN];
    char content[MEMO_CONTENT_LEN];
    int is_deleted; // 软删除标记，或者直接覆盖
} Memo;

// 初始化（加载数据）
int memo_init();

// 保存数据
void memo_save();

// 添加备忘 (返回新ID)
int memo_add(const char* title);

// 删除备忘 (返回 1 成功, 0 失败)
int memo_delete(int id);

// 删除备忘 (根据标题删除，用于 LLM 传回标题的情况)
int memo_delete_by_title(const char* title);

// 获取列表字符串 (将结果写入 output 缓冲区)
void memo_list(char* output, int max_len);

// 搜索备忘 (将结果写入 output)
void memo_search(const char* keyword, char* output, int max_len);

// 根据标题读取内容
void memo_read_by_title(const char* title, char* output, int max_len);

#endif