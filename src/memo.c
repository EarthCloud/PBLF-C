#include "memo.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Memo memos[MAX_MEMOS];
int count = 0;

void showMenu() {
    printf("\n1. 添加备忘录\n");
    printf("2. 查看备忘录\n");
    printf("3. 删除备忘录\n");
    printf("0. 退出\n");
    printf("选择: ");
}

void addMemo() {
    if(count >= MAX_MEMOS) {
        printf("备忘录已满！\n");
        return;
    }
    
    printf("标题: ");
    fgets(memos[count].title, MAX_LENGTH, stdin);
    memos[count].title[strcspn(memos[count].title, "\n")] = 0;
    
    printf("内容: ");
    fgets(memos[count].content, MAX_LENGTH, stdin);
    memos[count].content[strcspn(memos[count].content, "\n")] = 0;
    
    memos[count].id = count + 1;
    memos[count].done = 0;
    count++;
    
    printf("已添加！\n");
}

void listMemos() {
    if(count == 0) {
        printf("暂无备忘录\n");
        return;
    }
    
    printf("\n=== 备忘录列表 ===\n");
    for(int i = 0; i < count; i++) {
        printf("%d. %s %s\n", 
               memos[i].id,
               memos[i].done ? "[✓]" : "[ ]",
               memos[i].title);
    }
    
    printf("\n查看详情？(输入ID，0返回): ");
    int id;
    scanf("%d", &id);
    getchar();
    
    if(id > 0 && id <= count) {
        printf("\n标题: %s\n", memos[id-1].title);
        printf("内容: %s\n", memos[id-1].content);
        printf("状态: %s\n", memos[id-1].done ? "已完成" : "未完成");
        
        printf("\n标记为已完成？(y/N): ");
        char c;
        scanf("%c", &c);
        getchar();
        if(c == 'y' || c == 'Y') {
            memos[id-1].done = 1;
            printf("已标记完成！d(-_^)\n");
        }
    }
}

void deleteMemo() {
    if(count == 0) {
        printf("暂无备忘录\n");
        return;
    }
    
    listMemos();
    printf("\n删除哪个？(输入ID，0取消): ");
    int id;
    scanf("%d", &id);
    getchar();
    
    if(id > 0 && id <= count) {
        for(int i = id-1; i < count-1; i++) {
            memos[i] = memos[i+1];
            memos[i].id = i+1;
        }
        count--;
        printf("已删除！\n");
    }
}

void saveMemos() {
    FILE *file = fopen(FILENAME, "w");
    if(!file) return;
    
    fprintf(file, "%d\n", count);
    for(int i = 0; i < count; i++) {
        fprintf(file, "%d\n%s\n%s\n%d\n", 
                memos[i].id,
                memos[i].title,
                memos[i].content,
                memos[i].done);
    }
    fclose(file);
}

void loadMemos() {
    FILE *file = fopen(FILENAME, "r");
    if(!file) return;
    
    fscanf(file, "%d\n", &count);
    for(int i = 0; i < count; i++) {
        fscanf(file, "%d\n", &memos[i].id);
        fgets(memos[i].title, MAX_LENGTH, file);
        memos[i].title[strcspn(memos[i].title, "\n")] = 0;
        fgets(memos[i].content, MAX_LENGTH, file);
        memos[i].content[strcspn(memos[i].content, "\n")] = 0;
        fscanf(file, "%d\n", &memos[i].done);
    }
    fclose(file);
}