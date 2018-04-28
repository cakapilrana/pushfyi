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
#ifndef __CLIENT_FACTORY__
#define __CLIENT_FACTORY__

#include "Runnable.h"
#include "Thread.h"
#include "Mutex.h"
#include "Event.h"
#include "PriorityQueue.h"
#include "EpollProxy.h"

class PubSubRouter;

/*!
 *
 *
 */
class ClientFactory : public Runnable, EpollProxy,
								Pushable<Event> {
public:
    typedef std::map<std::string, int> CommandMap;

	static const int DEFAULT_EPOLL_TIME_OUT_MILLIS = 3000;

private:
    /*!
     * Constructor is private as this is a Singleton class
     */
    ClientFactory();

    /*!
     * Copy Constructor is private as this is a Singleton class
     */
    ClientFactory(const ClientFactory&);

    /*!
     * Self Instance
     */
    static ClientFactory* mInstance;

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

public:

    /*!
     * Interface method to get the class Instance
     */
    static ClientFactory* getInstance();

    /*!
     * Destructor
     *
     * This must always be virtual.
     */
    virtual ~ClientFactory();


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

    /*
     * Create a new client
     */
    PushFYIClient*	CreateClient(Event&);

private:

    bool mFinished;

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
    	eSetEpollWait = 0
    };

    /*!
     * Event Router Instance
     *
     * @see PubSubRouter The main Publish Subscribe Event router.
     *
     * Say the words and remember them : "I would not mess with this class"
     */
    PubSubRouter& mRouter;

    std::list<PushFYIClient*> mClients;
};

#endif //__SECURE_TCP_SERVER__
