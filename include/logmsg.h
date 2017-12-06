#ifndef LOG_MSG_H_
#define LOG_MSG_H_

enum {
	kLogLevelCritical = 0,
	kLogLevelError,
	kLogLevelWarning,
	kLogLevelInfo,
	kLogLevelDebug
};

#ifdef DEBUG
static const int kVerbosity = kLogLevelDebug;
#else
static const int kVerbosity = kLogLevelInfo;
#endif

void log_msg(int level, const char *file, int line, const char *fmt, ...);

#define log_err(...)	log_msg(kLogLevelError, __FILE__, __LINE__, __VA_ARGS__)
#define log_warn(...)	log_msg(kLogLevelWarning, __FILE__, __LINE__, __VA_ARGS__)
#define log_info(...)	log_msg(kLogLevelInfo, __FILE__, __LINE__, __VA_ARGS__)
#define debug(...)	log_msg(kLogLevelDebug, __FILE__, __LINE__, __VA_ARGS__)

#define sentinel(...)      {log_err(__VA_ARGS__); goto error;}

#endif  /* LOG_MSG_H_ */
