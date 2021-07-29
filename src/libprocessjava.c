#define _GNU_SOURCE
#include <unistd.h>
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "log.h"
#include "conf.h"

#define HOOK_COMMOND "java"

#define HOOK_NAME ".jar"

#define HOOK_NAME_PARAM_ONLY "-jar"

#define SKYWALKING_CFG_PATH "/etc/skywalking.cnf"

#define SKYWALKING_CLIENT "skywalking_client"
#define SKYWALKING_SERVER "skywalking_server"

#define SW_AGENT_PATH_KEY "SW_AGENT_PATH"

#define SW_AGENT_NAME "SW_AGENT_NAME"

#define SW_AGENT_PATH "-javaagent:/data/skywalkingoneagent/apache-skywalking-apm-bin/agent/skywalking-agent.jar"

typedef ssize_t (*execve_func_t)(const char *filename, char *const argv[], char *const envp[]);
static execve_func_t old_execve = NULL;

int stringEndWith(const char *source, const char *target)
{
  int sourceLen = strlen(source);
  int targetLen = strlen(target);
  int toffset = sourceLen - targetLen;
  int po = 0;
  if ((toffset < 0) || toffset > sourceLen - targetLen)
  {
    return -1;
  }
  while (--targetLen >= 0)
  {
    if (source[toffset++] != target[po++])
    {
      return -1;
    }
  }
  return 0;
}

int printArgs(char *const argv[])
{
  int len = 0;
  while (argv[len] != NULL)
  {
    printf("%s ", argv[len]);
    len++;
  }
  return len;
}

// 修改参数，构建新的参数返回
char **modifyArgv(char *const argv[], int length, char *hookName)
{
  char valueNew[1024] = {SW_AGENT_PATH};
  char **newParam = (char **)malloc((length + 1) * sizeof(char *));
  if (newParam == NULL)
  {
    printf("modify params error\n");
    return NULL;
  }
  ReadPrivateProfileString(SKYWALKING_CFG_PATH, SKYWALKING_CLIENT, SW_AGENT_PATH_KEY, valueNew, 1024);
  int len = 0;
  int index = 0;
  for (len = 0; len <= length; len++)
  {

    int size = strlen(argv[index]);
    if (size <= 0)
    {
      continue;
    }

    if (len == 1)
    {
      newParam[len] = (char *)malloc(strlen(valueNew) + 2);
      printf("agent path%s|312312312\n", valueNew);
      if (newParam[len] == NULL)
      {
        printf("modify params error  newParam[len]:%s len:%d\n", newParam[len], len);
        continue;
      }
      memset(newParam[len], 0, (size_t)sizeof(newParam[len]));
      strcpy(newParam[len], valueNew); // jar
      printf("newParam agent path%s|312321\n", newParam[len]);
      continue;
    }

    newParam[len] = (char *)malloc(size + 1);
    if (newParam[len] == NULL)
    {
      printf("modify params error ... newParam[len]:%s len:%d\n", newParam[len], len);
      continue;
    }
    memset(newParam[len], 0, (size_t)sizeof(newParam[len]));
    printf("----------: %s\n", argv[index]);
    if (stringEndWith(argv[index], HOOK_NAME) == 0)
    {
      printf("------------hook_name: %s\n", argv[index]);
      strcpy(hookName, argv[index]);
    }
    strcpy(newParam[len], argv[index]);
    index++;
  }
  newParam[len] = NULL;
  return newParam;
}

char **getEnv(char *const envp[], int length, char *skywalkingName)
{
  char **newParam = (char **)malloc((length + 6) * sizeof(char *));

  if (newParam == NULL)
  {
    printf("modify params error\n");
    return NULL;
  }
  int len = 0;
  for (len = 0; len < length; len++)
  {
    int size = strlen(envp[len]);
    if (size <= 0) // 防止中间有字符串为NULL
    {
      continue;
    }
    newParam[len] = (char *)malloc(strlen(envp[len]) + 1);
    memset(newParam[len], 0, (size_t)sizeof(newParam[len]));
    strcpy(newParam[len], envp[len]);
  }

  char **valueNew = (char **)malloc((55) * sizeof(char *));
  if (valueNew == NULL)
  {
    printf("modify params error\n");
    return NULL;
  }
  memset(valueNew, 0, (size_t)sizeof(valueNew));
  int i = 0;
  for (i = 0; i < 5; i++)
  {
    valueNew[i] = (char *)malloc(100 * sizeof(char));
    if (valueNew[i] == NULL)
    {
      continue;
    }
    memset(valueNew[i], 0, (size_t)sizeof(valueNew[i]));
  }
  int size = 0;
  ReadCfgSize(SKYWALKING_CFG_PATH, SKYWALKING_SERVER, valueNew, &size);
  strcat(valueNew[size], "SW_AGENT_NAME");
  strcat(valueNew[size], "=");
  strcat(valueNew[size], skywalkingName);
  int flag = 0;
  for (flag = 0; flag <= size; flag++)
  {
    newParam[len] = (char *)malloc(strlen(valueNew[flag]) + 1);

    if (newParam[len] == NULL)
    {
      continue;
    }
    memset(newParam[len], 0, (size_t)sizeof(newParam[len]));
    strcpy(newParam[len], valueNew[flag]);
    len++;
  }
  newParam[len] = NULL;
  printf("\ngetEnv end\n");
  return newParam;
}

int judge(char *const argv[])
{
  int len = 0;
  while (argv[len] != NULL)
  {
    if (strcmp(argv[len], HOOK_NAME_PARAM_ONLY) == 0)
    {
      return 0;
    }
    len++;
  }
  return len;
}

int execve(const char *filename, char *const argv[], char *const envp[])
{

  if (stringEndWith(filename, HOOK_COMMOND) == 0 && judge(argv) == 0)
  {
    printf("the raw command line: ");
    int len = printArgs(argv);
    char hookName[1024] = {0};
    char **argvs = modifyArgv(argv, len, hookName);
    printf("\nthe command line after modification:\n");
    len = printArgs(argvs);
    printf("\nthe raw evn line:");
    len = printArgs(envp);
    char **envs = getEnv(envp, len, hookName);
    printf("\nthe raw evn line after modification:\n");
    printArgs(envs);
    printf("\nprintArgs after");
    old_execve = dlsym(RTLD_NEXT, "execve");
    extern char **environ;
    return old_execve(filename, argvs, envs);
  }
  old_execve = dlsym(RTLD_NEXT, "execve");
  return old_execve(filename, argv, envp);
}
