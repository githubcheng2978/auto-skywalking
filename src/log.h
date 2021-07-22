#ifndef _LOG_H_
#define _LOG_H_

extern int  LogLevel[5];
void LOG(const char *file, int line, int level, int status, const char *fmt, ...);

#endif
