#include "http.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//引入网络编程有关的库
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#define closesocket close
#define SOCKET int
#define INVALID_SOCKET -1
#endif

/*
    http_post_json 函数向指定主机和端口的 HTTP 服务器发送带有 JSON 正文的 POST 请求，
    并返回服务器响应的正文内容（以字符串形式），若发生错误则返回 NULL。
    该函数支持 Windows 和类 Unix 系统，内部处理了 socket 连接、请求头构建、数据发送与响应接收等流程。
*/
char *http_post_json(const char *hostname, int port, const char *endpoint, const char *json_body) {
#ifdef _WIN32
    // 在Windows环境下，需要调用WSAStartup，这样才能在后续创建套接字
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) return NULL;
#endif

    // 创建套接字，常量AF_INET是使用IPV4地址，SOCK_STREAM表示使用TCP协议（面向有连接的可靠流）
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) return NULL;

    /*
     *解析服务器地址 (DNS Resolution)
     *hostent 结构体用于表示主机的信息
     *包括主机名 (h_name)、主机别名列表 (h_aliases)、地址类型 (h_addrtype)、地址长度 (h_length) 以及主机地址列表 (h_addr_list)。
     *它常用于网络编程中，特别是在解析主机名和地址时。
    */
    struct hostent *host = gethostbyname(hostname);
    if (!host) {
        closesocket(sock);
        return NULL;
    }

    // 设置设置连接目标 (Addressing)
    struct sockaddr_in server_addr = {0};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr = *((struct in_addr *) host->h_addr);

    // 进行三次握手
    if (connect(sock, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        printf("[HTTP] 连接到主机 %s:%d 失败\n", hostname, port);
        closesocket(sock);
        return NULL;
    }

    // 包装请求头
    char header[512];
    int body_len = (int) strlen(json_body);
    snprintf(header, sizeof(header),
             "POST %s HTTP/1.1\r\n"
             "Host: %s:%d\r\n"
             "Content-Type: application/json\r\n"
             "Content-Length: %d\r\n"
             "Connection: close\r\n\r\n",
             endpoint, hostname, port, body_len
    );

    // 发送请求头和请求体
    send(sock, header, strlen(header), 0);
    send(sock, json_body, body_len, 0);

    // 先预定义响应体的容量
    int capacity = 4096, size = 0;
    // 手动分配指针
    char *response = malloc(capacity);
    int len;
    while ((len = recv(sock, response + size, capacity - size - 1, 0)) > 0) {
        size += len;
        if (capacity - size < 1024) {
            capacity *= 2;
            char *new_ptr = realloc(response, capacity);
            if (!new_ptr) {
                free(response);
                closesocket(sock);
                return NULL;
            }
            response = new_ptr;
        }
    }
    response[size] = '\0';
    closesocket(sock);

#ifdef _WIN32
    WSACleanup();
#endif

    char *body = strstr(response, "\r\n\r\n");
    if (body) {
        char *result = strdup(body + 4);
        free(response);
        return result;
    }
    return response;
}

// json解析函数
int http_extract_json_string(const char *json, const char *key, char *buffer, int max_len) {
    if (!json || !key || !buffer) return 0;

    char pattern[128];
    snprintf(pattern, sizeof(pattern), "\"%s\": \"", key);

    char *start = strstr(json, pattern);
    if (!start) {
        snprintf(pattern, sizeof(pattern), "\"%s\":\"", key);
        start = strstr(json, pattern);
    }

    if (!start) return 0;

    start += strlen(pattern);
    int i = 0;
    while (*start && i < max_len - 1) {
        if (*start == '"' && *(start - 1) != '\\') break;
        if (*start == '\\') {
            char next = *(start + 1);
            if (next == 'n') {
                buffer[i++] = '\n';
                start += 2;
            } else if (next == '"') {
                buffer[i++] = '"';
                start += 2;
            } else if (next == '\\') {
                buffer[i++] = '\\';
                start += 2;
            } else { buffer[i++] = *start++; }
        } else {
            buffer[i++] = *start++;
        }
    }
    buffer[i] = '\0';
    return 1;
}