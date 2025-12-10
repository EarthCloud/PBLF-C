#include "chatbot.h"
#include "user.h"
#include "llm.h"
#include "game.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// 全局实例
static LLMModel g_llm_model;

// 私有声明
static void chatbot_auth_loop(Chatbot *bot);

static void dispatch_intent(Chatbot *bot, Intent intent, char *resp);

// 初始化
int chatbot_init(Chatbot *bot) {
    if (!bot) return 0;

    printf("[System] 正在加载模块...\n");
    if (!user_init()) {
        printf("[Error] 用户模块加载失败\n");
        return 0;
    }

    printf("[System] 正在连接到AI...\n");
    llm_init(&g_llm_model, "127.0.0.1", 8080);

    memset(&bot->current_user, 0, sizeof(User));
    bot->is_running = 1;
    return 1;
}

// 主循环
void chatbot_run(Chatbot *bot) {
    // 登录验证
    chatbot_auth_loop(bot);
    if (!bot->is_running) return;

    printf("\n=== 欢迎 %s ===\n", bot->current_user.username);
    printf("你可以试试说：'添加买菜待办'，'新建日记标题为心情'，或者'玩贪吃蛇'\n\n");

    char input[MAX_INPUT];
    while (bot->is_running) {
        fflush(stdout);
        printf("[%s]> ", bot->current_user.username);
        if (!fgets(input, sizeof(input), stdin)) break;
        // 将换行符置为0
        input[strcspn(input, "\n")] = 0;
        if (strlen(input) == 0) continue;

        char *resp = chatbot_process_input(bot, input);
        if (resp && *resp) printf("[Assistant] %s\n\n", resp);
    }
}

// 核心处理
char *chatbot_process_input(Chatbot *bot, const char *input) {
    // 新建回复的字串
    static char response[MAX_RESPONSE];
    memset(response, 0, sizeof(response));

    // 如果检测到exit系统指令
    if (strcasecmp(input, "exit") == 0) {
        bot->is_running = 0;
        return "Bye!";
    }

    // 新建一个意图文件
    Intent intent = llm_analyze_intent(&g_llm_model, input);
    // 对意图进行分发
    dispatch_intent(bot, intent, response);
    return response;
}

// 意图分发中心
static void dispatch_intent(Chatbot *bot, Intent intent, char *resp) {
    char *param = intent.parameter;

    switch (intent.type) {
        // --- 待办事项模块 ---
        case INTENT_TODO_ADD:
            sprintf(resp, "[TODO] 执行添加待办: %s", param);
            // todo_add(&g_todo_list, param);
            break;
        case INTENT_TODO_LIST:
            sprintf(resp, "[TODO] 执行列出待办，过滤词: %s", param);
            // todo_list(&g_todo_list, param);
            break;
        case INTENT_TODO_COMPLETE:
            sprintf(resp, "[TODO] 执行完成待办 ID: %s", param);
            // todo_complete(&g_todo_list, atoi(param));
            break;
        case INTENT_TODO_DELETE:
            sprintf(resp, "[TODO] 执行删除待办 ID: %s", param);
            // todo_delete(&g_todo_list, atoi(param));
            break;

        // --- 备忘录模块 ---
        case INTENT_MEMO_ADD:
            sprintf(resp, "[MEMO] 新建备忘录，标题: %s\n请在下一行输入内容(模拟)...", param);
            // memo_add(...)
            break;
        case INTENT_MEMO_LIST:
            sprintf(resp, "[MEMO] 查找备忘录: %s", param);
            break;
        case INTENT_MEMO_READ:
            sprintf(resp, "[MEMO] 读取备忘录内容: %s", param);
            break;
        case INTENT_MEMO_DELETE:
            sprintf(resp, "[MEMO] 删除备忘录: %s", param);
            break;

        // --- 联系人模块 ---
        case INTENT_CONTACT_ADD:
            sprintf(resp, "[CONTACT] 新建联系人: %s", param);
            break;
        case INTENT_CONTACT_LIST:
            sprintf(resp, "[CONTACT] 搜索联系人: %s", param);
            break;
        case INTENT_CONTACT_DELETE:
            sprintf(resp, "[CONTACT] 删除联系人: %s", param);
            break;

        // --- 游戏模块 ---
        case INTENT_GAME_START:
            play_games();
            printf("感谢游玩，下次再来哦！\n");
            break;

        // --- 闲聊与系统 ---
        case INTENT_CHAT:
        default:
            char *chat = llm_chat_generate(&g_llm_model, param);
            if (chat) {
                strncpy(resp, chat, MAX_RESPONSE - 1);
                free(chat);
            } else {
                strcpy(resp, "AI 连接超时，请检查 llamafile。");
            }
            break;
    }
}


static void chatbot_auth_loop(Chatbot *bot) {
    // 简单模拟直接登录，方便调试
    // 实际使用时请取消注释下方的交互逻辑
    user_login("earthcloud", "123", &bot->current_user);

    /* char input[MAX_INPUT], cmd[20], u[50], p[50];
    user_show_guide();
    while (bot->is_running) {
        printf("Auth> ");
        if (!fgets(input, sizeof(input), stdin)) { bot->is_running = 0; return; }
        if (sscanf(input, "%s %s %s", cmd, u, p) < 1) continue;

        if (strcmp(cmd, "login") == 0) {
            if (user_login(u, p, &bot->current_user)) return;
            printf("Fail.\n");
        } else if (strcmp(cmd, "register") == 0) {
            user_register(u, p);
            printf("Registered.\n");
        } else if (strcmp(cmd, "exit") == 0) {
            bot->is_running = 0;
        }
    }
    */
}
