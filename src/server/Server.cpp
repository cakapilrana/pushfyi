/*
*********************************************************
        This code is a proprietary of iN Technologies

Following may not be used in part on in whole without the
prior permission of iN Technologies

Author: Kapil Rana
Date: 20/06/2015
Purpose: PUSHFYI-RS Server Main File

*********************************************************
*/

#include <openssl/ssl.h>
#include <uuid/uuid.h>
#include <curl/curl.h>
#include "Server.h"
#include "Properties.h"
#include "ProcessInit.h"
#include "Time.h"
#include "SystemTimer.h"
#include "Pushfyi-Defs.h"
#include "PushFYIProtocolFactory.h"
#include "PushFYIMgr.h"
#include "TCPServer.h"
#include "SecureTCPServer.h"
#include "TCPReader.h"
#include "ClientFactory.h"

Server* Server::mInstance = 0;
Mutex** Server::mLocks = 0;
rxLong* Server::mLockCount = 0;

Server::Server() : mFinished(false)
{
}

Server::~Server()
{
	curl_global_init(CURL_GLOBAL_ALL);
}

Server* Server::getInstance()
{
	if( mInstance == 0 ) {
		mInstance = new Server();
	}
	return mInstance;
}

bool Server::online()
{
	Properties* sysProps = ProcessInit::instance()->getSysProps();

	/*
	 * Initialize Pushfyi Protocol Factory
	 *
	 * Pushfyi protocols:
	 *
	 * 1. WebSocket Protocol
	 * 		RFC 6455 Compliant
	 * 		Clients: Browsers
	 * 2. Stream Socket Protocol
	 * 		Plain Stream Socket Protocol
	 * 		Clients: Native Apps
	 *
	 * TLS must be used irrespective of the Protocol
	 * in use.
	 */
	INFO(Log::eProcess, "Initializing Pushfyi Protocol Factory");
	PushFYIProtocolFactory* factory = PushFYIProtocolFactory::getInstance();

	INFO(Log::eProcess, "Initializing Pushfyi Monotonic Clock");
	Time* clock = Time::instance();
	clock->online();

	INFO(Log::eProcess, "Initializing Pushfyi System Timer");
    SystemTimer* sysTimer = SystemTimer::instance();
    sysTimer->online();

	INFO(Log::eProcess, "Initializing SSL With MT Support");
	initializeSSL();

    INFO(Log::eProcess, "Initializing Pushfyi Manager");
    PushFYIManager* Mgr = PushFYIManager::getInstance();
    Mgr->online();

    INFO(Log::eProcess, "Initializing Client Factory");
    ClientFactory* Fac = ClientFactory::getInstance();
    Fac->online();

    /*
     * Start the listeners.
     *
     * 1. If we have http alertnate port defined
     * than we will listen on that port for
     * both secure and insecure clients.
     *
     * 2. Else we will lsiten on standard
     * http port 80 and ssl port 443
     *
     */
    bool alternatePort = sysProps->contains("http.alternate.port");
    rxInt httpPort = 80;

    if(alternatePort) {
    	httpPort = sysProps->getInt("http.alternate.port");

        INFO(Log::eProcess, "Initializing HTTP Listener on Alternate HTTP Port %d", httpPort);
        TCPServer* server = TCPServer::getInstance();
        server->setAlternatePort(httpPort);
        server->online();
    }

    else {

        INFO(Log::eProcess, "Initializing TCP Listener on Standard HTTP Port %d", httpPort);
        TCPServer* server = TCPServer::getInstance();
        server->online();

        INFO(Log::eProcess, "Initializing Secure TCP Listener");
        SecureTCPServer* tlsserver = SecureTCPServer::getInstance();
        tlsserver->online();

    }

    rxUInt readers = 1;
    if(sysProps->contains("tcp.reader.threads"))
    {
    	readers = sysProps->getUInt("tcp.reader.threads");
    }

	INFO(Log::eProcess, "%d Reader threads will be started", readers);

    /*
     * Lets start TCP reader threads.
     */
    for(rxUInt i=0; i<readers; i++)
    {
    	string topic = getUUID();
		INFO(Log::eProcess, "Initializing TCP Reader With Topic = %s", topic.c_str());
		TCPReader* reader = new TCPReader(topic);
		reader->online();
    }

    /*
    INFO(Log::eProcess, "Initializing Pushfyi Listener");
    TCPServer* server = TCPServer::getInstance();
    server->online();

    INFO(Log::eProcess, "Initializing Pushfyi TLS Listener");
    SecureTCPServer* tlsserver = SecureTCPServer::getInstance();
    tlsserver->online();

    INFO(Log::eProcess, "Initializing Pushfyi TCP Reader");
    TCPReader* reader = new TCPReader(PUSHFYI_TLS_READER_TOPIC);
    reader->online();
	*/

    sleep(5);

    INFO(Log::eProcess, "..............................................................");
    INFO(Log::eProcess, "ALL SUBSYSTEMS OPERATIONAL. WE ARE UP AND RUNNING. HURRAY!!!!!");
    INFO(Log::eProcess, "..............................................................");

    /*
     * Go on in an infinite wait
     */
    Synchronized lock(mMutex);
    while(!mFinished) {
    	mRunning.wait(lock);
    }
	return true;
}

bool Server::offline()
{
	//TODO
	return true;
}

void Server::initializeSSL()
{
	/*
	 * Initialize openssl library
	 */
	SSL_library_init();
	OpenSSL_add_all_algorithms();
    SSL_load_error_strings();

    /*
     * Initialize the SSLSessionFactory.
     *
     * This must be done prior to starting any thread
     * that uses SSL
     */
    SSLSessionFactory* sessionFactory = SSLSessionFactory::getInstance();
    initssllocks();
}

void Server::initssllocks()
{
	int i;

	mLocks = (Mutex**)OPENSSL_malloc(CRYPTO_num_locks() * sizeof(Mutex));
	mLockCount = (rxLong*)OPENSSL_malloc(CRYPTO_num_locks() * sizeof(rxLong));

	for (i=0; i<CRYPTO_num_locks(); i++)
	{
		mLockCount[i] = 0;
		mLocks[i] = new Mutex();
		//pthread_mutex_init(&(lock_cs[i]),NULL);
	}

	CRYPTO_set_id_callback((unsigned long (*)())pthreads_thread_id);
	CRYPTO_set_locking_callback((void (*)(int, int, const char*, int))pthreads_locking_callback);
}

void Server::pthreads_locking_callback(int mode, int type, const char *file,
	     int line)
{
	if (mode & CRYPTO_LOCK) {
		mLocks[type]->lock();
		mLockCount[type]++;

		//pthread_mutex_lock(&(lock_cs[type]));
		//lock_count[type]++;
	}
	else {
		mLocks[type]->unlock();
		//pthread_mutex_unlock(&(lock_cs[type]));
	}
}

unsigned long Server::pthreads_thread_id(void)
{
	unsigned long ret;

	ret=(unsigned long)pthread_self();
	return(ret);
}

/**
 * Generate UUID
 */
string Server::getUUID()
{
    char   uuidStr[64] = {0};
    uuid_t uuid;

    ::uuid_generate(uuid);
    ::uuid_unparse(uuid, uuidStr);

    return uuidStr;
}

