#ifndef LLM_H
#define LLM_H

#include "common.h"

// 简化的 LLM 配置
typedef struct {
    char hostname[64];
    int port;
} LLMModel;

// 意图枚举 (根据 data/patterns.txt 重构)
typedef enum {
    INTENT_UNKNOWN,

    // 待办事项 (TODO)
    INTENT_TODO_ADD,        // 添加待办
    INTENT_TODO_LIST,       // 列出/搜索待办
    INTENT_TODO_COMPLETE,   // 完成待办 (参数: ID)
    INTENT_TODO_DELETE,     // 删除待办 (参数: ID)

    // 备忘录 (MEMO)
    INTENT_MEMO_LIST,       // 列出/搜索备忘
    INTENT_MEMO_ADD,        // 新建备忘 (参数: 标题)
    INTENT_MEMO_READ,       // 读取备忘 (参数: 标题)
    INTENT_MEMO_DELETE,     // 删除备忘 (参数: 标题)

    // 联系人 (CONTACT)
    INTENT_CONTACT_LIST,    // 列出/搜索联系人
    INTENT_CONTACT_ADD,     // 新建联系人 (参数: 姓名)
    INTENT_CONTACT_DELETE,  // 删除联系人 (参数: 姓名)

    // 其他
    INTENT_GAME_START,      // 启动游戏
    INTENT_CHAT,            // 闲聊
    INTENT_EXIT             // 退出程序 (系统级)
} IntentType;

// 意图结果
typedef struct {
    IntentType type;
    char parameter[MAX_INPUT];
} Intent;

// 初始化
void llm_init(LLMModel *model, const char *hostname, int port);

// 核心功能
Intent llm_analyze_intent(LLMModel *model, const char *user_input);
char *llm_chat_generate(LLMModel *model, const char *user_input);
//int llm_extract_keywords(LLMModel *model, const char *text, char **keywords, int max_keywords);

#endif