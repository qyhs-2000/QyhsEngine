#pragma once
#include "function/global/global_context.h"
#include "core/log/log_system.h"


#define LOG_HELPER(LOG_LEVEL,...)\
		g_runtime_global_context.m_logger_system->log(LOG_LEVEL, "[" + std::string(__FUNCTION__) + "] " + __VA_ARGS__);
#define LOG_INFO(...)	LOG_HELPER(LogSystem::LogLevel::info,__VA_ARGS__);
#define LOG_ERROR(...)	LOG_HELPER(LogSystem::LogLevel::error,__VA_ARGS__);
#define LOG_FUNCTION_CALL(function)  std::cout<<"Calling " << function << " at " << __FILE__ << ": at line " << __LINE__ << std::endl;

