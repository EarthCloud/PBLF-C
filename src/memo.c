#include "memo.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static Memo g_memos[MAX_MEMOS];
static int g_count = 0;

int memo_init() {
    FILE *fp = fopen(MEMO_FILE, "r");
    if (!fp) return 0;

    g_count = 0;
    while (g_count < MAX_MEMOS && !feof(fp)) {
        Memo *m = &g_memos[g_count];
        // 简单的文本格式：ID|标题|内容
        if (fscanf(fp, "%d|%49[^|]|%199[^\n]\n", &m->id, m->title, m->content) == 3) {
            g_count++;
        }
    }
    fclose(fp);
    return 1;
}

void memo_save() {
    FILE *fp = fopen(MEMO_FILE, "w");
    if (!fp) return;

    for (int i = 0; i < g_count; i++) {
        fprintf(fp, "%d|%s|%s\n", g_memos[i].id, g_memos[i].title, g_memos[i].content);
    }
    fclose(fp);
}

int memo_add(const char* title) {
    if (g_count >= MAX_MEMOS) return -1;

    Memo *m = &g_memos[g_count];

    // 生成 ID
    int max_id = 0;
    for(int i=0; i<g_count; i++) {
        if(g_memos[i].id > max_id) max_id = g_memos[i].id;
    }
    m->id = max_id + 1;

    // 设置标题
    strncpy(m->title, title, MEMO_TITLE_LEN - 1);
    m->title[MEMO_TITLE_LEN - 1] = '\0'; // 确保截断安全

    // [新增] 交互式读取内容
    printf(">> 请输入备忘录内容: ");
    if (fgets(m->content, MEMO_CONTENT_LEN, stdin)) {
        // 去除末尾换行符
        m->content[strcspn(m->content, "\n")] = 0;
    } else {
        m->content[0] = '\0';
    }

    g_count++;
    memo_save(); // 保存到文件
    return m->id;
}

int memo_delete(int id) {
    int index = -1;
    for (int i = 0; i < g_count; i++) {
        if (g_memos[i].id == id) {
            index = i;
            break;
        }
    }

    if (index != -1) {
        // 移动数组填补空缺
        for (int i = index; i < g_count - 1; i++) {
            g_memos[i] = g_memos[i+1];
        }
        g_count--;
        memo_save();
        return 1;
    }
    return 0;
}

int memo_delete_by_title(const char* title) {
    // 简单的按标题删除，如果有重名只删第一个
    for (int i = 0; i < g_count; i++) {
        if (strstr(title,g_memos[i].title ) != NULL) { // 支持模糊匹配
            return memo_delete(g_memos[i].id);
        }
    }
    return 0;
}

void memo_list(char* output, int max_len) {
    if (g_count == 0) {
        snprintf(output, max_len, "备忘录是空的。");
        return;
    }

    output[0] = '\0';
    char line[256];
    strncat(output, "=== 备忘录列表 ===\n", max_len - strlen(output) - 1);

    for (int i = 0; i < g_count; i++) {
        snprintf(line, sizeof(line), "[ID:%d] %s\n", g_memos[i].id, g_memos[i].title);
        if (strlen(output) + strlen(line) < max_len) {
            strcat(output, line);
        } else {
            break;
        }
    }
}

void memo_search(const char* keyword, char* output, int max_len) {
    output[0] = '\0';
    char line[256];
    int found = 0;

    for (int i = 0; i < g_count; i++) {
        if (strstr(g_memos[i].title, keyword) || strstr(g_memos[i].content, keyword)) {
            snprintf(line, sizeof(line), "[ID:%d] %s: %s\n", g_memos[i].id, g_memos[i].title, g_memos[i].content);
            if (strlen(output) + strlen(line) < max_len) {
                strcat(output, line);
                found++;
            }
        }
    }

    if (found == 0) {
        snprintf(output, max_len, "未找到包含 '%s' 的备忘。", keyword);
    }
}

void memo_read_by_title(const char* title, char* output, int max_len) {
    for (int i = 0; i < g_count; i++) {
        if (strstr(g_memos[i].title, title) != NULL) {
            snprintf(output, max_len, "【%s】\n%s", g_memos[i].title, g_memos[i].content);
            return;
        }
    }
    snprintf(output, max_len, "未找到标题包含 '%s' 的备忘。", title);
}