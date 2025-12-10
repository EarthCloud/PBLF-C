#ifndef USER_H
#define USER_H

#include "common.h"

// [修复] 删除这里的 User 结构体定义
// 因为它已经在 common.h 中定义过了

// 用户数据文件路径
#define USER_DATA_FILE "data/userlist.txt"

// 用户管理模块初始化
int user_init();

// 用户注册
int user_register(const char* username, const char* password);

// 用户登录
int user_login(const char* username, const char* password, User* current_user);

// 用户登出
void user_logout(User* current_user);

// 检查用户名是否已存在
int user_exists(const char* username);

// 获取当前登录用户
User* user_get_current();

// 设置当前登录用户
void user_set_current(const User* user);

// 显示用户引导信息
void user_show_guide();

// 清理用户模块资源
void user_cleanup();

#endif