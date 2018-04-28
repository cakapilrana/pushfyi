/*
*********************************************************
        This code is a proprietary of Kapil Rana

Following may not be used in part or in whole without the
prior permission of Kapil Rana

Author: Kapil Rana
Date: 20/06/2015
Purpose: PUSH FYI RS TCP Reader

*********************************************************
*/
#ifndef __TCP_READER__
#define __TCP_READER__

#include "Runnable.h"
#include "Thread.h"
#include "Mutex.h"
#include "Event.h"
#include "EventList.h"
#include "PriorityQueue.h"
#include "EpollProxy.h"
#include "PubSubRouter.h"
#include "SSLSessionFactory.h"

#define DEFAULT_MAX_CLIENTS_10M			100000

/*!
 * PUSH FYI RS's main TCP Reader Class
 *
 * All incoming connections are routed to this class for further publish
 * and subscribe operations by the TCP SERVER.
 *
 * At any given point of time there could be multiple
 * TCP READER Threads running. Each thread is capable of handling
 * a large number of IO operations. This class uses epoll mechanism
 * to achieve Async IO by multiple clients simultaneously.
 *
 * This class performs following key operations.
 *
 * 1. Validate the client's publish and subscribe keys.
 * 2. Maintains a cache of valid publish and subscribe keys.
 * 3. Serves publish, subscribe and unsubscribe requests.
 */
class TCPReader : public Runnable, EpollProxy,
                    Pushable<Event> {
public:
    typedef std::map<std::string, int> CommandMap;
	static const int DEFAULT_EPOLL_TIME_OUT_MILLIS	= 1000;

private:
    /*!
     * Prevent copy of TCP READER's instance
     */
    TCPReader(const TCPReader&);

    /*!
     * Process the command sent to the TCP Reader.
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
     * subscribe for TCP Reader topic and subtopic to the PubSubRouter
     */
    void subscribe();

    /*!
     * unsubscribe for TCP Reader topic and subtopic to the PubSubRouter
     */
    void unsubscribe();

public:
    /*!
     * TCP READER CONSTRUCTOR
     *
     * @param topic The topic of this instance of the TCP Reader
     * @param subtopic The subtopic of this instance of the TCP Reader
     */
    TCPReader(string);

    /*!
     * Destructor
     *
     * This must always be virtual.
     */
    virtual ~TCPReader();

    /*! TCPReader Thread Function
     *
     *	Implement the TCPReader's main thread.
     */
    void run();

    /*! Start the reader
     *
     *	Method to start the TCP Reader main thread to perform IO
     */
    bool online();

    /*! Stop the reader
     *
     *	Method to stop the TCP Reader main thread. This will stop all IO
     */
    bool offline();

    /*!
     * Process something in idle time.
     *
     * This should not be a very lengthy operation.
     *
     * TODO: This could be a place to submit IO stats to the database.
     * However, in high load scenarios this method may just starve.
     */
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
     * Do not use this method for pushing events on the TCPReader queue without knowing
     * what might happen. Unknowing use of this method can lead to starvation of incoming
     * messages and some other control messages.
     *
     * Avoid it's use unless you know exactly what you are doing.
     */
    virtual void pushFront(Event event);

private:
    /*
     * node name
     */
    string mNodeName;

    /*! Thread object
     *
     * TCP Reader's thread object
     * @see Thread
     */
    Thread*     mThread;

    /*! Synchronization Mutex
     *
     * pthread_mutex_t wrapper
     */
    Mutex       mMutex;

    bool        mFinished;

    /*! TCP Reader's Command Queue
     *
     * TCP Reader can be controlled dynamically by sending command's to this queue.
     *
     * Command event must contain the following parameters:
     *
     * topic = {uuid}
     * subtopic = *
     * command = <command-to-process>
     */
    PriorityQueue<Event>*	mCmdQueue;

    /*!
     * TCP Reader Topic String in UUID Format
     * At any given time there could be multiple TCP Reader's running.
     *
     * Each TCP Reader must have a UUID as it's topic.
     */
    string                  mTopic;

    /*!
     * TCP Reader Subtopic String
     */
    string                  mSubTopic;

    /*!
     * Event Router Instance
     *
     * @see PubSubRouter The main Publish Subscribe Event router.
     *
     * Say the words and remember them : "I would not mess with this class"
     */
    PubSubRouter&   mRouter;

    /*!
     * TCP Reader is notified asynchronously of any commands that have been sent to it.
     *
     * This notification is executed to trigger the processing of commands.
     */
    int 		mPipeFd[2];

    /*! TCP Reader Command List
     *
     * A map of TCP Reader command code and corresponding string value
     */
    CommandMap	mCommandMap;

    /*!
     * Command codes
     */
    enum CommandType {
    	eDefaultCommand =0,
    	eNewClientConnection,
    	eHello,
    	eStatus,
    	eCleanUp
    };

    /*
     * Configurables
     */
    rxUInt		mEpollWaitPeriod;

    /*!
     * This method process the Handshake Message by the client.
     */
    void processHandshake(PushFYIClient* pSocket);

    /*!
     * This method processes a new client request from the TCP Server
     * after the connection has been established and protocol handshaking done.
     */
    PushFYIClient* processNewClient(Event& req);

    /*!
     * This method processes an event triggered by epoll.
     *
     * This event could be a subscribe request from a client or a publish request.
     */
    void processNewEvent(PushFYIClient* pSocket);

    int processClientCommand(PushFYIClient*, Event&);

    /**
     * Clean up dead clients
     */
    void cleanup();

    /*!
     * Destroy the EventSocket
     */
    void expire(PushFYIClient* pSocket);

    /*
     * Generate UUID
     */
    string getUUID();
};


#endif //__TCP_READER__
