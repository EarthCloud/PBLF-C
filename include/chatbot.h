#ifndef CHATBOT_H
#define CHATBOT_H

#include "common.h"

typedef struct {
    User current_user;
    int is_running;
} Chatbot;

int chatbot_init(Chatbot *bot);
void chatbot_run(Chatbot *bot);
char *chatbot_process_input(Chatbot *bot, const char *input);

#endif