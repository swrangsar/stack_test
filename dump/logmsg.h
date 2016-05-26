#ifndef LOG_MSG_H_
#define LOG_MSG_H_

enum {
	LOG_LEVEL_CRIT=0,
	LOG_LEVEL_ERR,
	LOG_LEVEL_WARN,
	LOG_LEVEL_INFO,
	LOG_LEVEL_DBG
};

void log_msg(unsigned int level, const char *file, int line, const char *func, const char *fmt, ...);

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
#define STRFUNC		((const char *) (__func__))
#elif defined(__GNUC__) || (defined(_MSC_VER) && (_MSC_VER > 1300))
#define STRFUNC		((const char *) (__FUNCTION__))
#else
#define STRFUNC		((const char *) ("???"))
#endif


#define log_err(...)	log_msg(LOG_LEVEL_ERR, __FILE__, __LINE__, STRFUNC, __VA_ARGS__)
#define log_warn(...)	log_msg(LOG_LEVEL_WARN, __FILE__, __LINE__, STRFUNC, __VA_ARGS__)
#define log_info(...)	log_msg(LOG_LEVEL_INFO, __FILE__, __LINE__, STRFUNC, __VA_ARGS__)
#define debug(...)	log_msg(LOG_LEVEL_DBG, __FILE__, __LINE__, STRFUNC, __VA_ARGS__)

#define sentinel(...)      {log_err(__VA_ARGS__); goto error;}

#endif
