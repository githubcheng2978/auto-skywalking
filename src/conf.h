#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <stdbool.h>

bool ReadPrivateProfileString(const char *fileName, const char *session, const char *key, char *value, const int valueBufferSize);
bool ReadCfgSize(const char *fileName, const char *session, char **value,int* foundSize);

bool RevisePrivateProfileString(const char *fileName, const char *session, const char *key, const char*newValue);

#endif
