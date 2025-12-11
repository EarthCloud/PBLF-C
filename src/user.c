#include "user.h"
#include <string.h>
#include <stdlib.h>

// 全局变量：仅保留当前登录用户
User current_user = {0};

// 获取当前登录用户
User *user_get_current() {
    return &current_user;
}

// 设置当前登录用户
void user_set_current(const User *user) {
    if (user != NULL) {
        // 使用 memcpy 确保所有字段都被复制
        memcpy(&current_user, user, sizeof(User));
    }
}

// 用户模块初始化
int user_init() {
    // 尝试以追加模式打开文件，如果文件不存在则创建它
    FILE *fp = fopen(USER_DATA_FILE, "a+");
    if (fp == NULL) {
        return 0; // 初始化失败
    }
    fclose(fp);

    // 清空当前用户状态
    memset(&current_user, 0, sizeof(User));
    return 1;
}

// 清理用户模块资源 
void user_cleanup() {
    memset(&current_user, 0, sizeof(User));
}


// 检查用户名是否已存在 (直接读取文件进行判重)
int user_exists(const char *username) {
    FILE *fp = fopen(USER_DATA_FILE, "r");
    if (fp == NULL) {
        // 文件不存在或无法读取，视为该用户不存在
        return 0;
    }

    char stored_username[MAX_USERNAME];
    char stored_password[MAX_PASSWORD];

    // 按行读取文件，检查是否存在同名用户
    while (fscanf(fp, "%s %s", stored_username, stored_password) == 2) {
        if (strcmp(stored_username, username) == 0) {
            fclose(fp);
            return 1; // 找到用户
        }
    }

    fclose(fp);
    return 0; // 未找到用户
}

// 用户注册 
int user_register(const char *username, const char *password) {
    // 1. 检查用户名是否已存在
    if (user_exists(username)) {
        return 0; // 用户已存在，注册失败
    }

    // 2. 将新用户追加写入文件
    FILE *fp = fopen(USER_DATA_FILE, "a");
    if (fp == NULL) {
        return 0; // 无法写入文件，注册失败
    }

    // 格式：用户名 密码\n (例如: testuser 123456\n)
    fprintf(fp, "%s %s\n", username, password);

    fclose(fp);
    return 1; // 注册成功
}

// 用户登录 (从文件加载数据并验证)
int user_login(const char *username, const char *password, User *user) {
    FILE *fp = fopen(USER_DATA_FILE, "r");
    if (fp == NULL) {
        return 0; // 文件不存在或无法读取，登录失败
    }

    char stored_username[MAX_USERNAME];
    char stored_password[MAX_PASSWORD];
    int success = 0;

    // 按行读取文件，寻找匹配的用户名和密码
    while (fscanf(fp, "%s %s", stored_username, stored_password) == 2) {
        if (strcmp(stored_username, username) == 0 &&
            strcmp(stored_password, password) == 0) {
            // 匹配成功，填充用户信息
            strcpy(user->username, stored_username);
            // 注意：密码字段可以不存，出于安全考虑，但为了匹配原始结构保留
            strcpy(user->password, stored_password);
            user->is_logged_in = 1;

            // 设置当前登录用户
            user_set_current(user);

            success = 1;
            break; // 找到即退出循环
        }
    }

    fclose(fp);
    return success;
}

// 用户登出
void user_logout(User *user) {
    if (user != NULL) {
        user->is_logged_in = 0;
    }
    // 确保全局的 current_user 被清空
    memset(&current_user, 0, sizeof(User));
}

// 显示用户引导信息
void user_show_guide() {
    printf("\n=== 用户引导 ===\n");
    printf("1. 输入 'register [用户名] [密码]' 进行注册\n");
    printf("2. 输入 'login [用户名] [密码]' 进行登录\n");
    printf("3. 输入 'exit' 退出程序\n");
    printf("4. 登录后可以使用待办清单等功能\n");
    printf("================\n\n");
}