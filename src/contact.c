#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "contact.h"

static int next_id=1;

// 初始化链表
bool init_list(ContactNode **L) {
    if (L == NULL) {
        return false;
    }
    *L = (ContactNode*)malloc(sizeof(ContactNode));
    if (*L == NULL) {
        return false;
    }
        (*L)->next = NULL;
        return true;
}

// 销毁链表
void destroy_list(ContactNode **L) {
    if(L==NULL||*L==NULL){
        return;
    }
    ContactNode *curr = *L;
    ContactNode *next = NULL;
    while (curr != NULL) {
        next=curr->next;
        free(curr);
        curr=next;
    }
    *L = NULL;
}

// 添加联系人
int add_contact(ContactNode *L, const char *name, const char *phone, const char *email) {
    if (L == NULL) return 0;
    
    ContactNode *new_node = (ContactNode*)malloc(sizeof(ContactNode));
    if (new_node == NULL) return 0;
    
    new_node->data.id = next_id++;
    strncpy(new_node->data.name, name, MAX_NAME_LEN - 1);
    strncpy(new_node->data.phone, phone, MAX_PHONE_LEN - 1);
    strncpy(new_node->data.email, email, MAX_EMAIL_LEN - 1);
    new_node->data.name[MAX_NAME_LEN - 1] = '\0';
    new_node->data.phone[MAX_PHONE_LEN - 1] = '\0';
    new_node->data.email[MAX_EMAIL_LEN - 1] = '\0';
    
    new_node->next = L->next;
    L->next = new_node;
    return new_node->data.id;
}

// 删除联系人
bool delete_contact(ContactNode *L, int id) {
    if (L == NULL || L->next == NULL) return false;
    
    ContactNode *prev = L, *curr = L->next;
    while (curr != NULL) {
        if (curr->data.id == id) {
            prev->next = curr->next;
            free(curr);
            return true;
        }
        prev = curr;
        curr = curr->next;
    }
    return false;
}

// 修改联系人
bool update_contact(ContactNode *L, int id, const char *name, const char *phone, const char *email) {
    ContactNode *node = search_by_id(L, id);
    if (node == NULL) return false;
    
    if (name != NULL) strncpy(node->data.name, name, MAX_NAME_LEN - 1);
    if (phone != NULL) strncpy(node->data.phone, phone, MAX_PHONE_LEN - 1);
    if (email != NULL) strncpy(node->data.email, email, MAX_EMAIL_LEN - 1);
    return true;
}

// 按ID查找
ContactNode* search_by_id(ContactNode *L, int id) {
    if (L == NULL) return NULL;
    ContactNode *curr = L->next;
    while (curr != NULL) {
        if (curr->data.id == id) return curr;
        curr = curr->next;
    }
    return NULL;
}

// 按姓名查找
ContactNode* search_by_name(ContactNode *L, const char *name) {
    if (L == NULL) return NULL;
    ContactNode *curr = L->next;
    while (curr != NULL) {
        if (strcmp(curr->data.name, name) == 0) return curr;
        curr = curr->next;
    }
    return NULL;
}

// 按电话查找
ContactNode* search_by_phone(ContactNode *L, const char *phone) {
    if (L == NULL) return NULL;
    ContactNode *curr = L->next;
    while (curr != NULL) {
        if (strcmp(curr->data.phone, phone) == 0) return curr;
        curr = curr->next;
    }
    return NULL;
}

// 显示单个联系人
void display_one(ContactNode *node) {
    if (node == NULL) return;
    printf("ID: %d, 姓名: %s, 电话: %s, 邮箱: %s\n",
           node->data.id, node->data.name, node->data.phone, node->data.email);
}

// 显示所有联系人
void display_all(ContactNode *L) {
    if (L == NULL || L->next == NULL) {
        printf("通讯录为空\n");
        return;
    }
    ContactNode *curr = L->next;
    while (curr != NULL) {
        display_one(curr);
        curr = curr->next;
    }
}

// 保存到文件
bool save_to_file(ContactNode *L, const char *filename) {
    if (L == NULL) return false;
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) return false;
    
    ContactNode *curr = L->next;
    while (curr != NULL) {
        fprintf(fp, "%d,%s,%s,%s\n",
                curr->data.id, curr->data.name, curr->data.phone, curr->data.email);
        curr = curr->next;
    }
    fclose(fp);
    return true;
}

// 从文件加载
bool load_from_file(ContactNode **L, const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) return false;
    
    destroy_list(L);
    init_list(L);
 
    Contactinfo info;
    while (fscanf(fp, "%d,%49[^,],%24[^,],%99[^\n]\n",&info.id, info.name, info.phone, info.email) == 4) 
    {
        ContactNode *new_node = (ContactNode*)malloc(sizeof(ContactNode));
        if (new_node==NULL){
            fclose(fp);
            return false;
        }
        new_node->data = info;
        new_node->next = (*L)->next;
        (*L)->next = new_node;
        if (info.id>=next_id) {next_id=info.id+1;
        
        }
    }
    fclose(fp);
    return true;
}

// 获取联系人数量  
int get_contact_count(ContactNode*L){
    int count=0;
    if(L!=NULL){
        ContactNode*curr=L->next;
        while (curr!=NULL){
            count++;
            curr=curr->next;
        }
    }
    return count;
}

