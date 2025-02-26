


#pragma once

#include "AdvFormat.h"
#include "Logging/LogMacros.h"

struct AAAMECHANICS_API AdvLogger {
	static void Output(int32 key, float timeToDisplay, const FString& msg);

	template<class... ARGS> inline
	static void _LogInternal(int32 key, float timeToDisplay, const char* format_str, const ARGS& ... args) {
		FString tmp;
		fmt::format_to(AdvFormat_FStringBackInserter(tmp), format_str, args...);
		Output(key, timeToDisplay, tmp);
	}
};

#define SHOW(...)  do { AdvLogger::_LogInternal(INDEX_NONE, 3,  __VA_ARGS__); } while(false)


#define  ADV_ASSERT(X) check(X)