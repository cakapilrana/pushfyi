/*
*********************************************************
        This code is a proprietary of iN Technologies

Following may not be used in part on in whole without the
prior permission of iN Technologies

Author: Kapil Rana
Date: 20/06/2015
Purpose: PUBNET Logging Utility

*********************************************************
*/
#include <string.h>
#include "Log.h"

using namespace std;

const int Log::sModule_len = 15;
const int Log::sLevel_len = 8;

const char* Log::sModules[sModule_len] = {
	"logger", 	"process", 	"pubsub", 	"network",
	"database", "tcp-server", "secure-tcp-server", "tcp-reader", "pushfyi-protocol", "pushfi-manager",
	"cluster-manager", "client-factory", "soap",  "auth",  "unknown"
};


const char* Log::sLevels[sLevel_len] = {
        "alert", "critical", "error", "warning",
        "info",  "debug", "finer",    "finest"
};

Log* Log::sLog = 0;


Log::Log()
{
}

Log::~Log()
{
}

const char* Log::getModuleString(eLogModule module)
{
    rxInt i = (module < sModule_len) ?
    		module :
    		(sModule_len-1);

    return sModules[i];
}

const char* Log::getLogLevelString(eLogLevel level)
{
    rxInt i = (level < sLevel_len) ?
    		level :
    		(sLevel_len-1);

    return sLevels[i];
}

Log::eLogLevel Log::getLogLevel(const char* sLevel)
{
	if(sLevel == 0){
		return Log::eAlert;
	}

	for (int i=0; i<sLevel_len; ++i){
		if(strcmp(sLevels[i], sLevel)==0){
			return (Log::eLogLevel)i;
		}
	}
    return Log::eAlert;
}


