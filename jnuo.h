/*
  2026  from jnuo      v1.0.0
    - 持续制作中......
    - Continuously in production .....
    - 功能1：日志功能
    - Function 1: Log function
      - 请先init_log_file("./你自定义的日志名.txt");
      - Please first initw_file("./your custom log name. txt");
      - 使用JNUO_LOG_日志类型("日志内容");
      - Use JNUO_LOG_Log type("log content");
        - 支持的日志类型有：DEBUG,INFO,ERROR,WARN
        - The supported log types are DEBUG, INFO, ERROR, and WARN
		- 也就是JNUO_LOG_DEBUG("日志内容");JNUO_LOG_INFO("日志内容");
                JNUO_LOG_ERROR("日志内容");JNUO_LOG_WARN("日志内容");
        - That is, JNUO_LOG_DEBUG("log content"); JNUO_LOG_INFO("log content");
                   JNUO_LOG_ERROR("Log Content"); JNUO_LOG_WARN("Log Content");
    - GitHub的项目：https://github.com/jnuo35/header_file_jnuo
    - Projects on GitHub: https://github.com/jnuo35/header_file_jnuo
*/
#ifndef JNUO_H
#define JNUO_H

#include <stdio.h>
#include <time.h>
#include <stdarg.h>
#include <string.h>

#ifdef _WIN32
#include <direct.h>  // _mkdir
#include <windows.h>
#define mkdir_(path) _mkdir(path)
#define PATH_SEP '\\'
#else
#include <sys/stat.h>
#include <sys/types.h>
#define mkdir_(path) mkdir(path, 0755)
#define PATH_SEP '/'
#endif

namespace jnuo {
	
	enum class LogLevel { Debug, Info, Warn, Error };
	static LogLevel g_log_threshold = LogLevel::Info;
	static FILE* g_log_file = NULL;
	
// 递归创建目录
	inline void create_directories(const char* path) {
		if (!path || !*path) return;
		
		char temp[512];
		strcpy(temp, path);
		
		// 去掉文件名，只留目录路径
		char* last_sep = strrchr(temp, PATH_SEP);
		if (!last_sep) return;  // 没有目录，就在当前目录
		
		*last_sep = '\0';  // 截断，只保留目录部分
		
		// 逐级创建目录
		for (char* p = temp; *p; p++) {
			if (*p == PATH_SEP) {
				char old = *p;
				*p = '\0';
				mkdir_(temp);  // 尝试创建，失败也无所谓（可能已存在）
				*p = old;
			}
		}
		mkdir_(temp);  // 创建最后一级
	}
	
// 初始化日志文件（智能版）
	inline void init_log_file(const char* filename) {
		if (g_log_file) fclose(g_log_file);
		
		// 先尝试创建目录
		create_directories(filename);
		
		// 打开文件（追加模式）
		g_log_file = fopen(filename, "a");
		
		if (g_log_file) {
			JNUO_LOG_INFO("日志文件已创建: %s", filename);
		} else {
			// 如果失败，试试当前目录
			g_log_file = fopen("./log.txt", "a");
			if (g_log_file) {
				JNUO_LOG_WARN("无法创建指定目录，使用当前目录的 log.txt");
			}
		}
	}
	
// 关闭日志文件
	inline void close_log_file() {
		if (g_log_file) {
			fprintf(g_log_file, "========== 日志结束 ==========\n");
			fclose(g_log_file);
			g_log_file = NULL;
		}
	}
	
	inline const char* timestamp() {
		static char buf[20];
		time_t t = time(NULL);
		struct tm tm = *localtime(&t);
		strftime(buf, sizeof(buf), "%H:%M:%S", &tm);
		return buf;
	}
	
	inline void log(LogLevel level, const char* fmt, ...) {
		const char* level_str[] = {"debug", "info", "warn", "error"};
		
		char prefix[256];
		snprintf(prefix, sizeof(prefix), "[%s][%s] ", timestamp(), level_str[static_cast<int>(level)]);
		
		char buffer[1024];
		va_list args;
		va_start(args, fmt);
		vsnprintf(buffer, sizeof(buffer), fmt, args);
		va_end(args);
		
		// 输出到屏幕
//		FILE* out = (level >= LogLevel::Warn) ? stderr : stdout;
//		fprintf(out, "%s%s\n", prefix, buffer);
//		fflush(out);
		
		// 输出到文件
		if (g_log_file) {
			fprintf(g_log_file, "%s%s\n", prefix, buffer);
			fflush(g_log_file);
		}
	}
	
}

// 宏
#define JNUO_LOG_DEBUG(...) \
do { if (jnuo::g_log_threshold <= jnuo::LogLevel::Debug) \
jnuo::log(jnuo::LogLevel::Debug, __VA_ARGS__); } while(0)

#define JNUO_LOG_INFO(...) \
do { if (jnuo::g_log_threshold <= jnuo::LogLevel::Info) \
jnuo::log(jnuo::LogLevel::Info, __VA_ARGS__); } while(0)

#define JNUO_LOG_WARN(...) \
do { if (jnuo::g_log_threshold <= jnuo::LogLevel::Warn) \
jnuo::log(jnuo::LogLevel::Warn, __VA_ARGS__); } while(0)

#define JNUO_LOG_ERROR(...) \
do { if (jnuo::g_log_threshold <= jnuo::LogLevel::Error) \
jnuo::log(jnuo::LogLevel::Error, __VA_ARGS__); } while(0)

#endif
