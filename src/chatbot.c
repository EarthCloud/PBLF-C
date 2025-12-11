#include "chatbot.h"
#include "user.h"
#include "llm.h"
#include "game.h"
#include "memo.h"
#include "contact.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "todo.h"

// 全局实例
static LLMModel g_llm_model;
static ContactList *g_contact_list = NULL;

// 私有声明
static void chatbot_auth_loop(Chatbot *bot);

static void dispatch_intent(Chatbot *bot, Intent intent, char *resp);

// 初始化
int chatbot_init(Chatbot *bot) {
    if (!bot) return 0;

    printf("[System] 正在加载模块...\n");

    g_contact_list = contact_create();
    if (!user_init()||!memo_init()||!g_contact_list) {
        printf("模块加载失败！\n");
        return 0;
    }
    todo_init();

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
        // --- 待办事项模块 (TODO) ---
        case INTENT_TODO_ADD: {
            int id = todo_add(param);
            if (id != -1) {
                sprintf(resp, "已添加待办 [ID:%d] %s", id, param[0] ? param : "(新输入)");
            } else {
                sprintf(resp, "添加失败，清单已满。");
            }
            break;
        }
        case INTENT_TODO_LIST:
            // 支持带参搜索
            todo_list(resp, MAX_RESPONSE, param);
            break;

        case INTENT_TODO_COMPLETE: {
            int id = atoi(param);
            if (id > 0 && todo_complete(id)) {
                sprintf(resp, "待办 [ID:%d] 已标记完成。", id);
            } else {
                sprintf(resp, "未找到待办 ID: %s", param);
            }
            break;
        }
        case INTENT_TODO_DELETE: {
            int id = atoi(param);
            if (id > 0 && todo_delete(id)) {
                sprintf(resp, "待办 [ID:%d] 已删除。", id);
            } else {
                sprintf(resp, "删除失败，ID 无效: %s", param);
            }
            break;
        }

        case INTENT_MEMO_ADD: {
            // LLM 当前可能只提取了标题。
            // 需要多轮对话来获取内容
            int id = memo_add(param);
            if (id != -1) {
                sprintf(resp, "已新建备忘录 [ID:%d]\n标题: %s", id, param);
            } else {
                sprintf(resp, "备忘录已满，无法添加！");
            }
            break;
        }
        case INTENT_MEMO_LIST:
            if (strlen(param) > 0) {
                memo_search(param, resp, MAX_RESPONSE); // 如果有参数则搜索
            } else {
                memo_list(resp, MAX_RESPONSE); // 否则列出所有
            }
            break;

        case INTENT_MEMO_READ:
            memo_read_by_title(param, resp, MAX_RESPONSE);
            break;
        case INTENT_MEMO_DELETE:
            if (memo_delete_by_title(param)) {
                sprintf(resp, "已删除标题为 '%s' 的备忘录。", param);
            } else {
                sprintf(resp, "未找到标题为 '%s' 的备忘录，删除失败。", param);
            }
            break;

            // --- 联系人 (CONTACT)---
        case INTENT_CONTACT_ADD: {
            // 参数 param 通常是姓名 (例如 "丁真")
            // 函数内部会交互式询问电话号码
            int id = contact_add(g_contact_list, param);
            if (id != -1) {
                sprintf(resp, "已保存联系人: %s", param[0] ? param : "(新联系人)");
            } else {
                sprintf(resp, "保存联系人失败。");
            }
            break;
        }
        case INTENT_CONTACT_LIST:
            // 支持按姓名或电话搜索
            contact_list_to_buffer(g_contact_list, resp, MAX_RESPONSE, param);
            break;

        case INTENT_CONTACT_DELETE:
            // 按姓名删除
            if (contact_delete(g_contact_list, param)) {
                sprintf(resp, "已删除联系人: %s", param);
            } else {
                sprintf(resp, "未找到联系人: %s", param);
            }
            break;

        // --- 游戏模块 ---
        case INTENT_GAME_START:
            printf("正在启动游戏...\n");
            // 注意：游戏通常需要接管控制台，所以可能不适合通过 resp 返回
            // 这里直接调用 game_snake() 然后 return 一个结束语
            if (strstr(param, "snake") || strstr(param, "贪吃蛇")) {
                game_snake();
                sprintf(resp, "贪吃蛇游戏结束。");
            } else if (strstr(param, "sudoku") || strstr(param, "数独")) {
                game_sudoku();
                sprintf(resp, "数独游戏结束。");
            } else {
                // 如果没匹配到，打开菜单
                play_games();
                sprintf(resp, "游戏选单已关闭。");
            }
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
