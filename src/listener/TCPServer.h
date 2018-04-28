/*
*********************************************************
        This code is a proprietary of Kapil Rana

Following may not be used in part or in whole without the
prior permission of Kapil Rana

Author: Kapil Rana
Date: 20/06/2015
Purpose: PUSH FYI RS SSL TCP Server Listener.

*********************************************************
*/
#ifndef __PLAIN_TCP_SERVER__
#define __PLAIN_TCP_SERVER__

#include <unistd.h>
#include <list>
#include "Runnable.h"
#include "Thread.h"
#include "Mutex.h"
#include "Event.h"
#include "PriorityQueue.h"
#include "ServerSocket.h"
#include "EpollProxy.h"

#define DEFAULT_REPORT_STATS_INTERVAL	5000

class TCPReader;
class PubSubRouter;

/*!
 *
 * Push FYI RS's main SSL TCP Server class.
 *
 * This class listens for incoming connections from unsecure clients.
 * This class performs following key tasks:
 *
 * 1. Listen for incoming connections.
 * 2. Pass the connection to TCPReader for handshake
 *
 */
class TCPServer : public Runnable, public ServerSocket,
                            EpollProxy, Pushable<Event> {
public:
    typedef std::map<std::string, int> CommandMap;

	static const int DEFAULT_EPOLL_TIME_OUT_MILLIS	 = 1000;

private:
    /*!
     * Constructor is private as this is a Singleton class
     */
    TCPServer();

    /*!
     * Copy Constructor is private as this is a Singleton class
     */
    TCPServer(const TCPServer&);

    /*!
     * Self Instance
     */
    static TCPServer* mInstance;

    /*!
     * Process the command sent to the TCP Server.
     *
     * This method is called when notification pipe is triggered. It then fetches the
     * commands present in the command queue and process them.
     */
    void processCmd();

    /*!
     * Initialize the list of commands and corresponding codes.
     */
    void initializeCommandMap();

    /*!
     * This method triggers the notification pipe by writing to it's write end.
     */
    void notify();

    /*!
     * Pushes the command depending upon it's priority.
     * @see push
     * @see pushFront
     */
    void submitCmd(Event event, bool isHighPriority = false);

    /*!
     * subscribe for TCP Server topic and subtopic to the PubSubRouter
     */
    void 	subscribe();

    /*!
     * unsubscribe for TCP Server topic and subtopic to the PubSubRouter
     */
    void 	unsubscribe();

    /*!
     * internal method to generate uuid
     */
    string getUUID();

public:

    /*!
     * Interface method to get the class Instance
     */
    static TCPServer* getInstance();

    /*!
     * Destructor
     *
     * This must always be virtual.
     */
    virtual ~TCPServer();

    /*!
     *  This is a concrete implementation of ServerSocket processNewClientSocket
     *	@see ServerSocket Server Socket wrapper class
     *
     *  @param socketFd Socket file descriptor returned by call to accept
     */
    //void processNewClientSocket(int socketFd);
    void processNewClientSocket(int socketFd, char* ip, rxUShort port);

    /*!
     *  Override this method to implement ServerSocket::Accept yourself
     */
    void Accept();

    /*! TCPServer Thread Function
     *
     *	Implement the TCPServer's main thread.
     */
    void run();

    /*! Start the server
     *
     *	Method to start the TCP Server main thread and start listening
     */
    bool online();

    /*! Stop the server
     *
     *	Method to stop the TCP Server main thread
     */
    bool offline();

    void idle(int);

    /*!
     * Concrete implementation for Pushable::push
     *
     * @param event Request event to be pushed
     */
    virtual void push(Event event);

    /*!
     * Concrete implementation for Pushable::pushFront to push high priority messages.
     * @see Pushable Pushable interface
     *
     * @param event Request event to be pushed.
     *
     * Do not use this method for pushing events on the TCPServer queue without knowing what might happen.
     * Unknowing use of this method can lead to starvation of incoming messages and some other control messages.
     *
     * Avoid it's use unless you know exactly what you are doing.
     */
    virtual void pushFront(Event event);

    /**
     * Set alternate port
     */
    void setAlternatePort(rxInt port);

private:

    /*! Thread object
     *
     * TCP Sever's thread object
     * @see Thread
     */
    Thread*     mThread;

    /*! Synchronization Mutex
     *
     * pthread_mutex_t wrapper
     */
    Mutex       mMutex;

    /*! TCP Server Command Queue
     *
     * TCP Server can be controlled dynamically by sending command's to this queue.
     *
     * Command event must contain the following parameters:
     *
     * topic = pushfyirs-tcp-server
     * subtopic = *
     * command = <command-to-process>
     */
    PriorityQueue<Event>*	mCmdQueue;

    /*!
     * TCP Server Topic String
     */
    string                  mTopic;

    /*!
     * TCP Server is notified asynchronously of any commands that have been sent to it.
     *
     * This notification is executed to trigger the processing of commands.
     */
    int 		mPipeFd[2];

    /*! TCP Server Command List
     *
     * A map of TCP Server command code and corresponding string value
     */
    CommandMap	mCommandMap;

    /*!
     * Command codes
     */
    enum CommandType {
    	eReaderOnline = 0,
    	eStatus
    };

    /*!
     * Class to perform Push FYI Handshake and identify the protocol used by client.
     *
     * @see PushFYIProtocol Check out the class definition for all the protocols supported.
     */
    //PushFYIProtocol    mProtocol;

    /*!
     * Event Router Instance
     *
     * @see PubSubRouter The main Publish Subscribe Event router.
     *
     * Say the words and remember them : "I would not mess with this class"
     */
    PubSubRouter& mRouter;

    /*
     * is Alternate Port on
     */
    bool mIsAlternatePort;

    /*
     * TCPReader instances
     */
    std::vector<string>	mReaders;
    rxUInt mReaderIndex;

    /*
     * Server Diagnostics
     */
    rxULong _StartTime;
    rxULong _ConnectionAccepts;
    rxULong _IdleCycles;

    void processReaderOnline(Event&);
};

#endif //__SECURE_TCP_SERVER__
