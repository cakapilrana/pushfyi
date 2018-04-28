/*
*********************************************************
        This code is a proprietary of iN Technologies

Following may not be used in part on in whole without the
prior permission of iN Technologies

Author: Kapil Rana
Date: 20/06/2015
Purpose: PUBNET Server Main File

*********************************************************
*/

#ifndef __SERVER_H__
#define __SERVER_H__

#define OPENSSL_THREAD_DEFINES
 #include <openssl/opensslconf.h>
 #if defined(OPENSSL_THREADS)

 #else
   #error OPENSSL Multithread Support Required
 #endif


#include <string>
#include "ConditionVar.h"
#include "Mutex.h"

using namespace std;

class Server {
public:
	virtual ~Server();
	static Server* getInstance();

	bool online();
	bool offline();

private:
	Server();
	Server(const Server&);

	static Server* mInstance;

	/**
	 * Initialize openssl
	 * MT pthread
	 *
	 * Reference: crypto/threads/mttest.c
	 */
	void initializeSSL();

	/*
	 * UUID Generator
	 */
	string getUUID();

	/**
	 * Enable openssl MT Support
	 */
	static Mutex**		mLocks;
	static rxLong*		mLockCount;

	static void initssllocks();

	/**
	 * Locking callback
	 */
	static void pthreads_locking_callback(int mode, int type, const char *file,
		     int line);

	/**
	 * Thread id callback
	 */
	static unsigned long pthreads_thread_id(void);


	bool			mFinished;
	ConditionVar 	mRunning;
	Mutex			mMutex;
};

#endif //__SERVER_H__
