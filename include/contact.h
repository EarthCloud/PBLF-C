#ifndef CONTACT_H
#define CONTACT_H

#include "common.h"

#define CONTACT_NAME_LEN 50
#define CONTACT_PHONE_LEN 20

// 联系人节点 (链表结构)
typedef struct Contact {
    int id;
    char name[CONTACT_NAME_LEN];
    char phone[CONTACT_PHONE_LEN];
    struct Contact *next;
} Contact;

// 联系人列表管理器
typedef struct {
    Contact *head;
    int count;
    int next_id;
} ContactList;

// 初始化/创建列表
ContactList* contact_create();

// 销毁列表
void contact_destroy(ContactList* list);

// 添加联系人 (交互式：如果 name 为空则询问，phone 总是询问)
int contact_add(ContactList* list, const char* name);

// 删除联系人 (按姓名)
int contact_delete(ContactList* list, const char* name);

// 搜索/列出联系人 (结果写入 output 缓冲区)
void contact_list_to_buffer(ContactList* list, char* output, int max_len, const char* filter);

#endif