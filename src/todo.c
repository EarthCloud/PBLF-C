#include "todo.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static TodoItem g_todos[MAX_TODOS];
static int g_count = 0;

void todo_init() {
    g_count = 0;
    memset(g_todos, 0, sizeof(g_todos));
}

int todo_add(const char* content) {
    if (g_count >= MAX_TODOS) return -1;

    TodoItem *t = &g_todos[g_count];

    // 生成 ID (简单的自增逻辑)
    int max_id = 0;
    for(int i=0; i<g_count; i++) {
        if(g_todos[i].id > max_id) max_id = g_todos[i].id;
    }
    t->id = max_id + 1;
    t->is_completed = 0;

    // 交互逻辑：如果参数为空，询问用户
    if (content && strlen(content) > 0) {
        strncpy(t->content, content, TODO_CONTENT_LEN - 1);
    } else {
        printf(">> 请输入待办内容: ");
        if (fgets(t->content, TODO_CONTENT_LEN, stdin)) {
            t->content[strcspn(t->content, "\n")] = 0;
        }
    }

    g_count++;
    // 移除了 save 操作
    return t->id;
}

void todo_list(char* output, int max_len, const char* filter) {
    if (g_count == 0) {
        snprintf(output, max_len, "待办清单是空的。");
        return;
    }

    output[0] = '\0';
    char line[256];
    int found_count = 0;

    strncat(output, "=== 待办清单 ===\n", max_len - 1);

    for (int i = 0; i < g_count; i++) {
        // 如果有过滤词且不匹配，跳过
        if (filter && strlen(filter) > 0 && !strstr(g_todos[i].content, filter)) {
            continue;
        }

        snprintf(line, sizeof(line), "[%c] ID:%d %s\n",
            g_todos[i].is_completed ? 'x' : ' ',
            g_todos[i].id,
            g_todos[i].content);

        if (strlen(output) + strlen(line) < max_len) {
            strcat(output, line);
            found_count++;
        }
    }

    if (found_count == 0 && filter) {
        snprintf(output, max_len, "没有找到包含 '%s' 的待办。", filter);
    }
}

int todo_complete(int id) {
    for (int i = 0; i < g_count; i++) {
        if (g_todos[i].id == id) {
            g_todos[i].is_completed = 1;
            return 1;
        }
    }
    return 0;
}

int todo_delete(int id) {
    for (int i = 0; i < g_count; i++) {
        if (g_todos[i].id == id) {
            // 移动数组元素覆盖删除
            for (int j = i; j < g_count - 1; j++) {
                g_todos[j] = g_todos[j + 1];
            }
            g_count--;
            return 1;
        }
    }
    return 0;
}