#include "logmsg.h"

#include <errno.h>
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <time.h>


extern int strerror_r(int errnum, char *buf, size_t buflen);

void log_msg(int level, const char *file, int linenum, const char *fmt, ...)
{
	if (level < kLogLevelCritical)
		level = kLogLevelCritical;
	else if (level > kLogLevelDebug)
		level = kLogLevelDebug;

	if (level <= kVerbosity) {
		static const char levels[5][4] = {
			"crt",
			"err",
			"wrn",
			"inf",
			"dbg"
		};

		int errsv = errno;
		va_list ap;

		fprintf(stderr, "%ld:%s:%s:%d: ", time(NULL), levels[level], file, linenum);
		va_start(ap, fmt);
		vfprintf(stderr, fmt, ap);
		va_end(ap);

		if ('\n' != fmt[strlen(fmt)-1]) {
			char buffer[LINE_MAX];

			strerror_r(errsv, buffer, sizeof(buffer));
			fprintf(stderr, ":(%d) %s\n", errsv, buffer);
		}

		fflush(stderr);
	}
}
