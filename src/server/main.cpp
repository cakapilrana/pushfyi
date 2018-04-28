/*
*********************************************************
        This code is a proprietary of Kapil Rana

Following may not be used in part on in whole without the
prior permission of Kapil Rana

Author: Kapil Rana
Date: 20/06/2015
Purpose: PYSHFYI-RS Server Entry Point

*********************************************************
*/
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "ProcessInit.h"
#include "Server.h"
#include "PubSubRouter.h"
#include "Log.h"
#include "LogId.h"
#include "CachedLog.h"

using namespace std;

static void printUsage(char* name) {
	printf("%s <config filename>\n", name);
}

static void printVersion(char* name) {
	printf("PushFYI Websocket Gateway V 1.0 (wolverine)\n");
}

int main(int argc, char* argv[])
{
    bool _daemon = false;

	if (argc==1 || strcmp(argv[1], "-h")==0 || strcmp(argv[1], "--help")==0 ) {
		printUsage(argv[0]);
		exit(0);
	}

	if (argc==1 || strcmp(argv[1], "-v")==0 || strcmp(argv[1], "--version")==0 ) {
		printVersion(argv[0]);
		exit(0);
	}

	if (argc==1 || strcmp(argv[1], "-d")==0 || strcmp(argv[1], "--daemon")==0 ) {
        _daemon = true;
	}

	//Start the daemon process
	if(_daemon) {
        pid_t pid;
        pid = fork();
        if(pid < 0) {
            exit(EXIT_FAILURE);
        }

        if(pid > 0) {
            //we got a good pid. lets exit the parent
            exit(EXIT_SUCCESS);
        }

        //change the file mode mask
        umask(0);
	}
	//lets init our process and start the logger
	ProcessInit* processInit = ProcessInit::instance();
	bool success = processInit->startup(argc, argv);

	if(!success) {
		cerr << "Process startup has failed. exiting.";
		return 1;
	}
	LogId::init();
    CachedLog* log = CachedLog::init();
    log->online();

    //Start the pub sub router
    PubSubRouter& router = PubSubRouter::getRouter();
    router.online();

    if(_daemon) {
        //create the new sid for the child process.
        pid_t sid = setsid();
        if(sid < 0) {
            exit(EXIT_FAILURE);
        }
        INFO(Log::eProcess, "New SID Created = %d", sid);
        //we have to change the working directory
        //close the standard file descriptors
        close(STDIN_FILENO);
        //close(STDOUT_FILENO);
        //close(STDERR_FILENO);
    }

    //cout << "pubnet process started\n";
    INFO(Log::eProcess, "..............................................................");
    INFO(Log::eProcess, "PUSHFYI REAL SERVER DAEMONIZED. INITIALIZING ALL SUB SYSTEMS");
    INFO(Log::eProcess, "..............................................................");

    Server* server = Server::getInstance();
    server->online();

    return 0;
}
