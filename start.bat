@echo off
setlocal

echo [System] 初始化 llamafile 环境...
set "PROJECT_ROOT=%~dp0"
set "PROJECT_ROOT=%PROJECT_ROOT:~0,-1%"

:: 设置端口
set "PORT=8080"
set "MODEL_FILE=qwen2.5-1.5b-instruct-q4_k_m.gguf"
:: ================= 启动 llamafile =================
echo [System] 正在启动本地 AI 引擎 (llamafile)...
echo [System] 模型: tools\model.gguf

:: 启动参数说明：
:: -m: 指定模型文件
:: --server: 启动 API 服务器模式
:: --nobrowser: 不自动打开浏览器
:: --port: 指定端口
:: >nul 2>&1: 隐藏后台输出
start /B "" "%PROJECT_ROOT%\tools\llamafile.exe" -m "%PROJECT_ROOT%\tools\%MODEL_FILE%" --server --nobrowser --port %PORT% >nul 2>&1

:: 等待预热 (llamafile 启动稍慢，多给点时间)
echo [System] 等待引擎启动 (约 5-8 秒)...
timeout /t 8 /nobreak >nul

:: ================= 启动主程序 =================
echo [System] 正在启动 Chatbot...
if exist "chatbot.exe" (
    chatbot.exe
) else (
    echo [Error] 找不到 chatbot.exe，请先编译!
)

:: ================= 清理退出 =================
echo.
echo [System] 正在关闭 AI 引擎...
taskkill /F /IM llamafile.exe >nul 2>&1
pause