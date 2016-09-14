#include "logmsg.h"
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <time.h>


#ifdef DEBUG
static const unsigned int verbosity = LOG_LEVEL_DBG;
#else
static const unsigned int verbosity = LOG_LEVEL_INFO;
#endif

extern int strerror_r(int errnum, char *buf, size_t buflen);

void log_msg(unsigned int level, const char *file, int linenum, const char *fmt, ...)
{
	if (level < LOG_LEVEL_CRIT)
		level = LOG_LEVEL_CRIT;
	else if (level > LOG_LEVEL_DBG)
		level = LOG_LEVEL_DBG;

	if (level <= verbosity) {
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
			char buffer[512];

			strerror_r(errsv, buffer, sizeof(buffer));
			fprintf(stderr, ":(%d) %s\n", errsv, buffer);
		}

		fflush(stderr);
	}
}
