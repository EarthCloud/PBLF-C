#include "contact.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 创建列表
ContactList* contact_create() {
    ContactList* list = (ContactList*)malloc(sizeof(ContactList));
    if (list) {
        list->head = NULL;
        list->count = 0;
        list->next_id = 1;
    }
    return list;
}

// 销毁列表
void contact_destroy(ContactList* list) {
    if (!list) return;
    Contact *current = list->head;
    while (current) {
        Contact *next = current->next;
        free(current);
        current = next;
    }
    free(list);
}

// 添加联系人
int contact_add(ContactList* list, const char* name) {
    if (!list) return -1;

    Contact *new_contact = (Contact*)malloc(sizeof(Contact));
    if (!new_contact) return -1;

    new_contact->id = list->next_id++;

    // 1. 处理姓名
    if (name && strlen(name) > 0) {
        strncpy(new_contact->name, name, CONTACT_NAME_LEN - 1);
    } else {
        printf(">> 请输入联系人姓名: ");
        if (fgets(new_contact->name, CONTACT_NAME_LEN, stdin)) {
            new_contact->name[strcspn(new_contact->name, "\n")] = 0;
        }
    }
    new_contact->name[CONTACT_NAME_LEN - 1] = '\0';

    // 2. 处理电话 (总是交互式询问，因为 LLM 很难一次性提取两个参数)
    printf(">> 请输入 [%s] 的电话: ", new_contact->name);
    if (fgets(new_contact->phone, CONTACT_PHONE_LEN, stdin)) {
        new_contact->phone[strcspn(new_contact->phone, "\n")] = 0;
    } else {
        new_contact->phone[0] = '\0';
    }

    // 3. 插入链表 (头插法简单点，或者尾插法)
    // 这里用头插法
    new_contact->next = list->head;
    list->head = new_contact;
    list->count++;

    return new_contact->id;
}

// 删除联系人 (按姓名)
int contact_delete(ContactList* list, const char* name) {
    if (!list || !list->head || !name) return 0;

    Contact *current = list->head;
    Contact *prev = NULL;

    while (current) {
        // 支持模糊匹配删除
        if (strstr(current->name, name) != NULL) {
            if (prev == NULL) {
                list->head = current->next;
            } else {
                prev->next = current->next;
            }
            free(current);
            list->count--;
            return 1;
        }
        prev = current;
        current = current->next;
    }
    return 0;
}

// 列表/搜索输出
void contact_list_to_buffer(ContactList* list, char* output, int max_len, const char* filter) {
    if (!list || list->count == 0) {
        snprintf(output, max_len, "联系人列表是空的。");
        return;
    }

    output[0] = '\0';
    char line[128];
    strncat(output, "=== 联系人列表 ===\n", max_len - 1);

    int found = 0;
    Contact *current = list->head;
    while (current) {
        // 过滤逻辑: 匹配姓名或电话
        if (!filter || strlen(filter) == 0 ||
            strstr(current->name, filter) || strstr(current->phone, filter)) {

            snprintf(line, sizeof(line), "[ID:%d] %s - %s\n",
                current->id, current->name, current->phone);

            if (strlen(output) + strlen(line) < max_len) {
                strcat(output, line);
                found++;
            }
        }
        current = current->next;
    }

    if (found == 0 && filter) {
        snprintf(output, max_len, "未找到匹配 '%s' 的联系人。", filter);
    }
}