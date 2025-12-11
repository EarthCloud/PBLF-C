#ifndef MEMO_H
#define MEMO_H

#define MAX_MEMOS 50
#define MAX_LENGTH 100
#define FILENAME "memos.txt"

typedef struct {
    int id;
    char title[MAX_LENGTH];
    char content[MAX_LENGTH];
    int done;
} Memo;

extern Memo memos[MAX_MEMOS];
extern int count;

void showMenu();
void addMemo();
void listMemos();
void deleteMemo();
void saveMemos();
void loadMemos();

#endif