#include "chatbot.h"
#include <stdio.h>

// 主程序入口
int main() {
    // 防止中文乱码，因windows控制台的默认编码为GBK
    system("chcp 65001");

    // 创建对话机器人对象
    Chatbot bot;

    // 初始化对话机器人
    if (!chatbot_init(&bot)) {
        printf("对话机器人初始化失败！\n");
        return 1;
    }

    // 运行对话机器人主循环
    chatbot_run(&bot);

    return 0;
}
