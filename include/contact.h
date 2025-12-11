#define CONTACT_H

#include<stdbool.h>

#define MAX_NAME_LEN 50
#define MAX_PHONE_LEN 25
#define MAX_EMAIL_LEN 100

// 联系人信息结构
typedef struct{
    int id;
    char name[MAX_NAME_LEN];
    char phone[MAX_PHONE_LEN];
    char email[MAX_EMAIL_LEN];
}Contactinfo;

// 链表节点
typedef struct ContactNode{
    Contactinfo data;
    struct ContatNode*next;
}ContactNode;

// 初始化链表与销毁链表
bool init_list(ContactNode **L);
void destroy_list(ContactNode **L);

// 联系人管理
int add_contact(ContactNode *L, const char *name, const char *phone, const char *email);
bool delete_contact(ContactNode *L, int id);
bool update_contact(ContactNode *L, int id, const char *name, const char *phone, const char *email);

// 查找功能
ContactNode* search_by_id(ContactNode *L, int id);
ContactNode* search_by_name(ContactNode *L, const char *name);
ContactNode* search_by_phone(ContactNode *L, const char *phone);

// 显示功能
void display_one(ContactNode *node);
void display_all(ContactNode *L);

// 文件保存与加载
bool save_to_file(ContactNode *L, const char *filename);
bool load_from_file(ContactNode **L, const char *filename);

// 统计联系人数量
int get_contact_count(ContactNode *L);





