#pragma once
#include <time.h>

static void delay_ms(long milliseconds) {
	struct timespec ts;
	ts.tv_sec = milliseconds / 1000;
	ts.tv_nsec = (milliseconds % 1000) * 1000000;
	nanosleep(&ts, NULL);
}

// GNUC specific btw.. (clang supports it tho)
#define check_ret(expr)					\
({										\
	int check_ret_result = (expr);		\
	if (check_ret_result < 0) {			\
		perror(#expr " failed");		\
		assert(check_ret_result >=0);	\
		abort();						\
	} 									\
	check_ret_result;					\
})

