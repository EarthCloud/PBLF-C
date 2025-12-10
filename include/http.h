#ifndef HTTP_H
#define HTTP_H

// 发送 HTTP POST 请求
// 返回: 响应 Body 的动态分配字符串 (调用者需 free)
// 失败返回 NULL
char* http_post_json(const char* hostname, int port, const char* endpoint, const char* json_body);

// 辅助：从 JSON 中提取指定键的字符串值
// 返回: 1 成功, 0 失败
int http_extract_json_string(const char* json, const char* key, char* buffer, int max_len);

#endif