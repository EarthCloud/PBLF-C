#include "llm.h"
#include "http.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// 意图映射表 (Key -> Enum)
typedef struct {
    const char *key;
    IntentType type;
} IntentMapping;

// [重构] 完整的意图映射
static const IntentMapping INTENT_MAP[] = {
    // TODO
    {"TODO_ADD", INTENT_TODO_ADD},
    {"TODO_LIST", INTENT_TODO_LIST},
    {"TODO_COMPLETE", INTENT_TODO_COMPLETE},
    {"TODO_DELETE", INTENT_TODO_DELETE},

    // MEMO
    {"MEMO_LIST", INTENT_MEMO_LIST},
    {"MEMO_ADD", INTENT_MEMO_ADD},
    {"MEMO_READ", INTENT_MEMO_READ},
    {"MEMO_DELETE", INTENT_MEMO_DELETE},

    // CONTACT
    {"CONTACT_LIST", INTENT_CONTACT_LIST},
    {"CONTACT_ADD", INTENT_CONTACT_ADD},
    {"CONTACT_DELETE", INTENT_CONTACT_DELETE},

    // OTHERS
    {"GAME_START", INTENT_GAME_START},
    {"CHAT", INTENT_CHAT},
    {NULL, INTENT_UNKNOWN}
};

// 辅助：简单的 JSON 转义
static void json_escape(const char *input, char *output, int max_len) {
    int j = 0;
    for (int i = 0; input[i] && j < max_len - 2; i++) {
        if (input[i] == '"') {
            output[j++] = '\\';
            output[j++] = '"';
        } else if (input[i] == '\n') {
            output[j++] = '\\';
            output[j++] = 'n';
        } else if (input[i] == '\\') {
            output[j++] = '\\';
            output[j++] = '\\';
        } else output[j++] = input[i];
    }
    output[j] = '\0';
}

// 初始化大模型
void llm_init(LLMModel *model, const char *hostname, int port) {
    if (!model) return;
    strncpy(model->hostname, hostname ? hostname : "127.0.0.1", 63);
    model->port = (port > 0) ? port : 8080;
}

// 发送请求
static char *send_openai_request(LLMModel *model, const char *system_prompt, const char *user_input, float temp) {
    char escaped_input[2048];
    json_escape(user_input, escaped_input, 2048);

    char escaped_sys[4096];
    json_escape(system_prompt, escaped_sys, 4096);

    // 封装请求体
    char json_body[8192];
    snprintf(json_body, sizeof(json_body),
             "{"
             "\"model\": \"user-model\","
             "\"messages\": ["
             "  {\"role\": \"system\", \"content\": \"%s\"},"
             "  {\"role\": \"user\", \"content\": \"%s\"}"
             "],"
             "\"temperature\": %.1f"
             "}",
             escaped_sys, escaped_input, temp
    );

    printf("[Qwen] 正在思考...\n");
    return http_post_json(model->hostname, model->port, "/v1/chat/completions", json_body);
}

// 核心意图分析逻辑
Intent llm_analyze_intent(LLMModel *model, const char *user_input) {
    // 初始化意图
    Intent intent = {INTENT_UNKNOWN, ""};
    if (!model) return intent;

    // 构造详细的 Prompt，指导模型分类，由于相关转义问题，这里直接硬编码在文件里了

    const char *sys_prompt =
            "你是一个指令分类器。请分析用户输入，严格从以下类别中选择一个最匹配的意图：\\n"
            "【待办类】\\n"
            "- TODO_ADD: 添加待办/任务，动作性任务。如：'去买菜'、'提醒我跑步'。参数：任务内容。\\n"
            "- TODO_LIST: 查询/列出待办。参数：搜索关键词(无则为空)。\\n"
            "- TODO_COMPLETE: 完成/勾选待办。参数：任务ID(必须是数字)。\\n"
            "- TODO_DELETE: 删除/移除待办。参数：任务ID(必须是数字)。\\n"
            "【备忘录类】\\n"
            "- MEMO_ADD: 新建备忘/笔记，静态记录/日记/想法。如：'心情不好'、'写个日记'。参数：标题。\\n"
            "- MEMO_LIST: 列出备忘录。参数：搜索关键词。\\n"
            "- MEMO_READ: 读取/查看备忘内容。参数：标题。\\n"
            "- MEMO_DELETE: 删除备忘录。参数：标题。\\n"
            "【联系人类】\\n"
            "- CONTACT_ADD: 添加联系人。如：'存下丁真的电话'、'记一下那个人的联系方式'。参数：姓名。\\n"
            "- CONTACT_LIST: 查找/列出联系人。参数：姓名关键词。\\n"
            "- CONTACT_DELETE: 删除联系人。参数：姓名。\\n"
            "【其他】\\n"
            "- GAME_START: 玩游戏。参数：游戏名(贪吃蛇/计算器/数独)。\\n"
            "- CHAT: 闲聊、情感寻求、无明确指令。如：'我好无聊'、'求安慰'。参数：用户原话。\\n"
            "\\n"
            "回复格式必须严格为: 类别|参数\\n"
            "不要解释，不要输出表头。\\n"
            "类别只能是我上面用大写字母表示的，不要自己编类别。\\n"
            "但凡有一点不确定，就归类为CHAT。\\n"
            "如果归类为CHAT了，用户发的什么原封不动作为参数，一个字都不能改！！！\\n"
            "示例:\\n"
            "'买牛奶' -> TODO_ADD|买牛奶\\n"
            "'做完第2个了' -> TODO_COMPLETE|2\\n"
            "'写个日记叫今天' -> MEMO_ADD|今天\\n"
            "'查下王明的电话' -> CONTACT_LIST|王明\\n"
            "'我现在心情很不好，能安慰安慰我嘛' -> CHAT|我现在心情很不好，能安慰安慰我嘛\\n"
            "'你好' -> CHAT|你好\\n"
            "\\n"
            "用户的输入如下：\\n";


    // 创建请求实体
    char *response = send_openai_request(model, sys_prompt, user_input, 0.1F);
    if (!response) return intent;

    char content[1024] = {0};
    if (http_extract_json_string(response, "content", content, sizeof(content))) {
        // 使用鲁棒的解析逻辑
        printf("[Debug] 原始解析内容：%s\n",content);
        int found = 0;
        for (int i = 0; INTENT_MAP[i].key != NULL; i++) {
            char *keyword_pos = strstr(content, INTENT_MAP[i].key);

            if (keyword_pos) {
                intent.type = INTENT_MAP[i].type;

                // 跳过关键词和后续的分隔符
                char *p = keyword_pos + strlen(INTENT_MAP[i].key);
                while (*p && (*p == '|' || *p == ':' || *p == ' ' || *p == '\t' || *p == '\n')) {
                    p++;
                }

                strncpy(intent.parameter, p, MAX_INPUT - 1);

                // 去除尾部换行
                int len = strlen(intent.parameter);
                while (len > 0 && (intent.parameter[len - 1] == '\n' || intent.parameter[len - 1] == '\r')) {
                    intent.parameter[len - 1] = '\0';
                    len--;
                }

                found = 1;
                break;
            }
        }

        if (!found) {
            intent.type = INTENT_CHAT;
            strncpy(intent.parameter, user_input, MAX_INPUT - 1);
        }
    }
    free(response);
    return intent;
}

// 生成回复的逻辑
char *llm_chat_generate(LLMModel *model, const char *user_input) {
    const char *sys_prompt = "你是一个智能助手，请用中文简短回答。";
    char *response = send_openai_request(model, sys_prompt, user_input, 0.7F);
    if (!response) return NULL;

    char *clean_resp = malloc(2048);
    if (!http_extract_json_string(response, "content", clean_resp, 2048)) {
        strcpy(clean_resp, "Error parsing AI response.");
    }
    free(response);
    return clean_resp;
}

// int llm_extract_keywords(LLMModel *model, const char *text, char **keywords, int max_keywords) {
//     // 此函数暂时保留，逻辑不变，根据需要可后续优化
//     return 0;
// }
