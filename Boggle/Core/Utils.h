#pragma once

#ifndef UTILS_H__
#define UTILS_H__

void BreakPointCall();
#define GENERIC_ASSERT_TEXT "We appologies but something un-expected just happen."
#	define ASSERT_RELEASE(cond, text)																	\
	if (!(cond))																						\
	{																									\
		printf("ASSERT(%s) in %s at line %d with message: %s\n", #cond, __FILE__, __LINE__, text);		\
		BreakPointCall ();																				\
	}

#define ASSERT_ONCE_RELEASE(cond, text)																	\
{																										\
	static bool HasAsserted = false;																	\
	if ((false == HasAsserted) && (!(cond)))															\
	{																									\
		HasAsserted = true;																				\
		printf("ASSERT_ONCE(%s) in %s at line %d with message: %s\n", #cond, __FILE__, __LINE__, text);	\
	}																									\
}

#if  _WIN64
#	define BREAKPOINT_RELEASE {}
#else
#	define BREAKPOINT_RELEASE __asm int 3;
#endif

#ifdef _DEBUG
#	define BREAKPOINT BREAKPOINT_RELEASE
#	define ASSERT(cond, text) ASSERT_RELEASE(cond, text)
#	define ASSERT_ONCE(cond, text) ASSERT_ONCE_RELEASE (cond, text)
#else
#	define BREAKPOINT {}
#	define ASSERT(cond, text) {}
#	define ASSERT_ONCE(cond, text) {}

#endif // _DEBUG

#define ARRAY_SIZE(what) (sizeof(what)/sizeof(what[0]))


#endif // !UTILS_H__
