#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "log.h"

#define SKYWALKING_LOG_FILE	"/root/skywalkingagent.log"
#define SKYWALKING_MAX_STRING_LEN 		10240

#define IC_NO_LOG_LEVEL			0
#define IC_DEBUG_LEVEL			1
#define IC_INFO_LEVEL			2
#define IC_WARNING_LEVEL		3
#define IC_ERROR_LEVEL			4

int  LogLevel[5] = {IC_NO_LOG_LEVEL, IC_DEBUG_LEVEL, IC_INFO_LEVEL, IC_WARNING_LEVEL, IC_ERROR_LEVEL};

char ICLevelName[5][10] = {"NOLOG", "DEBUG", "INFO", "WARNING", "ERROR"};

static int Error_GetCurTime(char* strTime){
	struct tm*		tmTime = NULL;
	size_t			timeLen = 0;
	time_t			tTime = 0;	
	
	tTime = time(NULL);
	tmTime = localtime(&tTime);
	//timeLen = strftime(strTime, 33, "%Y(Y)%m(M)%d(D)%H(H)%M(M)%S(S)", tmTime);
	timeLen = strftime(strTime, 33, "%Y.%m.%d %H:%M:%S", tmTime);
	
	return timeLen;
}

static int Error_OpenFile(int* pf)
{
	char	fileName[1024];
	
	memset(fileName, 0, sizeof(fileName));
    sprintf(fileName, "%s",SKYWALKING_LOG_FILE);
    *pf = open(fileName, O_WRONLY|O_CREAT|O_APPEND, 0666);
    if(*pf < 0)
    {
        return -1;
    }
	
	return 0;
}

static void Error_Core(const char *file, int line, int level, int status, const char *fmt, va_list args)
{
    char str[SKYWALKING_MAX_STRING_LEN];
    int	 strLen = 0;
    char tmpStr[64];
    int	 tmpStrLen = 0;
    int  pf = 0;

    memset(str, 0, SKYWALKING_MAX_STRING_LEN);
    memset(tmpStr, 0, 64);
    
   
    tmpStrLen = Error_GetCurTime(tmpStr);
    tmpStrLen = sprintf(str, "[%s] ", tmpStr);
    strLen = tmpStrLen;

   
    tmpStrLen = sprintf(str+strLen, "[%s] ", ICLevelName[level]);
    strLen += tmpStrLen;
    

    if (status != 0) 
    {
        tmpStrLen = sprintf(str+strLen, "[ERRNO is %d] ", status);
    }
    else
    {
    	tmpStrLen = sprintf(str+strLen, "[SUCCESS] ");
    }
    strLen += tmpStrLen;


    tmpStrLen = vsprintf(str+strLen, fmt, args);
    strLen += tmpStrLen;


    tmpStrLen = sprintf(str+strLen, " [%s]", file);
    strLen += tmpStrLen;

    tmpStrLen = sprintf(str+strLen, " [%d]\n", line);
    strLen += tmpStrLen;
    

    if(Error_OpenFile(&pf))
	{
		return ;
	}
	

    write(pf, str, strLen);

    

    close(pf);
    
    return ;
}


void LOG(const char *file, int line, int level, int status, const char *fmt, ...)
{
    va_list args;
	

	if(level == IC_NO_LOG_LEVEL)
	{
		return ;
	}
	

    va_start(args, fmt);
    Error_Core(file, line, level, status, fmt, args);
    va_end(args);
    
    return ;
}
